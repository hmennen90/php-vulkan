/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkCommandPool                                           |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_command_pool_ce;
static zend_object_handlers vk_command_pool_handlers;

static zend_object *vk_command_pool_create_object(zend_class_entry *ce) {
    vk_command_pool_object *intern = zend_object_alloc(sizeof(vk_command_pool_object), ce);
    intern->command_pool = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_command_pool_handlers;
    return &intern->std;
}

static void vk_command_pool_free_object(zend_object *object) {
    vk_command_pool_object *intern = VK_OBJ(vk_command_pool_object, object);
    if (intern->command_pool != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyCommandPool(dev->device, intern->command_pool, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\CommandPool::__construct(Vk\Device $device, int $queueFamilyIndex, int $flags = 0) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_command_pool___construct, 0, 0, 2)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, queueFamilyIndex, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandPool, __construct) {
    zval *device_zval;
    zend_long queue_family_index;
    zend_long flags = 0;

    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_LONG(queue_family_index)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(flags)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_pool_object *intern = VK_OBJ(vk_command_pool_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    VkCommandPoolCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = (uint32_t)queue_family_index,
        .flags = (VkCommandPoolCreateFlags)flags,
    };

    VkResult result = vkCreateCommandPool(dev->device, &create_info, NULL, &intern->command_pool);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create command pool");
    }
}

/* Vk\CommandPool::allocateBuffers(int $count = 1, bool $primary = true): array<Vk\CommandBuffer> */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_command_pool_allocateBuffers, 0, 0, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "1")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, primary, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandPool, allocateBuffers) {
    zend_long count = 1;
    zend_bool primary = 1;

    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(count)
        Z_PARAM_BOOL(primary)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_pool_object *intern = VK_OBJ(vk_command_pool_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = intern->command_pool,
        .level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
        .commandBufferCount = (uint32_t)count,
    };

    VkCommandBuffer *buffers = ecalloc((uint32_t)count, sizeof(VkCommandBuffer));
    VkResult result = vkAllocateCommandBuffers(dev->device, &alloc_info, buffers);
    if (result != VK_SUCCESS) {
        efree(buffers);
        vk_throw_exception(result, "Failed to allocate command buffers");
        return;
    }

    array_init_size(return_value, (uint32_t)count);
    for (uint32_t i = 0; i < (uint32_t)count; i++) {
        zval cb_zval;
        object_init_ex(&cb_zval, vk_command_buffer_ce);
        vk_command_buffer_object *cb = VK_OBJ(vk_command_buffer_object, Z_OBJ(cb_zval));
        cb->command_buffer = buffers[i];
        ZVAL_COPY(&cb->pool_zval, ZEND_THIS);
        ZVAL_COPY(&cb->device_zval, &intern->device_zval);
        add_next_index_zval(return_value, &cb_zval);
    }

    efree(buffers);
}

/* Vk\CommandPool::reset(int $flags = 0): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_command_pool_reset, 0, 0, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandPool, reset) {
    zend_long flags = 0;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(flags)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_pool_object *intern = VK_OBJ(vk_command_pool_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    VkResult result = vkResetCommandPool(dev->device, intern->command_pool, (VkCommandPoolResetFlags)flags);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to reset command pool");
    }
}

static const zend_function_entry vk_command_pool_methods[] = {
    PHP_ME(VkCommandPool, __construct,     arginfo_vk_command_pool___construct,     ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandPool, allocateBuffers, arginfo_vk_command_pool_allocateBuffers, ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandPool, reset,           arginfo_vk_command_pool_reset,           ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_command_pool_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "CommandPool", vk_command_pool_methods);
    vk_command_pool_ce = zend_register_internal_class(&ce);
    vk_command_pool_ce->create_object = vk_command_pool_create_object;
    vk_command_pool_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_command_pool_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_command_pool_handlers.offset = XtOffsetOf(vk_command_pool_object, std);
    vk_command_pool_handlers.free_obj = vk_command_pool_free_object;
    vk_command_pool_handlers.clone_obj = NULL;
}
