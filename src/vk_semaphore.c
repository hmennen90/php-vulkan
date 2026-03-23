/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkSemaphore (Binary + Timeline)                         |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_semaphore_ce;
static zend_object_handlers vk_semaphore_handlers;

static zend_object *vk_semaphore_create_object(zend_class_entry *ce) {
    vk_semaphore_object *intern = zend_object_alloc(sizeof(vk_semaphore_object), ce);
    intern->semaphore = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_semaphore_handlers;
    return &intern->std;
}

static void vk_semaphore_free_object(zend_object *object) {
    vk_semaphore_object *intern = VK_OBJ(vk_semaphore_object, object);
    if (intern->semaphore != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroySemaphore(dev->device, intern->semaphore, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\Semaphore::__construct(Vk\Device $device, bool $timeline = false, int $initialValue = 0) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_semaphore___construct, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeline, _IS_BOOL, 0, "false")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, initialValue, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkSemaphore, __construct) {
    zval *device_zval;
    zend_bool timeline = 0;
    zend_long initial_value = 0;

    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(timeline)
        Z_PARAM_LONG(initial_value)
    ZEND_PARSE_PARAMETERS_END();

    vk_semaphore_object *intern = VK_OBJ(vk_semaphore_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    VkSemaphoreTypeCreateInfo type_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .semaphoreType = timeline ? VK_SEMAPHORE_TYPE_TIMELINE : VK_SEMAPHORE_TYPE_BINARY,
        .initialValue = (uint64_t)initial_value,
    };

    VkSemaphoreCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = timeline ? &type_info : NULL,
    };

    VkResult result = vkCreateSemaphore(dev->device, &create_info, NULL, &intern->semaphore);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create semaphore");
    }
}

/* Vk\Semaphore::wait(int $value, int $timeout = -1): bool (timeline only) */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_semaphore_wait, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

PHP_METHOD(VkSemaphore, wait) {
    zend_long value, timeout = -1;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_LONG(value)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(timeout)
    ZEND_PARSE_PARAMETERS_END();

    vk_semaphore_object *intern = VK_OBJ(vk_semaphore_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    uint64_t vk_timeout = (timeout < 0) ? UINT64_MAX : (uint64_t)timeout;

    VkSemaphoreWaitInfo wait_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .semaphoreCount = 1,
        .pSemaphores = &intern->semaphore,
        .pValues = (uint64_t[]){(uint64_t)value},
    };

    VkResult result = vkWaitSemaphores(dev->device, &wait_info, vk_timeout);
    if (result == VK_SUCCESS) {
        RETURN_TRUE;
    } else if (result == VK_TIMEOUT) {
        RETURN_FALSE;
    } else {
        vk_throw_exception(result, "Failed to wait for timeline semaphore");
    }
}

/* Vk\Semaphore::signal(int $value): void (timeline only) */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_semaphore_signal, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkSemaphore, signal) {
    zend_long value;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(value)
    ZEND_PARSE_PARAMETERS_END();

    vk_semaphore_object *intern = VK_OBJ(vk_semaphore_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    VkSemaphoreSignalInfo signal_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO,
        .semaphore = intern->semaphore,
        .value = (uint64_t)value,
    };

    VkResult result = vkSignalSemaphore(dev->device, &signal_info);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to signal timeline semaphore");
    }
}

/* Vk\Semaphore::getValue(): int (timeline only) */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_semaphore_getValue, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkSemaphore, getValue) {
    ZEND_PARSE_PARAMETERS_NONE();

    vk_semaphore_object *intern = VK_OBJ(vk_semaphore_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    uint64_t value = 0;
    VkResult result = vkGetSemaphoreCounterValue(dev->device, intern->semaphore, &value);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to get semaphore counter value");
        return;
    }
    RETURN_LONG((zend_long)value);
}

static const zend_function_entry vk_semaphore_methods[] = {
    PHP_ME(VkSemaphore, __construct, arginfo_vk_semaphore___construct, ZEND_ACC_PUBLIC)
    PHP_ME(VkSemaphore, wait,        arginfo_vk_semaphore_wait,        ZEND_ACC_PUBLIC)
    PHP_ME(VkSemaphore, signal,      arginfo_vk_semaphore_signal,      ZEND_ACC_PUBLIC)
    PHP_ME(VkSemaphore, getValue,    arginfo_vk_semaphore_getValue,    ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_semaphore_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Semaphore", vk_semaphore_methods);
    vk_semaphore_ce = zend_register_internal_class(&ce);
    vk_semaphore_ce->create_object = vk_semaphore_create_object;
    vk_semaphore_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_semaphore_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_semaphore_handlers.offset = XtOffsetOf(vk_semaphore_object, std);
    vk_semaphore_handlers.free_obj = vk_semaphore_free_object;
    vk_semaphore_handlers.clone_obj = NULL;
}
