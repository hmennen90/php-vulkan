/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkPhysicalDevice                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_physical_device_ce;
static zend_object_handlers vk_physical_device_handlers;

/* ------------------------------------------------------------------ */
/*  Object lifecycle                                                   */
/* ------------------------------------------------------------------ */

static zend_object *vk_physical_device_create_object(zend_class_entry *ce) {
    vk_physical_device_object *intern = zend_object_alloc(sizeof(vk_physical_device_object), ce);
    intern->physical_device = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->instance_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_physical_device_handlers;
    return &intern->std;
}

static void vk_physical_device_free_object(zend_object *object) {
    vk_physical_device_object *intern = VK_OBJ(vk_physical_device_object, object);
    zval_ptr_dtor(&intern->instance_zval);
    zend_object_std_dtor(&intern->std);
}

/* ------------------------------------------------------------------ */
/*  Methods                                                            */
/* ------------------------------------------------------------------ */

/* Vk\PhysicalDevice::getName(): string */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pd_getName, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPhysicalDevice, getName) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_physical_device_object *intern = VK_OBJ(vk_physical_device_object, Z_OBJ_P(ZEND_THIS));
    RETURN_STRING(intern->properties.deviceName);
}

/* Vk\PhysicalDevice::getType(): int */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pd_getType, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPhysicalDevice, getType) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_physical_device_object *intern = VK_OBJ(vk_physical_device_object, Z_OBJ_P(ZEND_THIS));
    RETURN_LONG((zend_long)intern->properties.deviceType);
}

/* Vk\PhysicalDevice::getTypeName(): string */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pd_getTypeName, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPhysicalDevice, getTypeName) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_physical_device_object *intern = VK_OBJ(vk_physical_device_object, Z_OBJ_P(ZEND_THIS));
    switch (intern->properties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: RETURN_STRING("Integrated GPU");
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: RETURN_STRING("Discrete GPU");
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: RETURN_STRING("Virtual GPU");
        case VK_PHYSICAL_DEVICE_TYPE_CPU: RETURN_STRING("CPU");
        default: RETURN_STRING("Other");
    }
}

/* Vk\PhysicalDevice::getApiVersion(): string */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pd_getApiVersion, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPhysicalDevice, getApiVersion) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_physical_device_object *intern = VK_OBJ(vk_physical_device_object, Z_OBJ_P(ZEND_THIS));
    char buf[64];
    snprintf(buf, sizeof(buf), "%d.%d.%d",
        VK_VERSION_MAJOR(intern->properties.apiVersion),
        VK_VERSION_MINOR(intern->properties.apiVersion),
        VK_VERSION_PATCH(intern->properties.apiVersion));
    RETURN_STRING(buf);
}

/* Vk\PhysicalDevice::getDriverVersion(): string */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pd_getDriverVersion, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPhysicalDevice, getDriverVersion) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_physical_device_object *intern = VK_OBJ(vk_physical_device_object, Z_OBJ_P(ZEND_THIS));
    char buf[64];
    snprintf(buf, sizeof(buf), "%d.%d.%d",
        VK_VERSION_MAJOR(intern->properties.driverVersion),
        VK_VERSION_MINOR(intern->properties.driverVersion),
        VK_VERSION_PATCH(intern->properties.driverVersion));
    RETURN_STRING(buf);
}

/* Vk\PhysicalDevice::getQueueFamilies(): array */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pd_getQueueFamilies, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPhysicalDevice, getQueueFamilies) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_physical_device_object *intern = VK_OBJ(vk_physical_device_object, Z_OBJ_P(ZEND_THIS));

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(intern->physical_device, &count, NULL);

    VkQueueFamilyProperties *families = ecalloc(count, sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(intern->physical_device, &count, families);

    array_init_size(return_value, count);

    for (uint32_t i = 0; i < count; i++) {
        zval family;
        array_init(&family);

        add_assoc_long(&family, "index", i);
        add_assoc_long(&family, "queueCount", families[i].queueCount);
        add_assoc_bool(&family, "graphics", (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0);
        add_assoc_bool(&family, "compute", (families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0);
        add_assoc_bool(&family, "transfer", (families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0);
        add_assoc_bool(&family, "sparseBinding", (families[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0);
        add_assoc_long(&family, "timestampValidBits", families[i].timestampValidBits);

        add_next_index_zval(return_value, &family);
    }

    efree(families);
}

/* Vk\PhysicalDevice::getMemoryProperties(): array */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pd_getMemoryProperties, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPhysicalDevice, getMemoryProperties) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_physical_device_object *intern = VK_OBJ(vk_physical_device_object, Z_OBJ_P(ZEND_THIS));

    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(intern->physical_device, &mem_props);

    array_init(return_value);

    /* Memory types */
    zval types;
    array_init_size(&types, mem_props.memoryTypeCount);
    for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
        zval type;
        array_init(&type);
        add_assoc_long(&type, "heapIndex", mem_props.memoryTypes[i].heapIndex);
        add_assoc_bool(&type, "deviceLocal",
            (mem_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0);
        add_assoc_bool(&type, "hostVisible",
            (mem_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0);
        add_assoc_bool(&type, "hostCoherent",
            (mem_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0);
        add_assoc_bool(&type, "hostCached",
            (mem_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) != 0);
        add_next_index_zval(&types, &type);
    }
    add_assoc_zval(return_value, "types", &types);

    /* Memory heaps */
    zval heaps;
    array_init_size(&heaps, mem_props.memoryHeapCount);
    for (uint32_t i = 0; i < mem_props.memoryHeapCount; i++) {
        zval heap;
        array_init(&heap);
        add_assoc_long(&heap, "size", (zend_long)mem_props.memoryHeaps[i].size);
        add_assoc_bool(&heap, "deviceLocal",
            (mem_props.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0);
        add_next_index_zval(&heaps, &heap);
    }
    add_assoc_zval(return_value, "heaps", &heaps);
}

/* Vk\PhysicalDevice::getProperties(): array */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pd_getProperties, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPhysicalDevice, getProperties) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_physical_device_object *intern = VK_OBJ(vk_physical_device_object, Z_OBJ_P(ZEND_THIS));

    array_init(return_value);
    add_assoc_string(return_value, "deviceName", intern->properties.deviceName);
    add_assoc_long(return_value, "deviceType", intern->properties.deviceType);
    add_assoc_long(return_value, "vendorID", intern->properties.vendorID);
    add_assoc_long(return_value, "deviceID", intern->properties.deviceID);

    char api_ver[64];
    snprintf(api_ver, sizeof(api_ver), "%d.%d.%d",
        VK_VERSION_MAJOR(intern->properties.apiVersion),
        VK_VERSION_MINOR(intern->properties.apiVersion),
        VK_VERSION_PATCH(intern->properties.apiVersion));
    add_assoc_string(return_value, "apiVersion", api_ver);

    /* Limits subset — most useful for engine work */
    zval limits;
    array_init(&limits);
    add_assoc_long(&limits, "maxImageDimension2D", intern->properties.limits.maxImageDimension2D);
    add_assoc_long(&limits, "maxImageDimension3D", intern->properties.limits.maxImageDimension3D);
    add_assoc_long(&limits, "maxFramebufferWidth", intern->properties.limits.maxFramebufferWidth);
    add_assoc_long(&limits, "maxFramebufferHeight", intern->properties.limits.maxFramebufferHeight);
    add_assoc_long(&limits, "maxViewports", intern->properties.limits.maxViewports);
    add_assoc_long(&limits, "maxPushConstantsSize", intern->properties.limits.maxPushConstantsSize);
    add_assoc_long(&limits, "maxBoundDescriptorSets", intern->properties.limits.maxBoundDescriptorSets);
    add_assoc_long(&limits, "maxComputeWorkGroupInvocations", intern->properties.limits.maxComputeWorkGroupInvocations);
    add_assoc_zval(return_value, "limits", &limits);
}

/* ------------------------------------------------------------------ */
/*  Method table                                                       */
/* ------------------------------------------------------------------ */

/* Vk\PhysicalDevice::getFormatProperties(int $format): array */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pd_getFormatProperties, 0, 1, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, format, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPhysicalDevice, getFormatProperties) {
    zend_long format;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(format)
    ZEND_PARSE_PARAMETERS_END();

    vk_physical_device_object *intern = VK_OBJ(vk_physical_device_object, Z_OBJ_P(ZEND_THIS));
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(intern->physical_device, (VkFormat)format, &props);

    array_init(return_value);
    add_assoc_long(return_value, "linearTilingFeatures", (zend_long)props.linearTilingFeatures);
    add_assoc_long(return_value, "optimalTilingFeatures", (zend_long)props.optimalTilingFeatures);
    add_assoc_long(return_value, "bufferFeatures", (zend_long)props.bufferFeatures);
}

/* Vk\PhysicalDevice::enumerateExtensions(): array */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pd_enumerateExtensions, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPhysicalDevice, enumerateExtensions) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_physical_device_object *intern = VK_OBJ(vk_physical_device_object, Z_OBJ_P(ZEND_THIS));

    uint32_t count = 0;
    vkEnumerateDeviceExtensionProperties(intern->physical_device, NULL, &count, NULL);
    VkExtensionProperties *exts = ecalloc(count, sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(intern->physical_device, NULL, &count, exts);

    array_init_size(return_value, count);
    for (uint32_t i = 0; i < count; i++) {
        zval ext;
        array_init(&ext);
        add_assoc_string(&ext, "name", exts[i].extensionName);
        add_assoc_long(&ext, "specVersion", exts[i].specVersion);
        add_next_index_zval(return_value, &ext);
    }
    efree(exts);
}

/* Vk\PhysicalDevice::getSurfaceSupport(int $queueFamilyIndex, Vk\Surface $surface): bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pd_getSurfaceSupport, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, queueFamilyIndex, IS_LONG, 0)
    ZEND_ARG_OBJ_INFO(0, surface, Vk\\Surface, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPhysicalDevice, getSurfaceSupport) {
    zend_long queue_family_index;
    zval *surface_zval;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(queue_family_index)
        Z_PARAM_OBJECT_OF_CLASS(surface_zval, vk_surface_ce)
    ZEND_PARSE_PARAMETERS_END();

    vk_physical_device_object *intern = VK_OBJ(vk_physical_device_object, Z_OBJ_P(ZEND_THIS));
    vk_surface_object *surface = VK_OBJ(vk_surface_object, Z_OBJ_P(surface_zval));

    VkBool32 supported = VK_FALSE;
    VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(
        intern->physical_device, (uint32_t)queue_family_index, surface->surface, &supported);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to query surface support");
        return;
    }
    RETURN_BOOL(supported == VK_TRUE);
}

static const zend_function_entry vk_physical_device_methods[] = {
    PHP_ME(VkPhysicalDevice, getName,              arginfo_vk_pd_getName,              ZEND_ACC_PUBLIC)
    PHP_ME(VkPhysicalDevice, getType,              arginfo_vk_pd_getType,              ZEND_ACC_PUBLIC)
    PHP_ME(VkPhysicalDevice, getTypeName,          arginfo_vk_pd_getTypeName,          ZEND_ACC_PUBLIC)
    PHP_ME(VkPhysicalDevice, getApiVersion,        arginfo_vk_pd_getApiVersion,        ZEND_ACC_PUBLIC)
    PHP_ME(VkPhysicalDevice, getDriverVersion,     arginfo_vk_pd_getDriverVersion,     ZEND_ACC_PUBLIC)
    PHP_ME(VkPhysicalDevice, getQueueFamilies,     arginfo_vk_pd_getQueueFamilies,     ZEND_ACC_PUBLIC)
    PHP_ME(VkPhysicalDevice, getMemoryProperties,  arginfo_vk_pd_getMemoryProperties,  ZEND_ACC_PUBLIC)
    PHP_ME(VkPhysicalDevice, getProperties,        arginfo_vk_pd_getProperties,        ZEND_ACC_PUBLIC)
    PHP_ME(VkPhysicalDevice, getFormatProperties,  arginfo_vk_pd_getFormatProperties,  ZEND_ACC_PUBLIC)
    PHP_ME(VkPhysicalDevice, enumerateExtensions,  arginfo_vk_pd_enumerateExtensions,  ZEND_ACC_PUBLIC)
    PHP_ME(VkPhysicalDevice, getSurfaceSupport,    arginfo_vk_pd_getSurfaceSupport,    ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* ------------------------------------------------------------------ */
/*  Registration                                                       */
/* ------------------------------------------------------------------ */

void php_vk_physical_device_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "PhysicalDevice", vk_physical_device_methods);
    vk_physical_device_ce = zend_register_internal_class(&ce);
    vk_physical_device_ce->create_object = vk_physical_device_create_object;
    vk_physical_device_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_physical_device_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_physical_device_handlers.offset = XtOffsetOf(vk_physical_device_object, std);
    vk_physical_device_handlers.free_obj = vk_physical_device_free_object;
    vk_physical_device_handlers.clone_obj = NULL;
}
