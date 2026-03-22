/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkImageView                                             |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_image_view_ce;
static zend_object_handlers vk_image_view_handlers;

static zend_object *vk_image_view_create_object(zend_class_entry *ce) {
    vk_image_view_object *intern = zend_object_alloc(sizeof(vk_image_view_object), ce);
    intern->image_view = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_image_view_handlers;
    return &intern->std;
}

static void vk_image_view_free_object(zend_object *object) {
    vk_image_view_object *intern = VK_OBJ(vk_image_view_object, object);
    if (intern->image_view != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyImageView(dev->device, intern->image_view, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\ImageView::__construct(Vk\Device $device, Vk\Image $image, int $format,
 *                            int $aspectMask = 1, int $viewType = 1) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_iv___construct, 0, 0, 3)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_OBJ_INFO(0, image, Vk\\Image, 0)
    ZEND_ARG_TYPE_INFO(0, format, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, aspectMask, IS_LONG, 0, "1")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, viewType, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

PHP_METHOD(VkImageView, __construct) {
    zval *device_zval, *image_zval;
    zend_long format;
    zend_long aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
    zend_long view_type = VK_IMAGE_VIEW_TYPE_2D;

    ZEND_PARSE_PARAMETERS_START(3, 5)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_OBJECT_OF_CLASS(image_zval, vk_image_ce)
        Z_PARAM_LONG(format)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(aspect_mask)
        Z_PARAM_LONG(view_type)
    ZEND_PARSE_PARAMETERS_END();

    vk_image_view_object *intern = VK_OBJ(vk_image_view_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    vk_image_object *img = VK_OBJ(vk_image_object, Z_OBJ_P(image_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    VkImageViewCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = img->image,
        .viewType = (VkImageViewType)view_type,
        .format = (VkFormat)format,
        .components = {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
        },
        .subresourceRange = {
            .aspectMask = (VkImageAspectFlags)aspect_mask,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    VkResult result = vkCreateImageView(dev->device, &create_info, NULL, &intern->image_view);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create image view");
    }
}

static const zend_function_entry vk_image_view_methods[] = {
    PHP_ME(VkImageView, __construct, arginfo_vk_iv___construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_image_view_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "ImageView", vk_image_view_methods);
    vk_image_view_ce = zend_register_internal_class(&ce);
    vk_image_view_ce->create_object = vk_image_view_create_object;
    vk_image_view_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_image_view_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_image_view_handlers.offset = XtOffsetOf(vk_image_view_object, std);
    vk_image_view_handlers.free_obj = vk_image_view_free_object;
    vk_image_view_handlers.clone_obj = NULL;
}
