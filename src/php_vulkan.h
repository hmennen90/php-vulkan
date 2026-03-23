/*
  +----------------------------------------------------------------------+
  | PHP Vulkan                                                           |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
  | MIT License                                                          |
  +----------------------------------------------------------------------+
  | Author: Hendrik Mennen                                               |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_VULKAN_H
#define PHP_VULKAN_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

#include <vulkan/vulkan.h>

#define PHP_VULKAN_VERSION "0.1.0"
#define PHP_VULKAN_EXTNAME "vulkan"

extern zend_module_entry vulkan_module_entry;
#define phpext_vulkan_ptr &vulkan_module_entry

/* ------------------------------------------------------------------ */
/*  Class entries (declared in their respective .c files)              */
/* ------------------------------------------------------------------ */

extern zend_class_entry *vk_instance_ce;
extern zend_class_entry *vk_physical_device_ce;
extern zend_class_entry *vk_device_ce;
extern zend_class_entry *vk_queue_ce;
extern zend_class_entry *vk_buffer_ce;
extern zend_class_entry *vk_device_memory_ce;
extern zend_class_entry *vk_command_pool_ce;
extern zend_class_entry *vk_command_buffer_ce;
extern zend_class_entry *vk_shader_module_ce;
extern zend_class_entry *vk_descriptor_set_layout_ce;
extern zend_class_entry *vk_descriptor_pool_ce;
extern zend_class_entry *vk_descriptor_set_ce;
extern zend_class_entry *vk_pipeline_layout_ce;
extern zend_class_entry *vk_pipeline_ce;
extern zend_class_entry *vk_render_pass_ce;
extern zend_class_entry *vk_framebuffer_ce;
extern zend_class_entry *vk_image_ce;
extern zend_class_entry *vk_image_view_ce;
extern zend_class_entry *vk_sampler_ce;
extern zend_class_entry *vk_fence_ce;
extern zend_class_entry *vk_semaphore_ce;
extern zend_class_entry *vk_swapchain_ce;
extern zend_class_entry *vk_surface_ce;
extern zend_class_entry *vk_pipeline_cache_ce;
extern zend_class_entry *vk_query_pool_ce;
extern zend_class_entry *vk_event_ce;

/* Exception */
extern zend_class_entry *vk_vulkan_exception_ce;

/* ------------------------------------------------------------------ */
/*  Object structs                                                     */
/* ------------------------------------------------------------------ */

typedef struct _vk_instance_object {
    VkInstance instance;
    zend_bool  enable_validation;
    zend_object std;
} vk_instance_object;

typedef struct _vk_physical_device_object {
    VkPhysicalDevice           physical_device;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures   features;
    zval                       instance_zval;
    zend_object                std;
} vk_physical_device_object;

typedef struct _vk_device_object {
    VkDevice    device;
    zval        physical_device_zval;
    zend_object std;
} vk_device_object;

typedef struct _vk_queue_object {
    VkQueue     queue;
    uint32_t    family_index;
    uint32_t    queue_index;
    zval        device_zval;
    zend_object std;
} vk_queue_object;

typedef struct _vk_buffer_object {
    VkBuffer    buffer;
    VkDeviceSize size;
    zval        device_zval;
    zend_object std;
} vk_buffer_object;

typedef struct _vk_device_memory_object {
    VkDeviceMemory memory;
    VkDeviceSize   size;
    void          *mapped;
    zval           device_zval;
    zend_object    std;
} vk_device_memory_object;

typedef struct _vk_command_pool_object {
    VkCommandPool command_pool;
    zval          device_zval;
    zend_object   std;
} vk_command_pool_object;

typedef struct _vk_command_buffer_object {
    VkCommandBuffer command_buffer;
    zval            pool_zval;
    zval            device_zval;
    zend_object     std;
} vk_command_buffer_object;

typedef struct _vk_shader_module_object {
    VkShaderModule shader_module;
    zval           device_zval;
    zend_object    std;
} vk_shader_module_object;

typedef struct _vk_descriptor_set_layout_object {
    VkDescriptorSetLayout layout;
    zval                  device_zval;
    zend_object           std;
} vk_descriptor_set_layout_object;

typedef struct _vk_descriptor_pool_object {
    VkDescriptorPool pool;
    zval             device_zval;
    zend_object      std;
} vk_descriptor_pool_object;

typedef struct _vk_descriptor_set_object {
    VkDescriptorSet set;
    zval            pool_zval;
    zval            device_zval;
    zend_object     std;
} vk_descriptor_set_object;

typedef struct _vk_pipeline_layout_object {
    VkPipelineLayout layout;
    zval             device_zval;
    zend_object      std;
} vk_pipeline_layout_object;

typedef struct _vk_pipeline_object {
    VkPipeline  pipeline;
    zval        device_zval;
    zend_object std;
} vk_pipeline_object;

typedef struct _vk_render_pass_object {
    VkRenderPass render_pass;
    zval         device_zval;
    zend_object  std;
} vk_render_pass_object;

typedef struct _vk_framebuffer_object {
    VkFramebuffer framebuffer;
    zval          device_zval;
    zend_object   std;
} vk_framebuffer_object;

typedef struct _vk_image_object {
    VkImage     image;
    zend_bool   owns_image; /* false for swapchain images */
    zval        device_zval;
    zend_object std;
} vk_image_object;

typedef struct _vk_image_view_object {
    VkImageView image_view;
    zval        device_zval;
    zend_object std;
} vk_image_view_object;

typedef struct _vk_sampler_object {
    VkSampler   sampler;
    zval        device_zval;
    zend_object std;
} vk_sampler_object;

typedef struct _vk_fence_object {
    VkFence     fence;
    zval        device_zval;
    zend_object std;
} vk_fence_object;

typedef struct _vk_semaphore_object {
    VkSemaphore semaphore;
    zval        device_zval;
    zend_object std;
} vk_semaphore_object;

typedef struct _vk_swapchain_object {
    VkSwapchainKHR swapchain;
    zval           device_zval;
    zval           surface_zval;
    zend_object    std;
} vk_swapchain_object;

typedef struct _vk_surface_object {
    VkSurfaceKHR surface;
    zval         instance_zval;
    zend_object  std;
} vk_surface_object;

typedef struct _vk_pipeline_cache_object {
    VkPipelineCache pipeline_cache;
    zval            device_zval;
    zend_object     std;
} vk_pipeline_cache_object;

typedef struct _vk_query_pool_object {
    VkQueryPool query_pool;
    uint32_t    query_count;
    zval        device_zval;
    zend_object std;
} vk_query_pool_object;

typedef struct _vk_event_object {
    VkEvent     event;
    zval        device_zval;
    zend_object std;
} vk_event_object;

/* ------------------------------------------------------------------ */
/*  Helpers                                                            */
/* ------------------------------------------------------------------ */

/* Offset-based object retrieval (std is always last member) */
static inline void *vk_object_from_zend(zend_object *obj, size_t offset) {
    return (void *)((char *)obj - offset);
}

#define VK_OBJ(type, zobj) \
    ((type *)vk_object_from_zend(zobj, XtOffsetOf(type, std)))

#define VK_OBJ_FROM_ZVAL(type, zv) \
    VK_OBJ(type, Z_OBJ_P(zv))

/* Throw Vulkan exception with VkResult */
void vk_throw_exception(VkResult result, const char *message);
const char *vk_result_to_string(VkResult result);

/* ------------------------------------------------------------------ */
/*  Registration functions (called from MINIT)                         */
/* ------------------------------------------------------------------ */

void php_vk_instance_register(void);
void php_vk_physical_device_register(void);
void php_vk_device_register(void);
void php_vk_queue_register(void);
void php_vk_buffer_register(void);
void php_vk_device_memory_register(void);
void php_vk_command_pool_register(void);
void php_vk_command_buffer_register(void);
void php_vk_shader_module_register(void);
void php_vk_descriptor_set_layout_register(void);
void php_vk_descriptor_pool_register(void);
void php_vk_descriptor_set_register(void);
void php_vk_pipeline_layout_register(void);
void php_vk_pipeline_register(void);
void php_vk_render_pass_register(void);
void php_vk_framebuffer_register(void);
void php_vk_image_register(void);
void php_vk_image_view_register(void);
void php_vk_sampler_register(void);
void php_vk_fence_register(void);
void php_vk_semaphore_register(void);
void php_vk_swapchain_register(void);
void php_vk_surface_register(void);
void php_vk_enums_register(void);
void php_vk_pipeline_cache_register(void);
void php_vk_query_pool_register(void);
void php_vk_event_register(void);

#endif /* PHP_VULKAN_H */
