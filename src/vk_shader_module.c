/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkShaderModule                                          |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_shader_module_ce;
static zend_object_handlers vk_shader_module_handlers;

static zend_object *vk_shader_module_create_object(zend_class_entry *ce) {
    vk_shader_module_object *intern = zend_object_alloc(sizeof(vk_shader_module_object), ce);
    intern->shader_module = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_shader_module_handlers;
    return &intern->std;
}

static void vk_shader_module_free_object(zend_object *object) {
    vk_shader_module_object *intern = VK_OBJ(vk_shader_module_object, object);
    if (intern->shader_module != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyShaderModule(dev->device, intern->shader_module, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\ShaderModule::__construct(Vk\Device $device, string $spirvCode) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_shader_module___construct, 0, 0, 2)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, spirvCode, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkShaderModule, __construct) {
    zval *device_zval;
    zend_string *spirv_code;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_STR(spirv_code)
    ZEND_PARSE_PARAMETERS_END();

    vk_shader_module_object *intern = VK_OBJ(vk_shader_module_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    if (ZSTR_LEN(spirv_code) % 4 != 0) {
        zend_throw_exception(vk_vulkan_exception_ce,
            "SPIR-V code size must be a multiple of 4 bytes", 0);
        return;
    }

    VkShaderModuleCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = ZSTR_LEN(spirv_code),
        .pCode = (const uint32_t *)ZSTR_VAL(spirv_code),
    };

    VkResult result = vkCreateShaderModule(dev->device, &create_info, NULL, &intern->shader_module);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create shader module");
    }
}

/* Vk\ShaderModule::createFromFile(Vk\Device $device, string $path): static */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_vk_shader_module_createFromFile, 0, 2, Vk\\ShaderModule, 0)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkShaderModule, createFromFile) {
    zval *device_zval;
    zend_string *path;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_STR(path)
    ZEND_PARSE_PARAMETERS_END();

    php_stream *stream = php_stream_open_wrapper(ZSTR_VAL(path), "rb", REPORT_ERRORS, NULL);
    if (!stream) {
        zend_throw_exception_ex(vk_vulkan_exception_ce, 0,
            "Cannot open SPIR-V file: %s", ZSTR_VAL(path));
        return;
    }

    zend_string *contents = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0);
    php_stream_close(stream);

    if (!contents) {
        zend_throw_exception_ex(vk_vulkan_exception_ce, 0,
            "Failed to read SPIR-V file: %s", ZSTR_VAL(path));
        return;
    }

    /* Call __construct with the data */
    object_init_ex(return_value, vk_shader_module_ce);
    vk_shader_module_object *intern = VK_OBJ(vk_shader_module_object, Z_OBJ_P(return_value));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    if (ZSTR_LEN(contents) % 4 != 0) {
        zend_string_release(contents);
        zend_throw_exception(vk_vulkan_exception_ce,
            "SPIR-V code size must be a multiple of 4 bytes", 0);
        return;
    }

    VkShaderModuleCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = ZSTR_LEN(contents),
        .pCode = (const uint32_t *)ZSTR_VAL(contents),
    };

    VkResult result = vkCreateShaderModule(dev->device, &create_info, NULL, &intern->shader_module);
    zend_string_release(contents);

    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create shader module from file");
    }
}

static const zend_function_entry vk_shader_module_methods[] = {
    PHP_ME(VkShaderModule, __construct,   arginfo_vk_shader_module___construct,   ZEND_ACC_PUBLIC)
    PHP_ME(VkShaderModule, createFromFile, arginfo_vk_shader_module_createFromFile, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

void php_vk_shader_module_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "ShaderModule", vk_shader_module_methods);
    vk_shader_module_ce = zend_register_internal_class(&ce);
    vk_shader_module_ce->create_object = vk_shader_module_create_object;
    vk_shader_module_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_shader_module_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_shader_module_handlers.offset = XtOffsetOf(vk_shader_module_object, std);
    vk_shader_module_handlers.free_obj = vk_shader_module_free_object;
    vk_shader_module_handlers.clone_obj = NULL;
}
