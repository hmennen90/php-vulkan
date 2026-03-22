/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkDescriptorPool                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_descriptor_pool_ce;
static zend_object_handlers vk_descriptor_pool_handlers;

static zend_object *vk_descriptor_pool_create_object(zend_class_entry *ce) {
    vk_descriptor_pool_object *intern = zend_object_alloc(sizeof(vk_descriptor_pool_object), ce);
    intern->pool = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_descriptor_pool_handlers;
    return &intern->std;
}

static void vk_descriptor_pool_free_object(zend_object *object) {
    vk_descriptor_pool_object *intern = VK_OBJ(vk_descriptor_pool_object, object);
    if (intern->pool != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(dev->device, intern->pool, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\DescriptorPool::__construct(Vk\Device $device, int $maxSets, array $poolSizes, int $flags = 0)
 * poolSizes: [['type' => int, 'count' => int], ...] */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_dp___construct, 0, 0, 3)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, maxSets, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, poolSizes, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkDescriptorPool, __construct) {
    zval *device_zval;
    zend_long max_sets;
    HashTable *pool_sizes;
    zend_long flags = 0;

    ZEND_PARSE_PARAMETERS_START(3, 4)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_LONG(max_sets)
        Z_PARAM_ARRAY_HT(pool_sizes)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(flags)
    ZEND_PARSE_PARAMETERS_END();

    vk_descriptor_pool_object *intern = VK_OBJ(vk_descriptor_pool_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    uint32_t ps_count = zend_hash_num_elements(pool_sizes);
    VkDescriptorPoolSize *sizes = ecalloc(ps_count, sizeof(VkDescriptorPoolSize));

    zval *zv;
    uint32_t idx = 0;
    ZEND_HASH_FOREACH_VAL(pool_sizes, zv) {
        if (Z_TYPE_P(zv) != IS_ARRAY) continue;
        HashTable *ps = Z_ARRVAL_P(zv);
        zval *ztype = zend_hash_str_find(ps, "type", sizeof("type") - 1);
        zval *zcount = zend_hash_str_find(ps, "count", sizeof("count") - 1);

        sizes[idx] = (VkDescriptorPoolSize){
            .type = ztype ? (VkDescriptorType)zval_get_long(ztype) : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = zcount ? (uint32_t)zval_get_long(zcount) : 1,
        };
        idx++;
    } ZEND_HASH_FOREACH_END();

    VkDescriptorPoolCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = (uint32_t)max_sets,
        .poolSizeCount = idx,
        .pPoolSizes = sizes,
        .flags = (VkDescriptorPoolCreateFlags)flags,
    };

    VkResult result = vkCreateDescriptorPool(dev->device, &create_info, NULL, &intern->pool);
    efree(sizes);

    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create descriptor pool");
    }
}

/* Vk\DescriptorPool::allocateSets(array<Vk\DescriptorSetLayout> $layouts): array<Vk\DescriptorSet> */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_dp_allocateSets, 0, 1, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, layouts, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkDescriptorPool, allocateSets) {
    HashTable *layouts;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(layouts)
    ZEND_PARSE_PARAMETERS_END();

    vk_descriptor_pool_object *intern = VK_OBJ(vk_descriptor_pool_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);

    uint32_t count = zend_hash_num_elements(layouts);
    VkDescriptorSetLayout *vk_layouts = ecalloc(count, sizeof(VkDescriptorSetLayout));
    VkDescriptorSet *vk_sets = ecalloc(count, sizeof(VkDescriptorSet));

    zval *zv;
    uint32_t idx = 0;
    ZEND_HASH_FOREACH_VAL(layouts, zv) {
        if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), vk_descriptor_set_layout_ce)) {
            vk_descriptor_set_layout_object *l = VK_OBJ(vk_descriptor_set_layout_object, Z_OBJ_P(zv));
            vk_layouts[idx++] = l->layout;
        }
    } ZEND_HASH_FOREACH_END();

    VkDescriptorSetAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = intern->pool,
        .descriptorSetCount = idx,
        .pSetLayouts = vk_layouts,
    };

    VkResult result = vkAllocateDescriptorSets(dev->device, &alloc_info, vk_sets);
    efree(vk_layouts);

    if (result != VK_SUCCESS) {
        efree(vk_sets);
        vk_throw_exception(result, "Failed to allocate descriptor sets");
        return;
    }

    array_init_size(return_value, idx);
    for (uint32_t i = 0; i < idx; i++) {
        zval ds_zval;
        object_init_ex(&ds_zval, vk_descriptor_set_ce);
        vk_descriptor_set_object *ds = VK_OBJ(vk_descriptor_set_object, Z_OBJ(ds_zval));
        ds->set = vk_sets[i];
        ZVAL_COPY(&ds->pool_zval, ZEND_THIS);
        ZVAL_COPY(&ds->device_zval, &intern->device_zval);
        add_next_index_zval(return_value, &ds_zval);
    }

    efree(vk_sets);
}

static const zend_function_entry vk_descriptor_pool_methods[] = {
    PHP_ME(VkDescriptorPool, __construct,  arginfo_vk_dp___construct,  ZEND_ACC_PUBLIC)
    PHP_ME(VkDescriptorPool, allocateSets, arginfo_vk_dp_allocateSets, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_descriptor_pool_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "DescriptorPool", vk_descriptor_pool_methods);
    vk_descriptor_pool_ce = zend_register_internal_class(&ce);
    vk_descriptor_pool_ce->create_object = vk_descriptor_pool_create_object;
    vk_descriptor_pool_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_descriptor_pool_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_descriptor_pool_handlers.offset = XtOffsetOf(vk_descriptor_pool_object, std);
    vk_descriptor_pool_handlers.free_obj = vk_descriptor_pool_free_object;
    vk_descriptor_pool_handlers.clone_obj = NULL;
}
