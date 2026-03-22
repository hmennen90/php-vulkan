/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkFramebuffer                                           |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_framebuffer_ce;
static zend_object_handlers vk_framebuffer_handlers;

static zend_object *vk_framebuffer_create_object(zend_class_entry *ce) {
    vk_framebuffer_object *intern = zend_object_alloc(sizeof(vk_framebuffer_object), ce);
    intern->framebuffer = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_framebuffer_handlers;
    return &intern->std;
}

static void vk_framebuffer_free_object(zend_object *object) {
    vk_framebuffer_object *intern = VK_OBJ(vk_framebuffer_object, object);
    if (intern->framebuffer != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(dev->device, intern->framebuffer, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\Framebuffer::__construct(Vk\Device $device, Vk\RenderPass $renderPass,
 *     array<Vk\ImageView> $attachments, int $width, int $height, int $layers = 1) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_fb___construct, 0, 0, 5)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_OBJ_INFO(0, renderPass, Vk\\RenderPass, 0)
    ZEND_ARG_TYPE_INFO(0, attachments, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, layers, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

PHP_METHOD(VkFramebuffer, __construct) {
    zval *device_zval, *rp_zval;
    HashTable *attachments;
    zend_long width, height, layers = 1;

    ZEND_PARSE_PARAMETERS_START(5, 6)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_OBJECT_OF_CLASS(rp_zval, vk_render_pass_ce)
        Z_PARAM_ARRAY_HT(attachments)
        Z_PARAM_LONG(width)
        Z_PARAM_LONG(height)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(layers)
    ZEND_PARSE_PARAMETERS_END();

    vk_framebuffer_object *intern = VK_OBJ(vk_framebuffer_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    vk_render_pass_object *rp = VK_OBJ(vk_render_pass_object, Z_OBJ_P(rp_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    uint32_t att_count = zend_hash_num_elements(attachments);
    VkImageView *views = ecalloc(att_count, sizeof(VkImageView));
    zval *zv;
    uint32_t idx = 0;
    ZEND_HASH_FOREACH_VAL(attachments, zv) {
        if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), vk_image_view_ce)) {
            vk_image_view_object *iv = VK_OBJ(vk_image_view_object, Z_OBJ_P(zv));
            views[idx++] = iv->image_view;
        }
    } ZEND_HASH_FOREACH_END();

    VkFramebufferCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = rp->render_pass,
        .attachmentCount = idx,
        .pAttachments = views,
        .width = (uint32_t)width,
        .height = (uint32_t)height,
        .layers = (uint32_t)layers,
    };

    VkResult result = vkCreateFramebuffer(dev->device, &create_info, NULL, &intern->framebuffer);
    efree(views);

    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create framebuffer");
    }
}

static const zend_function_entry vk_framebuffer_methods[] = {
    PHP_ME(VkFramebuffer, __construct, arginfo_vk_fb___construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_framebuffer_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Framebuffer", vk_framebuffer_methods);
    vk_framebuffer_ce = zend_register_internal_class(&ce);
    vk_framebuffer_ce->create_object = vk_framebuffer_create_object;
    vk_framebuffer_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_framebuffer_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_framebuffer_handlers.offset = XtOffsetOf(vk_framebuffer_object, std);
    vk_framebuffer_handlers.free_obj = vk_framebuffer_free_object;
    vk_framebuffer_handlers.clone_obj = NULL;
}
