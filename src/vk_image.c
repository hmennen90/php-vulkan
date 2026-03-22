/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkImage                                                 |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_image_ce;
static zend_object_handlers vk_image_handlers;

static zend_object *vk_image_create_object(zend_class_entry *ce) {
    vk_image_object *intern = zend_object_alloc(sizeof(vk_image_object), ce);
    intern->image = VK_NULL_HANDLE;
    intern->owns_image = 1;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_image_handlers;
    return &intern->std;
}

static void vk_image_free_object(zend_object *object) {
    vk_image_object *intern = VK_OBJ(vk_image_object, object);
    if (intern->image != VK_NULL_HANDLE && intern->owns_image && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyImage(dev->device, intern->image, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\Image::__construct(Vk\Device $device, int $width, int $height, int $format,
 *                        int $usage, int $tiling = 0, int $samples = 1) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_image___construct, 0, 0, 5)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, format, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, usage, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, tiling, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, samples, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

PHP_METHOD(VkImage, __construct) {
    zval *device_zval;
    zend_long width, height, format, usage;
    zend_long tiling = VK_IMAGE_TILING_OPTIMAL;
    zend_long samples = VK_SAMPLE_COUNT_1_BIT;

    ZEND_PARSE_PARAMETERS_START(5, 7)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_LONG(width)
        Z_PARAM_LONG(height)
        Z_PARAM_LONG(format)
        Z_PARAM_LONG(usage)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(tiling)
        Z_PARAM_LONG(samples)
    ZEND_PARSE_PARAMETERS_END();

    vk_image_object *intern = VK_OBJ(vk_image_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    VkImageCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent = {(uint32_t)width, (uint32_t)height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .format = (VkFormat)format,
        .tiling = (VkImageTiling)tiling,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = (VkImageUsageFlags)usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .samples = (VkSampleCountFlagBits)samples,
    };

    VkResult result = vkCreateImage(dev->device, &create_info, NULL, &intern->image);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create image");
    }
}

/* Vk\Image::getMemoryRequirements(): array */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_image_getMemoryRequirements, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkImage, getMemoryRequirements) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_image_object *intern = VK_OBJ(vk_image_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    VkMemoryRequirements reqs;
    vkGetImageMemoryRequirements(dev->device, intern->image, &reqs);

    array_init(return_value);
    add_assoc_long(return_value, "size", (zend_long)reqs.size);
    add_assoc_long(return_value, "alignment", (zend_long)reqs.alignment);
    add_assoc_long(return_value, "memoryTypeBits", (zend_long)reqs.memoryTypeBits);
}

/* Vk\Image::bindMemory(Vk\DeviceMemory $memory, int $offset = 0): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_image_bindMemory, 0, 1, IS_VOID, 0)
    ZEND_ARG_OBJ_INFO(0, memory, Vk\\DeviceMemory, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkImage, bindMemory) {
    zval *memory_zval;
    zend_long offset = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_OBJECT_OF_CLASS(memory_zval, vk_device_memory_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(offset)
    ZEND_PARSE_PARAMETERS_END();

    vk_image_object *intern = VK_OBJ(vk_image_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
    vk_device_memory_object *mem = VK_OBJ(vk_device_memory_object, Z_OBJ_P(memory_zval));

    VkResult result = vkBindImageMemory(dev->device, intern->image, mem->memory, (VkDeviceSize)offset);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to bind image memory");
    }
}

static const zend_function_entry vk_image_methods[] = {
    PHP_ME(VkImage, __construct,          arginfo_vk_image___construct,          ZEND_ACC_PUBLIC)
    PHP_ME(VkImage, getMemoryRequirements, arginfo_vk_image_getMemoryRequirements, ZEND_ACC_PUBLIC)
    PHP_ME(VkImage, bindMemory,           arginfo_vk_image_bindMemory,           ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_image_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Image", vk_image_methods);
    vk_image_ce = zend_register_internal_class(&ce);
    vk_image_ce->create_object = vk_image_create_object;
    vk_image_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_image_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_image_handlers.offset = XtOffsetOf(vk_image_object, std);
    vk_image_handlers.free_obj = vk_image_free_object;
    vk_image_handlers.clone_obj = NULL;
}
