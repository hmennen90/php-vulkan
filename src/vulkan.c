/*
  +----------------------------------------------------------------------+
  | PHP Vulkan                                                           |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
  | MIT License                                                          |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

/* Exception class entry */
zend_class_entry *vk_vulkan_exception_ce;

/* ------------------------------------------------------------------ */
/*  VkResult to string                                                 */
/* ------------------------------------------------------------------ */

const char *vk_result_to_string(VkResult result) {
    switch (result) {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
        default: return "VK_UNKNOWN_ERROR";
    }
}

void vk_throw_exception(VkResult result, const char *message) {
    zend_throw_exception_ex(vk_vulkan_exception_ce, (zend_long)result,
        "%s (%s)", message, vk_result_to_string(result));
}

/* ------------------------------------------------------------------ */
/*  Module info                                                        */
/* ------------------------------------------------------------------ */

PHP_MINFO_FUNCTION(vulkan) {
    php_info_print_table_start();
    php_info_print_table_header(2, "Vulkan support", "enabled");
    php_info_print_table_row(2, "Extension version", PHP_VULKAN_VERSION);

    /* Query Vulkan API version */
    uint32_t api_version = 0;
    if (vkEnumerateInstanceVersion != NULL) {
        vkEnumerateInstanceVersion(&api_version);
    }
    char version_str[64];
    snprintf(version_str, sizeof(version_str), "%d.%d.%d",
        VK_VERSION_MAJOR(api_version),
        VK_VERSION_MINOR(api_version),
        VK_VERSION_PATCH(api_version));
    php_info_print_table_row(2, "Vulkan API version", version_str);
    php_info_print_table_end();
}

/* ------------------------------------------------------------------ */
/*  MINIT                                                              */
/* ------------------------------------------------------------------ */

PHP_MINIT_FUNCTION(vulkan) {
    /* Register exception class: Vk\VulkanException extends \RuntimeException */
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "VulkanException", NULL);
    vk_vulkan_exception_ce = zend_register_internal_class_ex(&ce, spl_ce_RuntimeException);

    /* Register all Vulkan wrapper classes */
    php_vk_instance_register();
    php_vk_physical_device_register();
    php_vk_device_register();
    php_vk_queue_register();
    php_vk_buffer_register();
    php_vk_device_memory_register();
    php_vk_command_pool_register();
    php_vk_command_buffer_register();
    php_vk_shader_module_register();
    php_vk_descriptor_set_layout_register();
    php_vk_descriptor_pool_register();
    php_vk_descriptor_set_register();
    php_vk_pipeline_layout_register();
    php_vk_pipeline_register();
    php_vk_render_pass_register();
    php_vk_framebuffer_register();
    php_vk_image_register();
    php_vk_image_view_register();
    php_vk_sampler_register();
    php_vk_fence_register();
    php_vk_semaphore_register();
    php_vk_swapchain_register();
    php_vk_surface_register();
    php_vk_enums_register();

    return SUCCESS;
}

/* ------------------------------------------------------------------ */
/*  Module entry                                                       */
/* ------------------------------------------------------------------ */

zend_module_entry vulkan_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_VULKAN_EXTNAME,
    NULL,                  /* functions — we use class methods */
    PHP_MINIT(vulkan),
    NULL,                  /* MSHUTDOWN */
    NULL,                  /* RINIT */
    NULL,                  /* RSHUTDOWN */
    PHP_MINFO(vulkan),
    PHP_VULKAN_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_VULKAN
ZEND_GET_MODULE(vulkan)
#endif
