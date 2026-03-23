# CLAUDE.md -- php-vulkan C Extension

## Overview

php-vulkan is a PHP C extension that provides Vulkan API bindings for GPU graphics
and compute. It exposes Vulkan objects as PHP classes under the `Vk\` namespace.

Built for the PHPolygon game engine. Licensed under MIT.

## Build Instructions

```bash
cd ~/PhpstormProjects/php-vulkan
phpize --clean && phpize && ./configure && make -j$(sysctl -n hw.logicalcpu)
```

Install the built extension:
```bash
cp modules/vulkan.so "$HOME/Library/Application Support/Herd/config/php/85/extensions/vulkan.so"
```

### Dependencies

- **Vulkan SDK** -- headers and libvulkan (auto-detected in /opt/homebrew, /usr/local, /usr)
- **GLFW 3** -- required for `Vk\Surface` window integration (`brew install glfw`)
- **php-glfw** (phpgl/php-glfw) -- must be loaded at runtime for `Vk\Surface::__construct()`

## Architecture

### Source Layout

Each Vulkan object type has its own C source file in `src/`:

| File | PHP Class | Vulkan Handle |
|------|-----------|---------------|
| `vulkan.c` | Module entry, MINIT | -- |
| `vk_instance.c` | `Vk\Instance` | VkInstance |
| `vk_physical_device.c` | `Vk\PhysicalDevice` | VkPhysicalDevice |
| `vk_device.c` | `Vk\Device` | VkDevice |
| `vk_queue.c` | `Vk\Queue` | VkQueue |
| `vk_buffer.c` | `Vk\Buffer` | VkBuffer |
| `vk_device_memory.c` | `Vk\DeviceMemory` | VkDeviceMemory |
| `vk_command_pool.c` | `Vk\CommandPool` | VkCommandPool |
| `vk_command_buffer.c` | `Vk\CommandBuffer` | VkCommandBuffer |
| `vk_shader_module.c` | `Vk\ShaderModule` | VkShaderModule |
| `vk_descriptor_set_layout.c` | `Vk\DescriptorSetLayout` | VkDescriptorSetLayout |
| `vk_descriptor_pool.c` | `Vk\DescriptorPool` | VkDescriptorPool |
| `vk_descriptor_set.c` | `Vk\DescriptorSet` | VkDescriptorSet |
| `vk_descriptor_update_template.c` | `Vk\DescriptorUpdateTemplate` | VkDescriptorUpdateTemplate |
| `vk_pipeline_layout.c` | `Vk\PipelineLayout` | VkPipelineLayout |
| `vk_pipeline.c` | `Vk\Pipeline` | VkPipeline |
| `vk_pipeline_cache.c` | `Vk\PipelineCache` | VkPipelineCache |
| `vk_render_pass.c` | `Vk\RenderPass` | VkRenderPass |
| `vk_framebuffer.c` | `Vk\Framebuffer` | VkFramebuffer |
| `vk_image.c` | `Vk\Image` | VkImage |
| `vk_image_view.c` | `Vk\ImageView` | VkImageView |
| `vk_sampler.c` | `Vk\Sampler` | VkSampler |
| `vk_fence.c` | `Vk\Fence` | VkFence |
| `vk_semaphore.c` | `Vk\Semaphore` | VkSemaphore |
| `vk_surface.c` | `Vk\Surface` | VkSurfaceKHR |
| `vk_swapchain.c` | `Vk\Swapchain` | VkSwapchainKHR |
| `vk_query_pool.c` | `Vk\QueryPool` | VkQueryPool |
| `vk_event.c` | `Vk\Event` | VkEvent |
| `vk_enums.c` | Vulkan enum constants | -- |

### Header

`src/php_vulkan.h` is the single shared header. It contains:
- All object struct typedefs (e.g. `vk_instance_object`, `vk_surface_object`)
- All class entry externs (e.g. `vk_instance_ce`, `vk_surface_ce`)
- Helper macros: `VK_OBJ()`, `VK_OBJ_FROM_ZVAL()`
- Registration function declarations (called from MINIT in `vulkan.c`)

### Object Pattern

Every Vulkan wrapper follows the same pattern:
1. A typedef struct with the Vulkan handle, parent zval references, and `zend_object std` as the **last** member
2. `create_object` allocates and initializes
3. `free_object` destroys the Vulkan handle and releases parent references via `zval_ptr_dtor`
4. Parent zvals (e.g. `instance_zval`, `device_zval`) prevent GC of parent objects while children are alive

### Vk\Surface -- GLFW Integration

`Vk\Surface` requires the php-glfw extension at runtime. The constructor:
```php
new Vk\Surface(Vk\Instance $instance, GLFWwindow $window)
```
calls `glfwCreateWindowSurface()` to create the Vulkan surface from a GLFW window.

The php-glfw interop uses two extern symbols resolved at link/load time:
- `phpglfw_glfwwindow_ce` -- the class entry for GLFWwindow
- `phpglfw_glfwwindowptr_from_zval_ptr()` -- extracts `GLFWwindow*` from a PHP zval

These are exported by the php-glfw extension and do not require php-glfw headers at compile time.

## Version String Locations

When bumping the version, update ALL of these:

1. `src/php_vulkan.h` -- `PHP_VULKAN_VERSION` macro
2. `composer.json` -- `"version"` field
3. `tests/001-extension-loaded.phpt` -- expected output string

## Config

`config.m4` handles:
- Vulkan SDK detection (manual search + pkg-config fallback)
- GLFW3 detection (manual search + pkg-config fallback)
- Both libraries are linked into `VULKAN_SHARED_LIBADD`
