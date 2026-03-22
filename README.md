# php-vulkan

PHP extension providing Vulkan API bindings for use with [PHPolygon](https://github.com/hmennen90/php-vulkan) — a 2D/3D engine written in PHP.

## Requirements

- PHP >= 8.1
- Vulkan SDK (LunarG / MoltenVK on macOS)
- C compiler (gcc/clang)

## Installation

```bash
# Install Vulkan SDK (macOS)
brew install vulkan-sdk

# Build the extension
phpize
./configure --with-vulkan
make
make install
```

Add to your `php.ini`:
```ini
extension=vulkan
```

## Quick Start

```php
<?php
// Create Vulkan instance
$instance = new Vk\Instance(appName: 'MyApp');

// List GPUs
foreach ($instance->getPhysicalDevices() as $device) {
    echo $device->getName() . " (" . $device->getTypeName() . ")\n";
}
```

## API Overview

All classes live in the `Vk` namespace:

| Class | Vulkan Object |
|-------|--------------|
| `Vk\Instance` | VkInstance |
| `Vk\PhysicalDevice` | VkPhysicalDevice |
| `Vk\Device` | VkDevice |
| `Vk\Queue` | VkQueue |
| `Vk\Buffer` | VkBuffer |
| `Vk\DeviceMemory` | VkDeviceMemory |
| `Vk\CommandPool` | VkCommandPool |
| `Vk\CommandBuffer` | VkCommandBuffer |
| `Vk\ShaderModule` | VkShaderModule |
| `Vk\DescriptorSetLayout` | VkDescriptorSetLayout |
| `Vk\DescriptorPool` | VkDescriptorPool |
| `Vk\DescriptorSet` | VkDescriptorSet |
| `Vk\PipelineLayout` | VkPipelineLayout |
| `Vk\Pipeline` | VkPipeline |
| `Vk\RenderPass` | VkRenderPass |
| `Vk\Framebuffer` | VkFramebuffer |
| `Vk\Image` | VkImage |
| `Vk\ImageView` | VkImageView |
| `Vk\Sampler` | VkSampler |
| `Vk\Fence` | VkFence |
| `Vk\Semaphore` | VkSemaphore |
| `Vk\Swapchain` | VkSwapchainKHR |
| `Vk\Surface` | VkSurfaceKHR |

Constants are available via `Vk\Vk::CONSTANT_NAME`.

## Examples

See the `examples/` directory:

- `01_enumerate_devices.php` — List all GPUs and their properties
- `02_compute_shader.php` — Run a compute shader (multiply by 2)
- `03_memory_info.php` — Buffer creation and memory operations

## License

MIT
