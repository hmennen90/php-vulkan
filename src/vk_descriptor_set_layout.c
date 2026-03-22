/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkDescriptorSetLayout                                   |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_descriptor_set_layout_ce;
static zend_object_handlers vk_descriptor_set_layout_handlers;

static zend_object *vk_descriptor_set_layout_create_object(zend_class_entry *ce) {
    vk_descriptor_set_layout_object *intern = zend_object_alloc(sizeof(vk_descriptor_set_layout_object), ce);
    intern->layout = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_descriptor_set_layout_handlers;
    return &intern->std;
}

static void vk_descriptor_set_layout_free_object(zend_object *object) {
    vk_descriptor_set_layout_object *intern = VK_OBJ(vk_descriptor_set_layout_object, object);
    if (intern->layout != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(dev->device, intern->layout, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\DescriptorSetLayout::__construct(Vk\Device $device, array $bindings)
 * Each binding: ['binding' => int, 'type' => int, 'count' => int, 'stageFlags' => int] */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_dsl___construct, 0, 0, 2)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, bindings, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkDescriptorSetLayout, __construct) {
    zval *device_zval;
    HashTable *bindings;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_ARRAY_HT(bindings)
    ZEND_PARSE_PARAMETERS_END();

    vk_descriptor_set_layout_object *intern = VK_OBJ(vk_descriptor_set_layout_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    uint32_t count = zend_hash_num_elements(bindings);
    VkDescriptorSetLayoutBinding *layout_bindings = ecalloc(count, sizeof(VkDescriptorSetLayoutBinding));

    zval *zv;
    uint32_t idx = 0;
    ZEND_HASH_FOREACH_VAL(bindings, zv) {
        if (Z_TYPE_P(zv) != IS_ARRAY) continue;
        HashTable *b = Z_ARRVAL_P(zv);

        zval *zbinding = zend_hash_str_find(b, "binding", sizeof("binding") - 1);
        zval *ztype = zend_hash_str_find(b, "type", sizeof("type") - 1);
        zval *zcount = zend_hash_str_find(b, "count", sizeof("count") - 1);
        zval *zstage = zend_hash_str_find(b, "stageFlags", sizeof("stageFlags") - 1);

        layout_bindings[idx] = (VkDescriptorSetLayoutBinding){
            .binding = zbinding ? (uint32_t)zval_get_long(zbinding) : idx,
            .descriptorType = ztype ? (VkDescriptorType)zval_get_long(ztype) : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = zcount ? (uint32_t)zval_get_long(zcount) : 1,
            .stageFlags = zstage ? (VkShaderStageFlags)zval_get_long(zstage) : VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = NULL,
        };
        idx++;
    } ZEND_HASH_FOREACH_END();

    VkDescriptorSetLayoutCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = idx,
        .pBindings = layout_bindings,
    };

    VkResult result = vkCreateDescriptorSetLayout(dev->device, &create_info, NULL, &intern->layout);
    efree(layout_bindings);

    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create descriptor set layout");
    }
}

static const zend_function_entry vk_descriptor_set_layout_methods[] = {
    PHP_ME(VkDescriptorSetLayout, __construct, arginfo_vk_dsl___construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_descriptor_set_layout_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "DescriptorSetLayout", vk_descriptor_set_layout_methods);
    vk_descriptor_set_layout_ce = zend_register_internal_class(&ce);
    vk_descriptor_set_layout_ce->create_object = vk_descriptor_set_layout_create_object;
    vk_descriptor_set_layout_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_descriptor_set_layout_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_descriptor_set_layout_handlers.offset = XtOffsetOf(vk_descriptor_set_layout_object, std);
    vk_descriptor_set_layout_handlers.free_obj = vk_descriptor_set_layout_free_object;
    vk_descriptor_set_layout_handlers.clone_obj = NULL;
}
