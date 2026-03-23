--TEST--
Check all Vulkan classes exist
--FILE--
<?php
$classes = [
    'Vk\Instance', 'Vk\PhysicalDevice', 'Vk\Device', 'Vk\Queue',
    'Vk\Buffer', 'Vk\DeviceMemory', 'Vk\CommandPool', 'Vk\CommandBuffer',
    'Vk\ShaderModule', 'Vk\DescriptorSetLayout', 'Vk\DescriptorPool', 'Vk\DescriptorSet',
    'Vk\PipelineLayout', 'Vk\Pipeline', 'Vk\PipelineCache',
    'Vk\RenderPass', 'Vk\Framebuffer',
    'Vk\Image', 'Vk\ImageView', 'Vk\Sampler',
    'Vk\Fence', 'Vk\Semaphore', 'Vk\Event',
    'Vk\Swapchain', 'Vk\Surface',
    'Vk\QueryPool',
    'Vk\Vk', 'Vk\VulkanException',
];

foreach ($classes as $class) {
    echo $class . ': ' . (class_exists($class) ? 'OK' : 'MISSING') . "\n";
}
?>
--EXPECT--
Vk\Instance: OK
Vk\PhysicalDevice: OK
Vk\Device: OK
Vk\Queue: OK
Vk\Buffer: OK
Vk\DeviceMemory: OK
Vk\CommandPool: OK
Vk\CommandBuffer: OK
Vk\ShaderModule: OK
Vk\DescriptorSetLayout: OK
Vk\DescriptorPool: OK
Vk\DescriptorSet: OK
Vk\PipelineLayout: OK
Vk\Pipeline: OK
Vk\PipelineCache: OK
Vk\RenderPass: OK
Vk\Framebuffer: OK
Vk\Image: OK
Vk\ImageView: OK
Vk\Sampler: OK
Vk\Fence: OK
Vk\Semaphore: OK
Vk\Event: OK
Vk\Swapchain: OK
Vk\Surface: OK
Vk\QueryPool: OK
Vk\Vk: OK
Vk\VulkanException: OK
