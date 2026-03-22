/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkSwapchainKHR                                          |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_swapchain_ce;
static zend_object_handlers vk_swapchain_handlers;

static zend_object *vk_swapchain_create_object(zend_class_entry *ce) {
    vk_swapchain_object *intern = zend_object_alloc(sizeof(vk_swapchain_object), ce);
    intern->swapchain = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    ZVAL_UNDEF(&intern->surface_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_swapchain_handlers;
    return &intern->std;
}

static void vk_swapchain_free_object(zend_object *object) {
    vk_swapchain_object *intern = VK_OBJ(vk_swapchain_object, object);
    if (intern->swapchain != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(dev->device, intern->swapchain, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zval_ptr_dtor(&intern->surface_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\Swapchain::__construct(Vk\Device $device, Vk\Surface $surface, array $config)
 * config: minImageCount, format, colorSpace, width, height, imageUsage,
 *         presentMode, compositeAlpha, clipped, preTransform, imageArrayLayers */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_swapchain___construct, 0, 0, 3)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_OBJ_INFO(0, surface, Vk\\Surface, 0)
    ZEND_ARG_TYPE_INFO(0, config, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkSwapchain, __construct) {
    zval *device_zval, *surface_zval;
    HashTable *config;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_OBJECT_OF_CLASS(surface_zval, vk_surface_ce)
        Z_PARAM_ARRAY_HT(config)
    ZEND_PARSE_PARAMETERS_END();

    vk_swapchain_object *intern = VK_OBJ(vk_swapchain_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    vk_surface_object *surface = VK_OBJ(vk_surface_object, Z_OBJ_P(surface_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);
    ZVAL_COPY(&intern->surface_zval, surface_zval);

    zval *zv;
    #define CFG_LONG(key, def) \
        ((zv = zend_hash_str_find(config, key, sizeof(key) - 1)) ? (uint32_t)zval_get_long(zv) : (def))

    VkSwapchainCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface->surface,
        .minImageCount = CFG_LONG("minImageCount", 2),
        .imageFormat = (VkFormat)CFG_LONG("format", VK_FORMAT_B8G8R8A8_SRGB),
        .imageColorSpace = (VkColorSpaceKHR)CFG_LONG("colorSpace", VK_COLOR_SPACE_SRGB_NONLINEAR_KHR),
        .imageExtent = {CFG_LONG("width", 800), CFG_LONG("height", 600)},
        .imageArrayLayers = CFG_LONG("imageArrayLayers", 1),
        .imageUsage = (VkImageUsageFlags)CFG_LONG("imageUsage", VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = (VkSurfaceTransformFlagBitsKHR)CFG_LONG("preTransform", VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR),
        .compositeAlpha = (VkCompositeAlphaFlagBitsKHR)CFG_LONG("compositeAlpha", VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR),
        .presentMode = (VkPresentModeKHR)CFG_LONG("presentMode", VK_PRESENT_MODE_FIFO_KHR),
        .clipped = CFG_LONG("clipped", VK_TRUE),
    };
    #undef CFG_LONG

    VkResult result = vkCreateSwapchainKHR(dev->device, &create_info, NULL, &intern->swapchain);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create swapchain");
    }
}

/* Vk\Swapchain::getImages(): array<Vk\Image> */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_swapchain_getImages, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkSwapchain, getImages) {
    ZEND_PARSE_PARAMETERS_NONE();

    vk_swapchain_object *intern = VK_OBJ(vk_swapchain_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    uint32_t count = 0;
    vkGetSwapchainImagesKHR(dev->device, intern->swapchain, &count, NULL);
    VkImage *images = ecalloc(count, sizeof(VkImage));
    vkGetSwapchainImagesKHR(dev->device, intern->swapchain, &count, images);

    array_init_size(return_value, count);
    for (uint32_t i = 0; i < count; i++) {
        zval img_zval;
        object_init_ex(&img_zval, vk_image_ce);
        vk_image_object *img = VK_OBJ(vk_image_object, Z_OBJ(img_zval));
        img->image = images[i];
        img->owns_image = 0; /* swapchain owns these */
        ZVAL_COPY(&img->device_zval, &intern->device_zval);
        add_next_index_zval(return_value, &img_zval);
    }
    efree(images);
}

/* Vk\Swapchain::acquireNextImage(Vk\Semaphore $semaphore, ?Vk\Fence $fence = null,
 *                                 int $timeout = -1): int */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_swapchain_acquireNextImage, 0, 1, IS_LONG, 0)
    ZEND_ARG_OBJ_INFO(0, semaphore, Vk\\Semaphore, 0)
    ZEND_ARG_OBJ_INFO(0, fence, Vk\\Fence, 1)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

PHP_METHOD(VkSwapchain, acquireNextImage) {
    zval *sem_zval, *fence_zval = NULL;
    zend_long timeout = -1;

    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_OBJECT_OF_CLASS(sem_zval, vk_semaphore_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_OBJECT_OF_CLASS_OR_NULL(fence_zval, vk_fence_ce)
        Z_PARAM_LONG(timeout)
    ZEND_PARSE_PARAMETERS_END();

    vk_swapchain_object *intern = VK_OBJ(vk_swapchain_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
    vk_semaphore_object *sem = VK_OBJ(vk_semaphore_object, Z_OBJ_P(sem_zval));

    VkFence fence = VK_NULL_HANDLE;
    if (fence_zval) {
        vk_fence_object *f = VK_OBJ(vk_fence_object, Z_OBJ_P(fence_zval));
        fence = f->fence;
    }

    uint64_t vk_timeout = (timeout < 0) ? UINT64_MAX : (uint64_t)timeout;
    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(dev->device, intern->swapchain, vk_timeout,
        sem->semaphore, fence, &image_index);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        vk_throw_exception(result, "Failed to acquire swapchain image");
        return;
    }

    RETURN_LONG((zend_long)image_index);
}

static const zend_function_entry vk_swapchain_methods[] = {
    PHP_ME(VkSwapchain, __construct,      arginfo_vk_swapchain___construct,      ZEND_ACC_PUBLIC)
    PHP_ME(VkSwapchain, getImages,        arginfo_vk_swapchain_getImages,        ZEND_ACC_PUBLIC)
    PHP_ME(VkSwapchain, acquireNextImage, arginfo_vk_swapchain_acquireNextImage, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_swapchain_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Swapchain", vk_swapchain_methods);
    vk_swapchain_ce = zend_register_internal_class(&ce);
    vk_swapchain_ce->create_object = vk_swapchain_create_object;
    vk_swapchain_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_swapchain_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_swapchain_handlers.offset = XtOffsetOf(vk_swapchain_object, std);
    vk_swapchain_handlers.free_obj = vk_swapchain_free_object;
    vk_swapchain_handlers.clone_obj = NULL;
}
