/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkSemaphore                                             |
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

/* Vk\Semaphore::__construct(Vk\Device $device) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_semaphore___construct, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkSemaphore, __construct) {
    zval *device_zval;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
    ZEND_PARSE_PARAMETERS_END();

    vk_semaphore_object *intern = VK_OBJ(vk_semaphore_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    VkSemaphoreCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkResult result = vkCreateSemaphore(dev->device, &create_info, NULL, &intern->semaphore);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create semaphore");
    }
}

static const zend_function_entry vk_semaphore_methods[] = {
    PHP_ME(VkSemaphore, __construct, arginfo_vk_semaphore___construct, ZEND_ACC_PUBLIC)
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
