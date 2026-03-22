/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkDescriptorSet                                         |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_descriptor_set_ce;
static zend_object_handlers vk_descriptor_set_handlers;

static zend_object *vk_descriptor_set_create_object(zend_class_entry *ce) {
    vk_descriptor_set_object *intern = zend_object_alloc(sizeof(vk_descriptor_set_object), ce);
    intern->set = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->pool_zval);
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_descriptor_set_handlers;
    return &intern->std;
}

static void vk_descriptor_set_free_object(zend_object *object) {
    vk_descriptor_set_object *intern = VK_OBJ(vk_descriptor_set_object, object);
    zval_ptr_dtor(&intern->pool_zval);
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\DescriptorSet::writeBuffer(int $binding, Vk\Buffer $buffer,
 *                                int $offset = 0, ?int $range = null,
 *                                int $type = 7): void
 * type 7 = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_ds_writeBuffer, 0, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, binding, IS_LONG, 0)
    ZEND_ARG_OBJ_INFO(0, buffer, Vk\\Buffer, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, range, IS_LONG, 1, "null")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "7")
ZEND_END_ARG_INFO()

PHP_METHOD(VkDescriptorSet, writeBuffer) {
    zend_long binding;
    zval *buffer_zval;
    zend_long offset = 0;
    zend_long range_arg = 0;
    zend_bool range_is_null = 1;
    zend_long type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    ZEND_PARSE_PARAMETERS_START(2, 5)
        Z_PARAM_LONG(binding)
        Z_PARAM_OBJECT_OF_CLASS(buffer_zval, vk_buffer_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(offset)
        Z_PARAM_LONG_OR_NULL(range_arg, range_is_null)
        Z_PARAM_LONG(type)
    ZEND_PARSE_PARAMETERS_END();

    vk_descriptor_set_object *intern = VK_OBJ(vk_descriptor_set_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
    vk_buffer_object *buf = VK_OBJ(vk_buffer_object, Z_OBJ_P(buffer_zval));

    VkDescriptorBufferInfo buffer_info = {
        .buffer = buf->buffer,
        .offset = (VkDeviceSize)offset,
        .range = range_is_null ? VK_WHOLE_SIZE : (VkDeviceSize)range_arg,
    };

    VkWriteDescriptorSet write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = intern->set,
        .dstBinding = (uint32_t)binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = (VkDescriptorType)type,
        .pBufferInfo = &buffer_info,
    };

    vkUpdateDescriptorSets(dev->device, 1, &write, 0, NULL);
}

/* Vk\DescriptorSet::writeImage(int $binding, Vk\ImageView $imageView, Vk\Sampler $sampler,
 *                               int $imageLayout = 5, int $type = 1): void
 * layout 5 = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
 * type 1 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_ds_writeImage, 0, 3, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, binding, IS_LONG, 0)
    ZEND_ARG_OBJ_INFO(0, imageView, Vk\\ImageView, 0)
    ZEND_ARG_OBJ_INFO(0, sampler, Vk\\Sampler, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, imageLayout, IS_LONG, 0, "5")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

PHP_METHOD(VkDescriptorSet, writeImage) {
    zend_long binding;
    zval *image_view_zval, *sampler_zval;
    zend_long image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    zend_long type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    ZEND_PARSE_PARAMETERS_START(3, 5)
        Z_PARAM_LONG(binding)
        Z_PARAM_OBJECT_OF_CLASS(image_view_zval, vk_image_view_ce)
        Z_PARAM_OBJECT_OF_CLASS(sampler_zval, vk_sampler_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(image_layout)
        Z_PARAM_LONG(type)
    ZEND_PARSE_PARAMETERS_END();

    vk_descriptor_set_object *intern = VK_OBJ(vk_descriptor_set_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
    vk_image_view_object *iv = VK_OBJ(vk_image_view_object, Z_OBJ_P(image_view_zval));
    vk_sampler_object *sampler = VK_OBJ(vk_sampler_object, Z_OBJ_P(sampler_zval));

    VkDescriptorImageInfo image_info = {
        .sampler = sampler->sampler,
        .imageView = iv->image_view,
        .imageLayout = (VkImageLayout)image_layout,
    };

    VkWriteDescriptorSet write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = intern->set,
        .dstBinding = (uint32_t)binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = (VkDescriptorType)type,
        .pImageInfo = &image_info,
    };

    vkUpdateDescriptorSets(dev->device, 1, &write, 0, NULL);
}

static const zend_function_entry vk_descriptor_set_methods[] = {
    PHP_ME(VkDescriptorSet, writeBuffer, arginfo_vk_ds_writeBuffer, ZEND_ACC_PUBLIC)
    PHP_ME(VkDescriptorSet, writeImage,  arginfo_vk_ds_writeImage,  ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_descriptor_set_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "DescriptorSet", vk_descriptor_set_methods);
    vk_descriptor_set_ce = zend_register_internal_class(&ce);
    vk_descriptor_set_ce->create_object = vk_descriptor_set_create_object;
    vk_descriptor_set_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_descriptor_set_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_descriptor_set_handlers.offset = XtOffsetOf(vk_descriptor_set_object, std);
    vk_descriptor_set_handlers.free_obj = vk_descriptor_set_free_object;
    vk_descriptor_set_handlers.clone_obj = NULL;
}
