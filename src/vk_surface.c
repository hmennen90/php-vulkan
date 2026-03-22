/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkSurfaceKHR (placeholder for windowing integration)    |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_surface_ce;
static zend_object_handlers vk_surface_handlers;

static zend_object *vk_surface_create_object(zend_class_entry *ce) {
    vk_surface_object *intern = zend_object_alloc(sizeof(vk_surface_object), ce);
    intern->surface = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->instance_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_surface_handlers;
    return &intern->std;
}

static void vk_surface_free_object(zend_object *object) {
    vk_surface_object *intern = VK_OBJ(vk_surface_object, object);
    if (intern->surface != VK_NULL_HANDLE && !Z_ISUNDEF(intern->instance_zval)) {
        vk_instance_object *inst = VK_OBJ_FROM_ZVAL(vk_instance_object, &intern->instance_zval);
        if (inst->instance != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(inst->instance, intern->surface, NULL);
        }
    }
    zval_ptr_dtor(&intern->instance_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\Surface::getCapabilities(Vk\PhysicalDevice $physicalDevice): array */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_surface_getCapabilities, 0, 1, IS_ARRAY, 0)
    ZEND_ARG_OBJ_INFO(0, physicalDevice, Vk\\PhysicalDevice, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkSurface, getCapabilities) {
    zval *pd_zval;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(pd_zval, vk_physical_device_ce)
    ZEND_PARSE_PARAMETERS_END();

    vk_surface_object *intern = VK_OBJ(vk_surface_object, Z_OBJ_P(ZEND_THIS));
    vk_physical_device_object *pd = VK_OBJ(vk_physical_device_object, Z_OBJ_P(pd_zval));

    VkSurfaceCapabilitiesKHR caps;
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pd->physical_device, intern->surface, &caps);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to get surface capabilities");
        return;
    }

    array_init(return_value);
    add_assoc_long(return_value, "minImageCount", caps.minImageCount);
    add_assoc_long(return_value, "maxImageCount", caps.maxImageCount);
    add_assoc_long(return_value, "currentWidth", caps.currentExtent.width);
    add_assoc_long(return_value, "currentHeight", caps.currentExtent.height);
    add_assoc_long(return_value, "maxWidth", caps.maxImageExtent.width);
    add_assoc_long(return_value, "maxHeight", caps.maxImageExtent.height);
    add_assoc_long(return_value, "maxImageArrayLayers", caps.maxImageArrayLayers);
    add_assoc_long(return_value, "supportedTransforms", caps.supportedTransforms);
    add_assoc_long(return_value, "currentTransform", caps.currentTransform);
    add_assoc_long(return_value, "supportedCompositeAlpha", caps.supportedCompositeAlpha);
    add_assoc_long(return_value, "supportedUsageFlags", caps.supportedUsageFlags);
}

/* Vk\Surface::getFormats(Vk\PhysicalDevice $physicalDevice): array */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_surface_getFormats, 0, 1, IS_ARRAY, 0)
    ZEND_ARG_OBJ_INFO(0, physicalDevice, Vk\\PhysicalDevice, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkSurface, getFormats) {
    zval *pd_zval;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(pd_zval, vk_physical_device_ce)
    ZEND_PARSE_PARAMETERS_END();

    vk_surface_object *intern = VK_OBJ(vk_surface_object, Z_OBJ_P(ZEND_THIS));
    vk_physical_device_object *pd = VK_OBJ(vk_physical_device_object, Z_OBJ_P(pd_zval));

    uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(pd->physical_device, intern->surface, &count, NULL);
    VkSurfaceFormatKHR *formats = ecalloc(count, sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(pd->physical_device, intern->surface, &count, formats);

    array_init_size(return_value, count);
    for (uint32_t i = 0; i < count; i++) {
        zval fmt;
        array_init(&fmt);
        add_assoc_long(&fmt, "format", formats[i].format);
        add_assoc_long(&fmt, "colorSpace", formats[i].colorSpace);
        add_next_index_zval(return_value, &fmt);
    }
    efree(formats);
}

/* Vk\Surface::getPresentModes(Vk\PhysicalDevice $physicalDevice): array */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_surface_getPresentModes, 0, 1, IS_ARRAY, 0)
    ZEND_ARG_OBJ_INFO(0, physicalDevice, Vk\\PhysicalDevice, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkSurface, getPresentModes) {
    zval *pd_zval;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(pd_zval, vk_physical_device_ce)
    ZEND_PARSE_PARAMETERS_END();

    vk_surface_object *intern = VK_OBJ(vk_surface_object, Z_OBJ_P(ZEND_THIS));
    vk_physical_device_object *pd = VK_OBJ(vk_physical_device_object, Z_OBJ_P(pd_zval));

    uint32_t count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(pd->physical_device, intern->surface, &count, NULL);
    VkPresentModeKHR *modes = ecalloc(count, sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(pd->physical_device, intern->surface, &count, modes);

    array_init_size(return_value, count);
    for (uint32_t i = 0; i < count; i++) {
        add_next_index_long(return_value, (zend_long)modes[i]);
    }
    efree(modes);
}

static const zend_function_entry vk_surface_methods[] = {
    PHP_ME(VkSurface, getCapabilities,  arginfo_vk_surface_getCapabilities,  ZEND_ACC_PUBLIC)
    PHP_ME(VkSurface, getFormats,       arginfo_vk_surface_getFormats,       ZEND_ACC_PUBLIC)
    PHP_ME(VkSurface, getPresentModes,  arginfo_vk_surface_getPresentModes,  ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_surface_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Surface", vk_surface_methods);
    vk_surface_ce = zend_register_internal_class(&ce);
    vk_surface_ce->create_object = vk_surface_create_object;
    vk_surface_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_surface_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_surface_handlers.offset = XtOffsetOf(vk_surface_object, std);
    vk_surface_handlers.free_obj = vk_surface_free_object;
    vk_surface_handlers.clone_obj = NULL;
}
