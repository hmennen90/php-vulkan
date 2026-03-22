/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkSampler                                               |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_sampler_ce;
static zend_object_handlers vk_sampler_handlers;

static zend_object *vk_sampler_create_object(zend_class_entry *ce) {
    vk_sampler_object *intern = zend_object_alloc(sizeof(vk_sampler_object), ce);
    intern->sampler = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_sampler_handlers;
    return &intern->std;
}

static void vk_sampler_free_object(zend_object *object) {
    vk_sampler_object *intern = VK_OBJ(vk_sampler_object, object);
    if (intern->sampler != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroySampler(dev->device, intern->sampler, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\Sampler::__construct(Vk\Device $device, array $config = [])
 * config: magFilter, minFilter, addressModeU/V/W, anisotropy, maxAnisotropy */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_sampler___construct, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, config, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkSampler, __construct) {
    zval *device_zval;
    HashTable *config = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY_HT(config)
    ZEND_PARSE_PARAMETERS_END();

    vk_sampler_object *intern = VK_OBJ(vk_sampler_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    VkFilter mag = VK_FILTER_LINEAR, min_filter = VK_FILTER_LINEAR;
    VkSamplerAddressMode u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode w = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkBool32 aniso = VK_FALSE;
    float max_aniso = 1.0f;

    if (config) {
        zval *zv;
        if ((zv = zend_hash_str_find(config, "magFilter", sizeof("magFilter") - 1)))
            mag = (VkFilter)zval_get_long(zv);
        if ((zv = zend_hash_str_find(config, "minFilter", sizeof("minFilter") - 1)))
            min_filter = (VkFilter)zval_get_long(zv);
        if ((zv = zend_hash_str_find(config, "addressModeU", sizeof("addressModeU") - 1)))
            u = (VkSamplerAddressMode)zval_get_long(zv);
        if ((zv = zend_hash_str_find(config, "addressModeV", sizeof("addressModeV") - 1)))
            v = (VkSamplerAddressMode)zval_get_long(zv);
        if ((zv = zend_hash_str_find(config, "addressModeW", sizeof("addressModeW") - 1)))
            w = (VkSamplerAddressMode)zval_get_long(zv);
        if ((zv = zend_hash_str_find(config, "anisotropy", sizeof("anisotropy") - 1)))
            aniso = zval_is_true(zv) ? VK_TRUE : VK_FALSE;
        if ((zv = zend_hash_str_find(config, "maxAnisotropy", sizeof("maxAnisotropy") - 1)))
            max_aniso = (float)zval_get_double(zv);
    }

    VkSamplerCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = mag,
        .minFilter = min_filter,
        .addressModeU = u,
        .addressModeV = v,
        .addressModeW = w,
        .anisotropyEnable = aniso,
        .maxAnisotropy = max_aniso,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
    };

    VkResult result = vkCreateSampler(dev->device, &create_info, NULL, &intern->sampler);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create sampler");
    }
}

static const zend_function_entry vk_sampler_methods[] = {
    PHP_ME(VkSampler, __construct, arginfo_vk_sampler___construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_sampler_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Sampler", vk_sampler_methods);
    vk_sampler_ce = zend_register_internal_class(&ce);
    vk_sampler_ce->create_object = vk_sampler_create_object;
    vk_sampler_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_sampler_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_sampler_handlers.offset = XtOffsetOf(vk_sampler_object, std);
    vk_sampler_handlers.free_obj = vk_sampler_free_object;
    vk_sampler_handlers.clone_obj = NULL;
}
