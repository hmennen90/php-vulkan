/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkEvent                                                 |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_event_ce;
static zend_object_handlers vk_event_handlers;

static zend_object *vk_event_create_object(zend_class_entry *ce) {
    vk_event_object *intern = zend_object_alloc(sizeof(vk_event_object), ce);
    intern->event = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_event_handlers;
    return &intern->std;
}

static void vk_event_free_object(zend_object *object) {
    vk_event_object *intern = VK_OBJ(vk_event_object, object);
    if (intern->event != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyEvent(dev->device, intern->event, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\Event::__construct(Vk\Device $device) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_event___construct, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkEvent, __construct) {
    zval *device_zval;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
    ZEND_PARSE_PARAMETERS_END();

    vk_event_object *intern = VK_OBJ(vk_event_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    VkEventCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
    };
    VkResult result = vkCreateEvent(dev->device, &create_info, NULL, &intern->event);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create event");
    }
}

/* Vk\Event::set(): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_event_set, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkEvent, set) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_event_object *intern = VK_OBJ(vk_event_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
    VkResult result = vkSetEvent(dev->device, intern->event);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to set event");
    }
}

/* Vk\Event::reset(): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_event_reset, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkEvent, reset) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_event_object *intern = VK_OBJ(vk_event_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
    VkResult result = vkResetEvent(dev->device, intern->event);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to reset event");
    }
}

/* Vk\Event::isSignaled(): bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_event_isSignaled, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkEvent, isSignaled) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_event_object *intern = VK_OBJ(vk_event_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
    VkResult result = vkGetEventStatus(dev->device, intern->event);
    RETURN_BOOL(result == VK_EVENT_SET);
}

static const zend_function_entry vk_event_methods[] = {
    PHP_ME(VkEvent, __construct, arginfo_vk_event___construct, ZEND_ACC_PUBLIC)
    PHP_ME(VkEvent, set,         arginfo_vk_event_set,         ZEND_ACC_PUBLIC)
    PHP_ME(VkEvent, reset,       arginfo_vk_event_reset,       ZEND_ACC_PUBLIC)
    PHP_ME(VkEvent, isSignaled,  arginfo_vk_event_isSignaled,  ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_event_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Event", vk_event_methods);
    vk_event_ce = zend_register_internal_class(&ce);
    vk_event_ce->create_object = vk_event_create_object;
    vk_event_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_event_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_event_handlers.offset = XtOffsetOf(vk_event_object, std);
    vk_event_handlers.free_obj = vk_event_free_object;
    vk_event_handlers.clone_obj = NULL;
}
