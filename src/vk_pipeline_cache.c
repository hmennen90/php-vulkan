/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkPipelineCache                                         |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_pipeline_cache_ce;
static zend_object_handlers vk_pipeline_cache_handlers;

static zend_object *vk_pipeline_cache_create_object(zend_class_entry *ce) {
    vk_pipeline_cache_object *intern = zend_object_alloc(sizeof(vk_pipeline_cache_object), ce);
    intern->pipeline_cache = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_pipeline_cache_handlers;
    return &intern->std;
}

static void vk_pipeline_cache_free_object(zend_object *object) {
    vk_pipeline_cache_object *intern = VK_OBJ(vk_pipeline_cache_object, object);
    if (intern->pipeline_cache != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyPipelineCache(dev->device, intern->pipeline_cache, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\PipelineCache::__construct(Vk\Device $device, ?string $initialData = null) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_pipeline_cache___construct, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, initialData, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

PHP_METHOD(VkPipelineCache, __construct) {
    zval *device_zval;
    zend_string *initial_data = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_STR_OR_NULL(initial_data)
    ZEND_PARSE_PARAMETERS_END();

    vk_pipeline_cache_object *intern = VK_OBJ(vk_pipeline_cache_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    VkPipelineCacheCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .initialDataSize = initial_data ? ZSTR_LEN(initial_data) : 0,
        .pInitialData = initial_data ? ZSTR_VAL(initial_data) : NULL,
    };

    VkResult result = vkCreatePipelineCache(dev->device, &create_info, NULL, &intern->pipeline_cache);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create pipeline cache");
    }
}

/* Vk\PipelineCache::getData(): string */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pipeline_cache_getData, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPipelineCache, getData) {
    ZEND_PARSE_PARAMETERS_NONE();

    vk_pipeline_cache_object *intern = VK_OBJ(vk_pipeline_cache_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    size_t data_size = 0;
    vkGetPipelineCacheData(dev->device, intern->pipeline_cache, &data_size, NULL);

    if (data_size == 0) {
        RETURN_EMPTY_STRING();
    }

    zend_string *data = zend_string_alloc(data_size, 0);
    VkResult result = vkGetPipelineCacheData(dev->device, intern->pipeline_cache, &data_size, ZSTR_VAL(data));
    if (result != VK_SUCCESS) {
        zend_string_release(data);
        vk_throw_exception(result, "Failed to get pipeline cache data");
        return;
    }
    ZSTR_LEN(data) = data_size;
    ZSTR_VAL(data)[data_size] = '\0';
    RETURN_STR(data);
}

/* Vk\PipelineCache::merge(array<Vk\PipelineCache> $srcCaches): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_pipeline_cache_merge, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, srcCaches, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPipelineCache, merge) {
    HashTable *src_caches;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(src_caches)
    ZEND_PARSE_PARAMETERS_END();

    vk_pipeline_cache_object *intern = VK_OBJ(vk_pipeline_cache_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    uint32_t count = zend_hash_num_elements(src_caches);
    VkPipelineCache *caches = ecalloc(count, sizeof(VkPipelineCache));
    zval *zv;
    uint32_t idx = 0;
    ZEND_HASH_FOREACH_VAL(src_caches, zv) {
        if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), vk_pipeline_cache_ce)) {
            vk_pipeline_cache_object *c = VK_OBJ(vk_pipeline_cache_object, Z_OBJ_P(zv));
            caches[idx++] = c->pipeline_cache;
        }
    } ZEND_HASH_FOREACH_END();

    VkResult result = vkMergePipelineCaches(dev->device, intern->pipeline_cache, idx, caches);
    efree(caches);

    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to merge pipeline caches");
    }
}

static const zend_function_entry vk_pipeline_cache_methods[] = {
    PHP_ME(VkPipelineCache, __construct, arginfo_vk_pipeline_cache___construct, ZEND_ACC_PUBLIC)
    PHP_ME(VkPipelineCache, getData,     arginfo_vk_pipeline_cache_getData,     ZEND_ACC_PUBLIC)
    PHP_ME(VkPipelineCache, merge,       arginfo_vk_pipeline_cache_merge,       ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_pipeline_cache_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "PipelineCache", vk_pipeline_cache_methods);
    vk_pipeline_cache_ce = zend_register_internal_class(&ce);
    vk_pipeline_cache_ce->create_object = vk_pipeline_cache_create_object;
    vk_pipeline_cache_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_pipeline_cache_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_pipeline_cache_handlers.offset = XtOffsetOf(vk_pipeline_cache_object, std);
    vk_pipeline_cache_handlers.free_obj = vk_pipeline_cache_free_object;
    vk_pipeline_cache_handlers.clone_obj = NULL;
}
