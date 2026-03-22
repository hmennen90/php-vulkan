<?php
/**
 * Example 03: Memory Info & Buffer Operations
 *
 * Demonstrates buffer creation, memory allocation, mapping,
 * and data transfer — the building blocks for any GPU operation.
 */

$instance = new Vk\Instance();
$physicalDevice = $instance->getPhysicalDevices()[0];

echo "Device: " . $physicalDevice->getName() . "\n\n";

// Show memory layout
$memProps = $physicalDevice->getMemoryProperties();

echo "Memory Heaps:\n";
foreach ($memProps['heaps'] as $i => $heap) {
    $sizeMB = round($heap['size'] / 1024 / 1024);
    $local = $heap['deviceLocal'] ? 'Device Local' : 'Host';
    echo "  Heap #$i: $sizeMB MB ($local)\n";
}

echo "\nMemory Types:\n";
foreach ($memProps['types'] as $i => $type) {
    $flags = [];
    if ($type['deviceLocal']) $flags[] = 'DeviceLocal';
    if ($type['hostVisible']) $flags[] = 'HostVisible';
    if ($type['hostCoherent']) $flags[] = 'HostCoherent';
    if ($type['hostCached']) $flags[] = 'HostCached';
    echo "  Type #$i (heap {$type['heapIndex']}): " . implode(' | ', $flags) . "\n";
}

// Create a logical device
$queueFamilies = $physicalDevice->getQueueFamilies();
$device = new Vk\Device($physicalDevice, [
    ['familyIndex' => $queueFamilies[0]['index'], 'count' => 1],
]);

// Create a buffer and show its memory requirements
echo "\n--- Buffer Test ---\n";

$bufferSize = 1024 * 1024; // 1 MB
$buffer = new Vk\Buffer($device, $bufferSize, Vk\Vk::BUFFER_USAGE_STORAGE_BUFFER);
$reqs = $buffer->getMemoryRequirements();

echo "Buffer size:       " . $buffer->getSize() . " bytes\n";
echo "Memory required:   " . $reqs['size'] . " bytes\n";
echo "Alignment:         " . $reqs['alignment'] . " bytes\n";
echo "Memory type bits:  0b" . decbin($reqs['memoryTypeBits']) . "\n";

// Find host-visible memory and allocate
$memTypeIndex = null;
foreach ($memProps['types'] as $i => $type) {
    if ($type['hostVisible'] && $type['hostCoherent'] && ($reqs['memoryTypeBits'] & (1 << $i))) {
        $memTypeIndex = $i;
        break;
    }
}

if ($memTypeIndex !== null) {
    $memory = new Vk\DeviceMemory($device, $reqs['size'], $memTypeIndex);
    $buffer->bindMemory($memory);

    // Write and read data
    $memory->map();

    $testData = str_repeat("PHPolygon!", 100);
    $memory->write($testData);

    $readBack = $memory->read(strlen($testData));
    $memory->unmap();

    echo "\nWrite/Read test: " . ($readBack === $testData ? "PASSED" : "FAILED") . "\n";
    echo "Written: " . strlen($testData) . " bytes\n";
} else {
    echo "\nNo host-visible memory type available for test.\n";
}

$device->waitIdle();
echo "\nDone!\n";
