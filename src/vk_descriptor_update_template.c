/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkDescriptorUpdateTemplate                              |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_descriptor_update_template_ce;
static zend_object_handlers vk_descriptor_update_template_handlers;

static zend_object *vk_descriptor_update_template_create_object(zend_class_entry *ce) {
    vk_descriptor_update_template_object *intern = zend_object_alloc(sizeof(vk_descriptor_update_template_object), ce);
    intern->update_template = VK_NULL_HANDLE;
    intern->entry_count = 0;
    intern->data_size = 0;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_descriptor_update_template_handlers;
    return &intern->std;
}

static void vk_descriptor_update_template_free_object(zend_object *object) {
    vk_descriptor_update_template_object *intern = VK_OBJ(vk_descriptor_update_template_object, object);
    if (intern->update_template != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyDescriptorUpdateTemplate(dev->device, intern->update_template, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\DescriptorUpdateTemplate::__construct(Vk\Device $device,
 *     Vk\DescriptorSetLayout $layout, array $entries)
 *
 * entries: [['binding' => int, 'descriptorType' => int, 'descriptorCount' => int,
 *            'offset' => int, 'stride' => int], ...] */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_dut___construct, 0, 0, 3)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_OBJ_INFO(0, layout, Vk\\DescriptorSetLayout, 0)
    ZEND_ARG_TYPE_INFO(0, entries, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkDescriptorUpdateTemplate, __construct) {
    zval *device_zval, *layout_zval;
    HashTable *entries;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_OBJECT_OF_CLASS(layout_zval, vk_descriptor_set_layout_ce)
        Z_PARAM_ARRAY_HT(entries)
    ZEND_PARSE_PARAMETERS_END();

    vk_descriptor_update_template_object *intern = VK_OBJ(vk_descriptor_update_template_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    vk_descriptor_set_layout_object *dsl = VK_OBJ(vk_descriptor_set_layout_object, Z_OBJ_P(layout_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    uint32_t count = zend_hash_num_elements(entries);
    VkDescriptorUpdateTemplateEntry *tmpl_entries = ecalloc(count, sizeof(VkDescriptorUpdateTemplateEntry));

    zval *zv;
    uint32_t idx = 0;
    size_t max_end = 0;
    ZEND_HASH_FOREACH_VAL(entries, zv) {
        if (Z_TYPE_P(zv) != IS_ARRAY) continue;
        HashTable *e = Z_ARRVAL_P(zv);

        zval *zbinding = zend_hash_str_find(e, "binding", sizeof("binding") - 1);
        zval *ztype = zend_hash_str_find(e, "descriptorType", sizeof("descriptorType") - 1);
        zval *zcount = zend_hash_str_find(e, "descriptorCount", sizeof("descriptorCount") - 1);
        zval *zoffset = zend_hash_str_find(e, "offset", sizeof("offset") - 1);
        zval *zstride = zend_hash_str_find(e, "stride", sizeof("stride") - 1);

        uint32_t desc_count = zcount ? (uint32_t)zval_get_long(zcount) : 1;
        size_t off = zoffset ? (size_t)zval_get_long(zoffset) : 0;
        size_t stride = zstride ? (size_t)zval_get_long(zstride) : sizeof(VkDescriptorBufferInfo);

        tmpl_entries[idx] = (VkDescriptorUpdateTemplateEntry){
            .dstBinding = zbinding ? (uint32_t)zval_get_long(zbinding) : idx,
            .dstArrayElement = 0,
            .descriptorCount = desc_count,
            .descriptorType = ztype ? (VkDescriptorType)zval_get_long(ztype) : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .offset = off,
            .stride = stride,
        };

        size_t entry_end = off + desc_count * stride;
        if (entry_end > max_end) max_end = entry_end;
        idx++;
    } ZEND_HASH_FOREACH_END();

    intern->entry_count = idx;
    intern->data_size = max_end;

    VkDescriptorUpdateTemplateCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO,
        .descriptorUpdateEntryCount = idx,
        .pDescriptorUpdateEntries = tmpl_entries,
        .templateType = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET,
        .descriptorSetLayout = dsl->layout,
    };

    VkResult result = vkCreateDescriptorUpdateTemplate(dev->device, &create_info, NULL, &intern->update_template);
    efree(tmpl_entries);

    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create descriptor update template");
    }
}

/* Vk\DescriptorUpdateTemplate::update(Vk\DescriptorSet $descriptorSet, string $data): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_dut_update, 0, 2, IS_VOID, 0)
    ZEND_ARG_OBJ_INFO(0, descriptorSet, Vk\\DescriptorSet, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkDescriptorUpdateTemplate, update) {
    zval *ds_zval;
    zend_string *data;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(ds_zval, vk_descriptor_set_ce)
        Z_PARAM_STR(data)
    ZEND_PARSE_PARAMETERS_END();

    vk_descriptor_update_template_object *intern = VK_OBJ(vk_descriptor_update_template_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
    vk_descriptor_set_object *ds = VK_OBJ(vk_descriptor_set_object, Z_OBJ_P(ds_zval));

    vkUpdateDescriptorSetWithTemplate(dev->device, ds->set, intern->update_template, ZSTR_VAL(data));
}

static const zend_function_entry vk_descriptor_update_template_methods[] = {
    PHP_ME(VkDescriptorUpdateTemplate, __construct, arginfo_vk_dut___construct, ZEND_ACC_PUBLIC)
    PHP_ME(VkDescriptorUpdateTemplate, update,      arginfo_vk_dut_update,      ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_descriptor_update_template_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "DescriptorUpdateTemplate", vk_descriptor_update_template_methods);
    vk_descriptor_update_template_ce = zend_register_internal_class(&ce);
    vk_descriptor_update_template_ce->create_object = vk_descriptor_update_template_create_object;
    vk_descriptor_update_template_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_descriptor_update_template_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_descriptor_update_template_handlers.offset = XtOffsetOf(vk_descriptor_update_template_object, std);
    vk_descriptor_update_template_handlers.free_obj = vk_descriptor_update_template_free_object;
    vk_descriptor_update_template_handlers.clone_obj = NULL;
}
