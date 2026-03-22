/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkBuffer                                                |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_buffer_ce;
static zend_object_handlers vk_buffer_handlers;

static zend_object *vk_buffer_create_object(zend_class_entry *ce) {
    vk_buffer_object *intern = zend_object_alloc(sizeof(vk_buffer_object), ce);
    intern->buffer = VK_NULL_HANDLE;
    intern->size = 0;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_buffer_handlers;
    return &intern->std;
}

static void vk_buffer_free_object(zend_object *object) {
    vk_buffer_object *intern = VK_OBJ(vk_buffer_object, object);
    if (intern->buffer != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyBuffer(dev->device, intern->buffer, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\Buffer::__construct(Vk\Device $device, int $size, int $usage, int $sharingMode = 0) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_buffer___construct, 0, 0, 3)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, usage, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sharingMode, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkBuffer, __construct) {
    zval *device_zval;
    zend_long size;
    zend_long usage;
    zend_long sharing_mode = VK_SHARING_MODE_EXCLUSIVE;

    ZEND_PARSE_PARAMETERS_START(3, 4)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_LONG(size)
        Z_PARAM_LONG(usage)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(sharing_mode)
    ZEND_PARSE_PARAMETERS_END();

    vk_buffer_object *intern = VK_OBJ(vk_buffer_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);
    intern->size = (VkDeviceSize)size;

    VkBufferCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = (VkDeviceSize)size,
        .usage = (VkBufferUsageFlags)usage,
        .sharingMode = (VkSharingMode)sharing_mode,
    };

    VkResult result = vkCreateBuffer(dev->device, &create_info, NULL, &intern->buffer);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create buffer");
    }
}

/* Vk\Buffer::getMemoryRequirements(): array */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_buffer_getMemoryRequirements, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkBuffer, getMemoryRequirements) {
    ZEND_PARSE_PARAMETERS_NONE();

    vk_buffer_object *intern = VK_OBJ(vk_buffer_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    VkMemoryRequirements reqs;
    vkGetBufferMemoryRequirements(dev->device, intern->buffer, &reqs);

    array_init(return_value);
    add_assoc_long(return_value, "size", (zend_long)reqs.size);
    add_assoc_long(return_value, "alignment", (zend_long)reqs.alignment);
    add_assoc_long(return_value, "memoryTypeBits", (zend_long)reqs.memoryTypeBits);
}

/* Vk\Buffer::getSize(): int */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_buffer_getSize, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkBuffer, getSize) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_buffer_object *intern = VK_OBJ(vk_buffer_object, Z_OBJ_P(ZEND_THIS));
    RETURN_LONG((zend_long)intern->size);
}

/* Vk\Buffer::bindMemory(Vk\DeviceMemory $memory, int $offset = 0): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_buffer_bindMemory, 0, 1, IS_VOID, 0)
    ZEND_ARG_OBJ_INFO(0, memory, Vk\\DeviceMemory, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkBuffer, bindMemory) {
    zval *memory_zval;
    zend_long offset = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_OBJECT_OF_CLASS(memory_zval, vk_device_memory_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(offset)
    ZEND_PARSE_PARAMETERS_END();

    vk_buffer_object *intern = VK_OBJ(vk_buffer_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
    vk_device_memory_object *mem = VK_OBJ(vk_device_memory_object, Z_OBJ_P(memory_zval));

    VkResult result = vkBindBufferMemory(dev->device, intern->buffer, mem->memory, (VkDeviceSize)offset);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to bind buffer memory");
    }
}

static const zend_function_entry vk_buffer_methods[] = {
    PHP_ME(VkBuffer, __construct,          arginfo_vk_buffer___construct,          ZEND_ACC_PUBLIC)
    PHP_ME(VkBuffer, getMemoryRequirements, arginfo_vk_buffer_getMemoryRequirements, ZEND_ACC_PUBLIC)
    PHP_ME(VkBuffer, getSize,              arginfo_vk_buffer_getSize,              ZEND_ACC_PUBLIC)
    PHP_ME(VkBuffer, bindMemory,           arginfo_vk_buffer_bindMemory,           ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_buffer_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Buffer", vk_buffer_methods);
    vk_buffer_ce = zend_register_internal_class(&ce);
    vk_buffer_ce->create_object = vk_buffer_create_object;
    vk_buffer_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_buffer_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_buffer_handlers.offset = XtOffsetOf(vk_buffer_object, std);
    vk_buffer_handlers.free_obj = vk_buffer_free_object;
    vk_buffer_handlers.clone_obj = NULL;
}
