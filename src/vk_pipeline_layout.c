/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkPipelineLayout                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_pipeline_layout_ce;
static zend_object_handlers vk_pipeline_layout_handlers;

static zend_object *vk_pipeline_layout_create_object(zend_class_entry *ce) {
    vk_pipeline_layout_object *intern = zend_object_alloc(sizeof(vk_pipeline_layout_object), ce);
    intern->layout = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_pipeline_layout_handlers;
    return &intern->std;
}

static void vk_pipeline_layout_free_object(zend_object *object) {
    vk_pipeline_layout_object *intern = VK_OBJ(vk_pipeline_layout_object, object);
    if (intern->layout != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(dev->device, intern->layout, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\PipelineLayout::__construct(Vk\Device $device,
 *     array<Vk\DescriptorSetLayout> $setLayouts = [],
 *     array $pushConstantRanges = [])
 * pushConstantRanges: [['stageFlags' => int, 'offset' => int, 'size' => int], ...] */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_pl___construct, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, setLayouts, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, pushConstantRanges, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPipelineLayout, __construct) {
    zval *device_zval;
    HashTable *set_layouts = NULL;
    HashTable *push_constants = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY_HT(set_layouts)
        Z_PARAM_ARRAY_HT(push_constants)
    ZEND_PARSE_PARAMETERS_END();

    vk_pipeline_layout_object *intern = VK_OBJ(vk_pipeline_layout_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    /* Set layouts */
    uint32_t sl_count = set_layouts ? zend_hash_num_elements(set_layouts) : 0;
    VkDescriptorSetLayout *vk_layouts = NULL;
    if (sl_count > 0) {
        vk_layouts = ecalloc(sl_count, sizeof(VkDescriptorSetLayout));
        zval *zv;
        uint32_t idx = 0;
        ZEND_HASH_FOREACH_VAL(set_layouts, zv) {
            if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), vk_descriptor_set_layout_ce)) {
                vk_descriptor_set_layout_object *l = VK_OBJ(vk_descriptor_set_layout_object, Z_OBJ_P(zv));
                vk_layouts[idx++] = l->layout;
            }
        } ZEND_HASH_FOREACH_END();
        sl_count = idx;
    }

    /* Push constant ranges */
    uint32_t pc_count = push_constants ? zend_hash_num_elements(push_constants) : 0;
    VkPushConstantRange *pc_ranges = NULL;
    if (pc_count > 0) {
        pc_ranges = ecalloc(pc_count, sizeof(VkPushConstantRange));
        zval *zv;
        uint32_t idx = 0;
        ZEND_HASH_FOREACH_VAL(push_constants, zv) {
            if (Z_TYPE_P(zv) != IS_ARRAY) continue;
            HashTable *pc = Z_ARRVAL_P(zv);
            zval *zstage = zend_hash_str_find(pc, "stageFlags", sizeof("stageFlags") - 1);
            zval *zoffset = zend_hash_str_find(pc, "offset", sizeof("offset") - 1);
            zval *zsize = zend_hash_str_find(pc, "size", sizeof("size") - 1);

            pc_ranges[idx] = (VkPushConstantRange){
                .stageFlags = zstage ? (VkShaderStageFlags)zval_get_long(zstage) : VK_SHADER_STAGE_ALL,
                .offset = zoffset ? (uint32_t)zval_get_long(zoffset) : 0,
                .size = zsize ? (uint32_t)zval_get_long(zsize) : 0,
            };
            idx++;
        } ZEND_HASH_FOREACH_END();
        pc_count = idx;
    }

    VkPipelineLayoutCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = sl_count,
        .pSetLayouts = vk_layouts,
        .pushConstantRangeCount = pc_count,
        .pPushConstantRanges = pc_ranges,
    };

    VkResult result = vkCreatePipelineLayout(dev->device, &create_info, NULL, &intern->layout);
    if (vk_layouts) efree(vk_layouts);
    if (pc_ranges) efree(pc_ranges);

    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create pipeline layout");
    }
}

static const zend_function_entry vk_pipeline_layout_methods[] = {
    PHP_ME(VkPipelineLayout, __construct, arginfo_vk_pl___construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_pipeline_layout_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "PipelineLayout", vk_pipeline_layout_methods);
    vk_pipeline_layout_ce = zend_register_internal_class(&ce);
    vk_pipeline_layout_ce->create_object = vk_pipeline_layout_create_object;
    vk_pipeline_layout_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_pipeline_layout_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_pipeline_layout_handlers.offset = XtOffsetOf(vk_pipeline_layout_object, std);
    vk_pipeline_layout_handlers.free_obj = vk_pipeline_layout_free_object;
    vk_pipeline_layout_handlers.clone_obj = NULL;
}
