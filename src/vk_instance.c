/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkInstance                                              |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_instance_ce;
static zend_object_handlers vk_instance_handlers;

/* ------------------------------------------------------------------ */
/*  Object lifecycle                                                   */
/* ------------------------------------------------------------------ */

static zend_object *vk_instance_create_object(zend_class_entry *ce) {
    vk_instance_object *intern = zend_object_alloc(sizeof(vk_instance_object), ce);
    intern->instance = VK_NULL_HANDLE;
    intern->enable_validation = 0;
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_instance_handlers;
    return &intern->std;
}

static void vk_instance_free_object(zend_object *object) {
    vk_instance_object *intern = VK_OBJ(vk_instance_object, object);
    if (intern->instance != VK_NULL_HANDLE) {
        vkDestroyInstance(intern->instance, NULL);
        intern->instance = VK_NULL_HANDLE;
    }
    zend_object_std_dtor(&intern->std);
}

/* ------------------------------------------------------------------ */
/*  Methods                                                            */
/* ------------------------------------------------------------------ */

/* Vk\Instance::__construct(string $appName = "PHPolygon", int $appVersion = 1,
 *                          ?string $engineName = null, int $engineVersion = 0,
 *                          ?int $apiVersion = null, bool $enableValidation = false,
 *                          array $extensions = []) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_instance___construct, 0, 0, 0)
    ZEND_ARG_TYPE_INFO(0, appName, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, appVersion, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, engineName, IS_STRING, 1, "null")
    ZEND_ARG_TYPE_INFO(0, engineVersion, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, apiVersion, IS_LONG, 1, "null")
    ZEND_ARG_TYPE_INFO(0, enableValidation, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, extensions, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkInstance, __construct) {
    zend_string *app_name = NULL;
    zend_long app_version = 1;
    zend_string *engine_name = NULL;
    zend_long engine_version = 0;
    zend_long api_version_arg = 0;
    zend_bool api_version_is_null = 1;
    zend_bool enable_validation = 0;
    HashTable *extensions = NULL;

    ZEND_PARSE_PARAMETERS_START(0, 7)
        Z_PARAM_OPTIONAL
        Z_PARAM_STR(app_name)
        Z_PARAM_LONG(app_version)
        Z_PARAM_STR_OR_NULL(engine_name)
        Z_PARAM_LONG(engine_version)
        Z_PARAM_LONG_OR_NULL(api_version_arg, api_version_is_null)
        Z_PARAM_BOOL(enable_validation)
        Z_PARAM_ARRAY_HT(extensions)
    ZEND_PARSE_PARAMETERS_END();

    vk_instance_object *intern = VK_OBJ(vk_instance_object, Z_OBJ_P(ZEND_THIS));
    intern->enable_validation = enable_validation;

    uint32_t api_version = VK_API_VERSION_1_0;
    if (!api_version_is_null) {
        api_version = (uint32_t)api_version_arg;
    } else {
        vkEnumerateInstanceVersion(&api_version);
    }

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = app_name ? ZSTR_VAL(app_name) : "PHPolygon",
        .applicationVersion = (uint32_t)app_version,
        .pEngineName = engine_name ? ZSTR_VAL(engine_name) : "PHPolygon",
        .engineVersion = (uint32_t)engine_version,
        .apiVersion = api_version,
    };

    /* Build extension list */
    uint32_t ext_count = 0;
    const char **ext_names = NULL;

    /* On macOS we need VK_KHR_portability_enumeration */
    uint32_t base_ext_count = 0;
    const char *base_exts[2];
#ifdef __APPLE__
    base_exts[base_ext_count++] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
    base_exts[base_ext_count++] = "VK_KHR_get_physical_device_properties2";
#endif

    uint32_t user_ext_count = extensions ? zend_hash_num_elements(extensions) : 0;
    ext_count = base_ext_count + user_ext_count;

    if (ext_count > 0) {
        ext_names = ecalloc(ext_count, sizeof(const char *));
        for (uint32_t i = 0; i < base_ext_count; i++) {
            ext_names[i] = base_exts[i];
        }
        if (user_ext_count > 0) {
            zval *zv;
            uint32_t idx = base_ext_count;
            ZEND_HASH_FOREACH_VAL(extensions, zv) {
                if (Z_TYPE_P(zv) == IS_STRING) {
                    ext_names[idx++] = Z_STRVAL_P(zv);
                }
            } ZEND_HASH_FOREACH_END();
        }
    }

    /* Validation layers */
    uint32_t layer_count = 0;
    const char *layers[] = {"VK_LAYER_KHRONOS_validation"};
    if (enable_validation) {
        layer_count = 1;
    }

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = ext_count,
        .ppEnabledExtensionNames = ext_names,
        .enabledLayerCount = layer_count,
        .ppEnabledLayerNames = enable_validation ? layers : NULL,
#ifdef __APPLE__
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#else
        .flags = 0,
#endif
    };

    VkResult result = vkCreateInstance(&create_info, NULL, &intern->instance);

    if (ext_names) {
        efree(ext_names);
    }

    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create Vulkan instance");
        return;
    }
}

/* Vk\Instance::getPhysicalDevices(): array<Vk\PhysicalDevice> */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_instance_getPhysicalDevices, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkInstance, getPhysicalDevices) {
    ZEND_PARSE_PARAMETERS_NONE();

    vk_instance_object *intern = VK_OBJ(vk_instance_object, Z_OBJ_P(ZEND_THIS));

    uint32_t count = 0;
    VkResult result = vkEnumeratePhysicalDevices(intern->instance, &count, NULL);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to enumerate physical devices");
        return;
    }

    VkPhysicalDevice *devices = ecalloc(count, sizeof(VkPhysicalDevice));
    result = vkEnumeratePhysicalDevices(intern->instance, &count, devices);
    if (result != VK_SUCCESS) {
        efree(devices);
        vk_throw_exception(result, "Failed to enumerate physical devices");
        return;
    }

    array_init_size(return_value, count);

    for (uint32_t i = 0; i < count; i++) {
        zval dev_zval;
        object_init_ex(&dev_zval, vk_physical_device_ce);

        vk_physical_device_object *dev = VK_OBJ(vk_physical_device_object, Z_OBJ(dev_zval));
        dev->physical_device = devices[i];
        vkGetPhysicalDeviceProperties(devices[i], &dev->properties);
        vkGetPhysicalDeviceFeatures(devices[i], &dev->features);
        ZVAL_COPY(&dev->instance_zval, ZEND_THIS);

        add_next_index_zval(return_value, &dev_zval);
    }

    efree(devices);
}

/* Vk\Instance::getVersion(): string */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_instance_getVersion, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkInstance, getVersion) {
    ZEND_PARSE_PARAMETERS_NONE();

    uint32_t version = 0;
    vkEnumerateInstanceVersion(&version);

    char buf[64];
    snprintf(buf, sizeof(buf), "%d.%d.%d",
        VK_VERSION_MAJOR(version),
        VK_VERSION_MINOR(version),
        VK_VERSION_PATCH(version));

    RETURN_STRING(buf);
}

/* ------------------------------------------------------------------ */
/*  Method table                                                       */
/* ------------------------------------------------------------------ */

/* Vk\Instance::enumerateExtensions(): array (static) */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_instance_enumerateExtensions, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkInstance, enumerateExtensions) {
    ZEND_PARSE_PARAMETERS_NONE();

    uint32_t count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &count, NULL);
    VkExtensionProperties *exts = ecalloc(count, sizeof(VkExtensionProperties));
    vkEnumerateInstanceExtensionProperties(NULL, &count, exts);

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

/* Vk\Instance::enumerateLayers(): array (static) */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_instance_enumerateLayers, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkInstance, enumerateLayers) {
    ZEND_PARSE_PARAMETERS_NONE();

    uint32_t count = 0;
    vkEnumerateInstanceLayerProperties(&count, NULL);
    VkLayerProperties *layers = ecalloc(count, sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&count, layers);

    array_init_size(return_value, count);
    for (uint32_t i = 0; i < count; i++) {
        zval layer;
        array_init(&layer);
        add_assoc_string(&layer, "name", layers[i].layerName);
        add_assoc_string(&layer, "description", layers[i].description);
        add_assoc_long(&layer, "specVersion", layers[i].specVersion);
        add_assoc_long(&layer, "implementationVersion", layers[i].implementationVersion);
        add_next_index_zval(return_value, &layer);
    }
    efree(layers);
}

static const zend_function_entry vk_instance_methods[] = {
    PHP_ME(VkInstance, __construct,          arginfo_vk_instance___construct,          ZEND_ACC_PUBLIC)
    PHP_ME(VkInstance, getPhysicalDevices,   arginfo_vk_instance_getPhysicalDevices,   ZEND_ACC_PUBLIC)
    PHP_ME(VkInstance, getVersion,           arginfo_vk_instance_getVersion,           ZEND_ACC_PUBLIC)
    PHP_ME(VkInstance, enumerateExtensions,  arginfo_vk_instance_enumerateExtensions,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(VkInstance, enumerateLayers,      arginfo_vk_instance_enumerateLayers,      ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

/* ------------------------------------------------------------------ */
/*  Registration                                                       */
/* ------------------------------------------------------------------ */

void php_vk_instance_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Instance", vk_instance_methods);
    vk_instance_ce = zend_register_internal_class(&ce);
    vk_instance_ce->create_object = vk_instance_create_object;
    vk_instance_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_instance_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_instance_handlers.offset = XtOffsetOf(vk_instance_object, std);
    vk_instance_handlers.free_obj = vk_instance_free_object;
    vk_instance_handlers.clone_obj = NULL; /* no cloning */
}
