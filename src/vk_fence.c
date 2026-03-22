/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkFence                                                 |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_fence_ce;
static zend_object_handlers vk_fence_handlers;

static zend_object *vk_fence_create_object(zend_class_entry *ce) {
    vk_fence_object *intern = zend_object_alloc(sizeof(vk_fence_object), ce);
    intern->fence = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_fence_handlers;
    return &intern->std;
}

static void vk_fence_free_object(zend_object *object) {
    vk_fence_object *intern = VK_OBJ(vk_fence_object, object);
    if (intern->fence != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyFence(dev->device, intern->fence, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\Fence::__construct(Vk\Device $device, bool $signaled = false) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_fence___construct, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, signaled, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

PHP_METHOD(VkFence, __construct) {
    zval *device_zval;
    zend_bool signaled = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(signaled)
    ZEND_PARSE_PARAMETERS_END();

    vk_fence_object *intern = VK_OBJ(vk_fence_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    VkFenceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0,
    };

    VkResult result = vkCreateFence(dev->device, &create_info, NULL, &intern->fence);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create fence");
    }
}

/* Vk\Fence::wait(int $timeout = UINT64_MAX): bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_fence_wait, 0, 0, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

PHP_METHOD(VkFence, wait) {
    zend_long timeout = -1;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(timeout)
    ZEND_PARSE_PARAMETERS_END();

    vk_fence_object *intern = VK_OBJ(vk_fence_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    uint64_t vk_timeout = (timeout < 0) ? UINT64_MAX : (uint64_t)timeout;
    VkResult result = vkWaitForFences(dev->device, 1, &intern->fence, VK_TRUE, vk_timeout);

    if (result == VK_SUCCESS) {
        RETURN_TRUE;
    } else if (result == VK_TIMEOUT) {
        RETURN_FALSE;
    } else {
        vk_throw_exception(result, "Failed to wait for fence");
    }
}

/* Vk\Fence::reset(): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_fence_reset, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkFence, reset) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_fence_object *intern = VK_OBJ(vk_fence_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    VkResult result = vkResetFences(dev->device, 1, &intern->fence);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to reset fence");
    }
}

/* Vk\Fence::isSignaled(): bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_fence_isSignaled, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkFence, isSignaled) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_fence_object *intern = VK_OBJ(vk_fence_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    VkResult result = vkGetFenceStatus(dev->device, intern->fence);
    RETURN_BOOL(result == VK_SUCCESS);
}

static const zend_function_entry vk_fence_methods[] = {
    PHP_ME(VkFence, __construct, arginfo_vk_fence___construct, ZEND_ACC_PUBLIC)
    PHP_ME(VkFence, wait,        arginfo_vk_fence_wait,        ZEND_ACC_PUBLIC)
    PHP_ME(VkFence, reset,       arginfo_vk_fence_reset,       ZEND_ACC_PUBLIC)
    PHP_ME(VkFence, isSignaled,  arginfo_vk_fence_isSignaled,  ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_fence_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Fence", vk_fence_methods);
    vk_fence_ce = zend_register_internal_class(&ce);
    vk_fence_ce->create_object = vk_fence_create_object;
    vk_fence_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_fence_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_fence_handlers.offset = XtOffsetOf(vk_fence_object, std);
    vk_fence_handlers.free_obj = vk_fence_free_object;
    vk_fence_handlers.clone_obj = NULL;
}
