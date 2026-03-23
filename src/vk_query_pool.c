/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkQueryPool                                             |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_query_pool_ce;
static zend_object_handlers vk_query_pool_handlers;

static zend_object *vk_query_pool_create_object(zend_class_entry *ce) {
    vk_query_pool_object *intern = zend_object_alloc(sizeof(vk_query_pool_object), ce);
    intern->query_pool = VK_NULL_HANDLE;
    intern->query_count = 0;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_query_pool_handlers;
    return &intern->std;
}

static void vk_query_pool_free_object(zend_object *object) {
    vk_query_pool_object *intern = VK_OBJ(vk_query_pool_object, object);
    if (intern->query_pool != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyQueryPool(dev->device, intern->query_pool, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\QueryPool::__construct(Vk\Device $device, int $queryType, int $queryCount,
 *     int $pipelineStatistics = 0) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_query_pool___construct, 0, 0, 3)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, queryType, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, queryCount, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pipelineStatistics, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkQueryPool, __construct) {
    zval *device_zval;
    zend_long query_type, query_count, pipeline_statistics = 0;

    ZEND_PARSE_PARAMETERS_START(3, 4)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_LONG(query_type)
        Z_PARAM_LONG(query_count)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(pipeline_statistics)
    ZEND_PARSE_PARAMETERS_END();

    vk_query_pool_object *intern = VK_OBJ(vk_query_pool_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);
    intern->query_count = (uint32_t)query_count;

    VkQueryPoolCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
        .queryType = (VkQueryType)query_type,
        .queryCount = (uint32_t)query_count,
        .pipelineStatistics = (VkQueryPipelineStatisticFlags)pipeline_statistics,
    };

    VkResult result = vkCreateQueryPool(dev->device, &create_info, NULL, &intern->query_pool);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create query pool");
    }
}

/* Vk\QueryPool::getResults(int $firstQuery, int $queryCount, int $stride = 8,
 *     int $flags = 0): string */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_query_pool_getResults, 0, 2, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, firstQuery, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, queryCount, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, stride, IS_LONG, 0, "8")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkQueryPool, getResults) {
    zend_long first_query, query_count;
    zend_long stride = 8;
    zend_long flags = 0;

    ZEND_PARSE_PARAMETERS_START(2, 4)
        Z_PARAM_LONG(first_query)
        Z_PARAM_LONG(query_count)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(stride)
        Z_PARAM_LONG(flags)
    ZEND_PARSE_PARAMETERS_END();

    vk_query_pool_object *intern = VK_OBJ(vk_query_pool_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    size_t data_size = (size_t)(query_count * stride);
    zend_string *data = zend_string_alloc(data_size, 0);

    VkResult result = vkGetQueryPoolResults(dev->device, intern->query_pool,
        (uint32_t)first_query, (uint32_t)query_count,
        data_size, ZSTR_VAL(data), (VkDeviceSize)stride,
        (VkQueryResultFlags)flags | VK_QUERY_RESULT_64_BIT);

    if (result != VK_SUCCESS && result != VK_NOT_READY) {
        zend_string_release(data);
        vk_throw_exception(result, "Failed to get query pool results");
        return;
    }

    ZSTR_LEN(data) = data_size;
    ZSTR_VAL(data)[data_size] = '\0';
    RETURN_STR(data);
}

/* Vk\QueryPool::reset(int $firstQuery, int $queryCount): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_query_pool_reset, 0, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, firstQuery, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, queryCount, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkQueryPool, reset) {
    zend_long first_query, query_count;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(first_query)
        Z_PARAM_LONG(query_count)
    ZEND_PARSE_PARAMETERS_END();

    vk_query_pool_object *intern = VK_OBJ(vk_query_pool_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    vkResetQueryPool(dev->device, intern->query_pool,
        (uint32_t)first_query, (uint32_t)query_count);
}

static const zend_function_entry vk_query_pool_methods[] = {
    PHP_ME(VkQueryPool, __construct, arginfo_vk_query_pool___construct, ZEND_ACC_PUBLIC)
    PHP_ME(VkQueryPool, getResults,  arginfo_vk_query_pool_getResults,  ZEND_ACC_PUBLIC)
    PHP_ME(VkQueryPool, reset,       arginfo_vk_query_pool_reset,       ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_query_pool_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "QueryPool", vk_query_pool_methods);
    vk_query_pool_ce = zend_register_internal_class(&ce);
    vk_query_pool_ce->create_object = vk_query_pool_create_object;
    vk_query_pool_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_query_pool_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_query_pool_handlers.offset = XtOffsetOf(vk_query_pool_object, std);
    vk_query_pool_handlers.free_obj = vk_query_pool_free_object;
    vk_query_pool_handlers.clone_obj = NULL;
}
