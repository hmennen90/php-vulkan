/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkDeviceMemory                                          |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_device_memory_ce;
static zend_object_handlers vk_device_memory_handlers;

static zend_object *vk_device_memory_create_object(zend_class_entry *ce) {
    vk_device_memory_object *intern = zend_object_alloc(sizeof(vk_device_memory_object), ce);
    intern->memory = VK_NULL_HANDLE;
    intern->size = 0;
    intern->mapped = NULL;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_device_memory_handlers;
    return &intern->std;
}

static void vk_device_memory_free_object(zend_object *object) {
    vk_device_memory_object *intern = VK_OBJ(vk_device_memory_object, object);
    if (intern->memory != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            if (intern->mapped) {
                vkUnmapMemory(dev->device, intern->memory);
                intern->mapped = NULL;
            }
            vkFreeMemory(dev->device, intern->memory, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\DeviceMemory::__construct(Vk\Device $device, int $size, int $memoryTypeIndex) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_device_memory___construct, 0, 0, 3)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, memoryTypeIndex, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkDeviceMemory, __construct) {
    zval *device_zval;
    zend_long size;
    zend_long memory_type_index;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_LONG(size)
        Z_PARAM_LONG(memory_type_index)
    ZEND_PARSE_PARAMETERS_END();

    vk_device_memory_object *intern = VK_OBJ(vk_device_memory_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);
    intern->size = (VkDeviceSize)size;

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = (VkDeviceSize)size,
        .memoryTypeIndex = (uint32_t)memory_type_index,
    };

    VkResult result = vkAllocateMemory(dev->device, &alloc_info, NULL, &intern->memory);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to allocate device memory");
    }
}

/* Vk\DeviceMemory::map(int $offset = 0, ?int $size = null): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_device_memory_map, 0, 0, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, size, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

PHP_METHOD(VkDeviceMemory, map) {
    zend_long offset = 0;
    zend_long size_arg = 0;
    zend_bool size_is_null = 1;

    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(offset)
        Z_PARAM_LONG_OR_NULL(size_arg, size_is_null)
    ZEND_PARSE_PARAMETERS_END();

    vk_device_memory_object *intern = VK_OBJ(vk_device_memory_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    VkDeviceSize map_size = size_is_null ? VK_WHOLE_SIZE : (VkDeviceSize)size_arg;

    VkResult result = vkMapMemory(dev->device, intern->memory, (VkDeviceSize)offset, map_size, 0, &intern->mapped);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to map memory");
    }
}

/* Vk\DeviceMemory::unmap(): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_device_memory_unmap, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkDeviceMemory, unmap) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_device_memory_object *intern = VK_OBJ(vk_device_memory_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    if (intern->mapped) {
        vkUnmapMemory(dev->device, intern->memory);
        intern->mapped = NULL;
    }
}

/* Vk\DeviceMemory::write(string $data, int $offset = 0): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_device_memory_write, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkDeviceMemory, write) {
    zend_string *data;
    zend_long offset = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STR(data)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(offset)
    ZEND_PARSE_PARAMETERS_END();

    vk_device_memory_object *intern = VK_OBJ(vk_device_memory_object, Z_OBJ_P(ZEND_THIS));

    if (!intern->mapped) {
        zend_throw_exception(vk_vulkan_exception_ce, "Memory is not mapped. Call map() first.", 0);
        return;
    }

    memcpy((char *)intern->mapped + offset, ZSTR_VAL(data), ZSTR_LEN(data));
}

/* Vk\DeviceMemory::read(int $size, int $offset = 0): string */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_device_memory_read, 0, 1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkDeviceMemory, read) {
    zend_long size;
    zend_long offset = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_LONG(size)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(offset)
    ZEND_PARSE_PARAMETERS_END();

    vk_device_memory_object *intern = VK_OBJ(vk_device_memory_object, Z_OBJ_P(ZEND_THIS));

    if (!intern->mapped) {
        zend_throw_exception(vk_vulkan_exception_ce, "Memory is not mapped. Call map() first.", 0);
        return;
    }

    RETURN_STRINGL((char *)intern->mapped + offset, size);
}

/* Vk\DeviceMemory::flush(int $offset = 0, ?int $size = null): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_device_memory_flush, 0, 0, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, size, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

PHP_METHOD(VkDeviceMemory, flush) {
    zend_long offset = 0, size_arg = 0;
    zend_bool size_is_null = 1;

    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(offset)
        Z_PARAM_LONG_OR_NULL(size_arg, size_is_null)
    ZEND_PARSE_PARAMETERS_END();

    vk_device_memory_object *intern = VK_OBJ(vk_device_memory_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    VkMappedMemoryRange range = {
        .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .memory = intern->memory,
        .offset = (VkDeviceSize)offset,
        .size = size_is_null ? VK_WHOLE_SIZE : (VkDeviceSize)size_arg,
    };

    VkResult result = vkFlushMappedMemoryRanges(dev->device, 1, &range);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to flush mapped memory");
    }
}

/* Vk\DeviceMemory::invalidate(int $offset = 0, ?int $size = null): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_device_memory_invalidate, 0, 0, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, size, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

PHP_METHOD(VkDeviceMemory, invalidate) {
    zend_long offset = 0, size_arg = 0;
    zend_bool size_is_null = 1;

    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(offset)
        Z_PARAM_LONG_OR_NULL(size_arg, size_is_null)
    ZEND_PARSE_PARAMETERS_END();

    vk_device_memory_object *intern = VK_OBJ(vk_device_memory_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    VkMappedMemoryRange range = {
        .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .memory = intern->memory,
        .offset = (VkDeviceSize)offset,
        .size = size_is_null ? VK_WHOLE_SIZE : (VkDeviceSize)size_arg,
    };

    VkResult result = vkInvalidateMappedMemoryRanges(dev->device, 1, &range);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to invalidate mapped memory");
    }
}

static const zend_function_entry vk_device_memory_methods[] = {
    PHP_ME(VkDeviceMemory, __construct, arginfo_vk_device_memory___construct, ZEND_ACC_PUBLIC)
    PHP_ME(VkDeviceMemory, map,         arginfo_vk_device_memory_map,         ZEND_ACC_PUBLIC)
    PHP_ME(VkDeviceMemory, unmap,       arginfo_vk_device_memory_unmap,       ZEND_ACC_PUBLIC)
    PHP_ME(VkDeviceMemory, write,       arginfo_vk_device_memory_write,       ZEND_ACC_PUBLIC)
    PHP_ME(VkDeviceMemory, read,        arginfo_vk_device_memory_read,        ZEND_ACC_PUBLIC)
    PHP_ME(VkDeviceMemory, flush,       arginfo_vk_device_memory_flush,       ZEND_ACC_PUBLIC)
    PHP_ME(VkDeviceMemory, invalidate,  arginfo_vk_device_memory_invalidate,  ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_device_memory_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "DeviceMemory", vk_device_memory_methods);
    vk_device_memory_ce = zend_register_internal_class(&ce);
    vk_device_memory_ce->create_object = vk_device_memory_create_object;
    vk_device_memory_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_device_memory_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_device_memory_handlers.offset = XtOffsetOf(vk_device_memory_object, std);
    vk_device_memory_handlers.free_obj = vk_device_memory_free_object;
    vk_device_memory_handlers.clone_obj = NULL;
}
