<?php
/**
 * Example 02: Compute Shader — Multiply array by 2
 *
 * Demonstrates the full Vulkan compute pipeline in PHP:
 * - Create device and queue
 * - Allocate buffers and memory
 * - Load a SPIR-V compute shader
 * - Dispatch compute work
 * - Read results back
 *
 * Prerequisites: compile the shader first:
 *   glslangValidator -V examples/shaders/multiply.comp -o examples/shaders/multiply.comp.spv
 */

use Vk\Vk;

// --- Setup ---
$instance = new Vk\Instance(appName: 'PHPolygon Compute');
$devices = $instance->getPhysicalDevices();
$physicalDevice = $devices[0];

echo "Using device: " . $physicalDevice->getName() . "\n";

// Find a compute-capable queue family
$queueFamilies = $physicalDevice->getQueueFamilies();
$computeFamilyIndex = null;
foreach ($queueFamilies as $family) {
    if ($family['compute']) {
        $computeFamilyIndex = $family['index'];
        break;
    }
}

if ($computeFamilyIndex === null) {
    echo "No compute-capable queue family found!\n";
    exit(1);
}

$device = new Vk\Device($physicalDevice, [
    ['familyIndex' => $computeFamilyIndex, 'count' => 1],
]);

$queue = $device->getQueue($computeFamilyIndex);

// --- Data ---
$elementCount = 64;
$bufferSize = $elementCount * 4; // 32-bit floats

// Input data: [1.0, 2.0, 3.0, ..., 64.0]
$inputData = '';
for ($i = 0; $i < $elementCount; $i++) {
    $inputData .= pack('f', $i + 1.0);
}

// --- Buffers ---
$inputBuffer = new Vk\Buffer($device, $bufferSize,
    Vk::BUFFER_USAGE_STORAGE_BUFFER | Vk::BUFFER_USAGE_TRANSFER_DST);
$outputBuffer = new Vk\Buffer($device, $bufferSize,
    Vk::BUFFER_USAGE_STORAGE_BUFFER | Vk::BUFFER_USAGE_TRANSFER_SRC);

// --- Memory ---
// Find host-visible memory type
$memProps = $physicalDevice->getMemoryProperties();
$inputReqs = $inputBuffer->getMemoryRequirements();
$outputReqs = $outputBuffer->getMemoryRequirements();

$memTypeIndex = null;
foreach ($memProps['types'] as $i => $type) {
    if ($type['hostVisible'] && $type['hostCoherent']
        && ($inputReqs['memoryTypeBits'] & (1 << $i))
        && ($outputReqs['memoryTypeBits'] & (1 << $i))) {
        $memTypeIndex = $i;
        break;
    }
}

if ($memTypeIndex === null) {
    echo "No suitable memory type found!\n";
    exit(1);
}

$inputMemory = new Vk\DeviceMemory($device, $inputReqs['size'], $memTypeIndex);
$outputMemory = new Vk\DeviceMemory($device, $outputReqs['size'], $memTypeIndex);

$inputBuffer->bindMemory($inputMemory);
$outputBuffer->bindMemory($outputMemory);

// Upload input data
$inputMemory->map();
$inputMemory->write($inputData);
$inputMemory->unmap();

// --- Shader ---
$shaderPath = __DIR__ . '/shaders/multiply.comp.spv';
if (!file_exists($shaderPath)) {
    echo "Shader not found! Compile it first:\n";
    echo "  glslangValidator -V examples/shaders/multiply.comp -o examples/shaders/multiply.comp.spv\n";
    exit(1);
}

$shader = Vk\ShaderModule::createFromFile($device, $shaderPath);

// --- Descriptor Set ---
$descriptorSetLayout = new Vk\DescriptorSetLayout($device, [
    ['binding' => 0, 'type' => Vk::DESCRIPTOR_TYPE_STORAGE_BUFFER, 'count' => 1, 'stageFlags' => Vk::SHADER_STAGE_COMPUTE],
    ['binding' => 1, 'type' => Vk::DESCRIPTOR_TYPE_STORAGE_BUFFER, 'count' => 1, 'stageFlags' => Vk::SHADER_STAGE_COMPUTE],
]);

$descriptorPool = new Vk\DescriptorPool($device, 1, [
    ['type' => Vk::DESCRIPTOR_TYPE_STORAGE_BUFFER, 'count' => 2],
]);

$descriptorSets = $descriptorPool->allocateSets([$descriptorSetLayout]);
$descriptorSet = $descriptorSets[0];

$descriptorSet->writeBuffer(0, $inputBuffer);
$descriptorSet->writeBuffer(1, $outputBuffer);

// --- Pipeline ---
$pipelineLayout = new Vk\PipelineLayout($device, [$descriptorSetLayout]);
$pipeline = Vk\Pipeline::createCompute($device, $pipelineLayout, $shader);

// --- Command Buffer ---
$commandPool = new Vk\CommandPool($device, $computeFamilyIndex,
    Vk::COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER);
$commandBuffers = $commandPool->allocateBuffers(1);
$cmd = $commandBuffers[0];

$cmd->begin(Vk::COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT);
$cmd->bindPipeline(Vk::PIPELINE_BIND_POINT_COMPUTE, $pipeline);
$cmd->bindDescriptorSets(Vk::PIPELINE_BIND_POINT_COMPUTE, $pipelineLayout, 0, [$descriptorSet]);
$cmd->dispatch(intdiv($elementCount, 64) ?: 1); // workgroup size 64
$cmd->pipelineBarrier(
    Vk::PIPELINE_STAGE_COMPUTE_SHADER,
    Vk::PIPELINE_STAGE_TRANSFER,
    Vk::ACCESS_SHADER_WRITE,
    Vk::ACCESS_HOST_READ
);
$cmd->end();

// --- Execute ---
$fence = new Vk\Fence($device);
$queue->submit([$cmd], $fence);
$fence->wait();

// --- Read results ---
$outputMemory->map();
$resultData = $outputMemory->read($bufferSize);
$outputMemory->unmap();

echo "\nResults (first 16 elements):\n";
$results = unpack('f*', $resultData);
for ($i = 1; $i <= min(16, $elementCount); $i++) {
    $input = $i;
    $output = $results[$i];
    echo "  [$i] $input * 2 = $output\n";
}

echo "\nCompute shader executed successfully!\n";

$device->waitIdle();
