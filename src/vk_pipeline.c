/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkPipeline (Compute + Graphics)                         |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_pipeline_ce;
static zend_object_handlers vk_pipeline_handlers;

static zend_object *vk_pipeline_create_object(zend_class_entry *ce) {
    vk_pipeline_object *intern = zend_object_alloc(sizeof(vk_pipeline_object), ce);
    intern->pipeline = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_pipeline_handlers;
    return &intern->std;
}

static void vk_pipeline_free_object(zend_object *object) {
    vk_pipeline_object *intern = VK_OBJ(vk_pipeline_object, object);
    if (intern->pipeline != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyPipeline(dev->device, intern->pipeline, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\Pipeline::createCompute(Vk\Device $device, Vk\PipelineLayout $layout,
 *     Vk\ShaderModule $shader, string $entryPoint = "main",
 *     ?Vk\PipelineCache $cache = null, ?array $specialization = null): Vk\Pipeline */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_vk_pipeline_createCompute, 0, 3, Vk\\Pipeline, 0)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_OBJ_INFO(0, layout, Vk\\PipelineLayout, 0)
    ZEND_ARG_OBJ_INFO(0, shader, Vk\\ShaderModule, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, entryPoint, IS_STRING, 0, "\"main\"")
    ZEND_ARG_OBJ_INFO(0, cache, Vk\\PipelineCache, 1)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, specialization, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

PHP_METHOD(VkPipeline, createCompute) {
    zval *device_zval, *layout_zval, *shader_zval;
    zend_string *entry_point = NULL;
    zval *cache_zval = NULL;
    HashTable *specialization = NULL;

    ZEND_PARSE_PARAMETERS_START(3, 6)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_OBJECT_OF_CLASS(layout_zval, vk_pipeline_layout_ce)
        Z_PARAM_OBJECT_OF_CLASS(shader_zval, vk_shader_module_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_STR(entry_point)
        Z_PARAM_OBJECT_OF_CLASS_OR_NULL(cache_zval, vk_pipeline_cache_ce)
        Z_PARAM_ARRAY_HT_OR_NULL(specialization)
    ZEND_PARSE_PARAMETERS_END();

    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    vk_pipeline_layout_object *layout = VK_OBJ(vk_pipeline_layout_object, Z_OBJ_P(layout_zval));
    vk_shader_module_object *shader = VK_OBJ(vk_shader_module_object, Z_OBJ_P(shader_zval));

    VkPipelineCache vk_cache = VK_NULL_HANDLE;
    if (cache_zval) {
        vk_pipeline_cache_object *cache = VK_OBJ(vk_pipeline_cache_object, Z_OBJ_P(cache_zval));
        vk_cache = cache->pipeline_cache;
    }

    /* Specialization constants: ['data' => string, 'entries' => [['id' => int, 'offset' => int, 'size' => int], ...]] */
    VkSpecializationInfo spec_info = {0};
    VkSpecializationMapEntry *spec_entries = NULL;
    if (specialization) {
        zval *zdata = zend_hash_str_find(specialization, "data", sizeof("data") - 1);
        zval *zentries = zend_hash_str_find(specialization, "entries", sizeof("entries") - 1);
        if (zdata && Z_TYPE_P(zdata) == IS_STRING && zentries && Z_TYPE_P(zentries) == IS_ARRAY) {
            uint32_t entry_count = zend_hash_num_elements(Z_ARRVAL_P(zentries));
            spec_entries = ecalloc(entry_count, sizeof(VkSpecializationMapEntry));
            zval *ze;
            uint32_t ei = 0;
            ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zentries), ze) {
                if (Z_TYPE_P(ze) != IS_ARRAY) continue;
                HashTable *e = Z_ARRVAL_P(ze);
                zval *zid = zend_hash_str_find(e, "id", sizeof("id") - 1);
                zval *zoff = zend_hash_str_find(e, "offset", sizeof("offset") - 1);
                zval *zsize = zend_hash_str_find(e, "size", sizeof("size") - 1);
                spec_entries[ei] = (VkSpecializationMapEntry){
                    .constantID = zid ? (uint32_t)zval_get_long(zid) : ei,
                    .offset = zoff ? (uint32_t)zval_get_long(zoff) : 0,
                    .size = zsize ? (size_t)zval_get_long(zsize) : 4,
                };
                ei++;
            } ZEND_HASH_FOREACH_END();
            spec_info.mapEntryCount = ei;
            spec_info.pMapEntries = spec_entries;
            spec_info.dataSize = Z_STRLEN_P(zdata);
            spec_info.pData = Z_STRVAL_P(zdata);
        }
    }

    VkComputePipelineCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = shader->shader_module,
            .pName = entry_point ? ZSTR_VAL(entry_point) : "main",
            .pSpecializationInfo = spec_entries ? &spec_info : NULL,
        },
        .layout = layout->layout,
    };

    object_init_ex(return_value, vk_pipeline_ce);
    vk_pipeline_object *intern = VK_OBJ(vk_pipeline_object, Z_OBJ_P(return_value));
    ZVAL_COPY(&intern->device_zval, device_zval);

    VkResult result = vkCreateComputePipelines(dev->device, vk_cache, 1, &create_info, NULL, &intern->pipeline);
    if (spec_entries) efree(spec_entries);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create compute pipeline");
    }
}

/* Vk\Pipeline::createGraphics(Vk\Device $device, array $config): Vk\Pipeline
 * config keys: layout, renderPass, vertexShader, fragmentShader, vertexEntryPoint,
 *              fragmentEntryPoint, topology, viewportWidth, viewportHeight,
 *              cullMode, frontFace, depthTest, depthWrite, vertexBindings, vertexAttributes,
 *              dynamicStates */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_vk_pipeline_createGraphics, 0, 2, Vk\\Pipeline, 0)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, config, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkPipeline, createGraphics) {
    zval *device_zval;
    HashTable *config;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_ARRAY_HT(config)
    ZEND_PARSE_PARAMETERS_END();

    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));

    /* Extract required objects */
    zval *zlayout = zend_hash_str_find(config, "layout", sizeof("layout") - 1);
    zval *zrenderpass = zend_hash_str_find(config, "renderPass", sizeof("renderPass") - 1);
    zval *zvert_shader = zend_hash_str_find(config, "vertexShader", sizeof("vertexShader") - 1);
    zval *zfrag_shader = zend_hash_str_find(config, "fragmentShader", sizeof("fragmentShader") - 1);

    if (!zlayout || !zrenderpass || !zvert_shader || !zfrag_shader) {
        zend_throw_exception(vk_vulkan_exception_ce,
            "Config must include: layout, renderPass, vertexShader, fragmentShader", 0);
        return;
    }

    vk_pipeline_layout_object *layout = VK_OBJ(vk_pipeline_layout_object, Z_OBJ_P(zlayout));
    vk_render_pass_object *rp = VK_OBJ(vk_render_pass_object, Z_OBJ_P(zrenderpass));
    vk_shader_module_object *vert = VK_OBJ(vk_shader_module_object, Z_OBJ_P(zvert_shader));
    vk_shader_module_object *frag = VK_OBJ(vk_shader_module_object, Z_OBJ_P(zfrag_shader));

    zval *zvert_entry = zend_hash_str_find(config, "vertexEntryPoint", sizeof("vertexEntryPoint") - 1);
    zval *zfrag_entry = zend_hash_str_find(config, "fragmentEntryPoint", sizeof("fragmentEntryPoint") - 1);
    const char *vert_entry = (zvert_entry && Z_TYPE_P(zvert_entry) == IS_STRING) ? Z_STRVAL_P(zvert_entry) : "main";
    const char *frag_entry = (zfrag_entry && Z_TYPE_P(zfrag_entry) == IS_STRING) ? Z_STRVAL_P(zfrag_entry) : "main";

    /* Shader stages */
    VkPipelineShaderStageCreateInfo stages[2] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vert->shader_module,
            .pName = vert_entry,
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = frag->shader_module,
            .pName = frag_entry,
        },
    };

    /* Vertex input — build from config or use empty */
    zval *zvert_bindings = zend_hash_str_find(config, "vertexBindings", sizeof("vertexBindings") - 1);
    zval *zvert_attrs = zend_hash_str_find(config, "vertexAttributes", sizeof("vertexAttributes") - 1);

    uint32_t vb_count = 0;
    VkVertexInputBindingDescription *vb_descs = NULL;
    uint32_t va_count = 0;
    VkVertexInputAttributeDescription *va_descs = NULL;

    if (zvert_bindings && Z_TYPE_P(zvert_bindings) == IS_ARRAY) {
        vb_count = zend_hash_num_elements(Z_ARRVAL_P(zvert_bindings));
        vb_descs = ecalloc(vb_count, sizeof(VkVertexInputBindingDescription));
        zval *zv;
        uint32_t i = 0;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zvert_bindings), zv) {
            if (Z_TYPE_P(zv) != IS_ARRAY) continue;
            HashTable *vb = Z_ARRVAL_P(zv);
            zval *zbinding = zend_hash_str_find(vb, "binding", sizeof("binding") - 1);
            zval *zstride = zend_hash_str_find(vb, "stride", sizeof("stride") - 1);
            zval *zrate = zend_hash_str_find(vb, "inputRate", sizeof("inputRate") - 1);
            vb_descs[i] = (VkVertexInputBindingDescription){
                .binding = zbinding ? (uint32_t)zval_get_long(zbinding) : i,
                .stride = zstride ? (uint32_t)zval_get_long(zstride) : 0,
                .inputRate = zrate ? (VkVertexInputRate)zval_get_long(zrate) : VK_VERTEX_INPUT_RATE_VERTEX,
            };
            i++;
        } ZEND_HASH_FOREACH_END();
        vb_count = i;
    }

    if (zvert_attrs && Z_TYPE_P(zvert_attrs) == IS_ARRAY) {
        va_count = zend_hash_num_elements(Z_ARRVAL_P(zvert_attrs));
        va_descs = ecalloc(va_count, sizeof(VkVertexInputAttributeDescription));
        zval *zv;
        uint32_t i = 0;
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zvert_attrs), zv) {
            if (Z_TYPE_P(zv) != IS_ARRAY) continue;
            HashTable *va = Z_ARRVAL_P(zv);
            zval *zlocation = zend_hash_str_find(va, "location", sizeof("location") - 1);
            zval *zbinding = zend_hash_str_find(va, "binding", sizeof("binding") - 1);
            zval *zformat = zend_hash_str_find(va, "format", sizeof("format") - 1);
            zval *zoffset = zend_hash_str_find(va, "offset", sizeof("offset") - 1);
            va_descs[i] = (VkVertexInputAttributeDescription){
                .location = zlocation ? (uint32_t)zval_get_long(zlocation) : i,
                .binding = zbinding ? (uint32_t)zval_get_long(zbinding) : 0,
                .format = zformat ? (VkFormat)zval_get_long(zformat) : VK_FORMAT_R32G32B32_SFLOAT,
                .offset = zoffset ? (uint32_t)zval_get_long(zoffset) : 0,
            };
            i++;
        } ZEND_HASH_FOREACH_END();
        va_count = i;
    }

    VkPipelineVertexInputStateCreateInfo vertex_input = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = vb_count,
        .pVertexBindingDescriptions = vb_descs,
        .vertexAttributeDescriptionCount = va_count,
        .pVertexAttributeDescriptions = va_descs,
    };

    /* Topology */
    zval *ztopology = zend_hash_str_find(config, "topology", sizeof("topology") - 1);
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = ztopology ? (VkPrimitiveTopology)zval_get_long(ztopology) : VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    /* Dynamic state */
    VkDynamicState default_dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineDynamicStateCreateInfo dynamic_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates = default_dynamic_states,
    };

    /* Viewport (count only, actual values set dynamically) */
    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    /* Rasterization */
    zval *zcull = zend_hash_str_find(config, "cullMode", sizeof("cullMode") - 1);
    zval *zfront = zend_hash_str_find(config, "frontFace", sizeof("frontFace") - 1);
    VkPipelineRasterizationStateCreateInfo rasterization = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = zcull ? (VkCullModeFlags)zval_get_long(zcull) : VK_CULL_MODE_BACK_BIT,
        .frontFace = zfront ? (VkFrontFace)zval_get_long(zfront) : VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
    };

    /* Multisampling */
    zval *zsamples = zend_hash_str_find(config, "samples", sizeof("samples") - 1);
    zval *zsample_shading = zend_hash_str_find(config, "sampleShading", sizeof("sampleShading") - 1);
    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = zsample_shading ? VK_TRUE : VK_FALSE,
        .rasterizationSamples = zsamples ? (VkSampleCountFlagBits)zval_get_long(zsamples) : VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = zsample_shading ? (float)zval_get_double(zsample_shading) : 1.0f,
    };

    /* Depth stencil */
    zval *zdepth_test = zend_hash_str_find(config, "depthTest", sizeof("depthTest") - 1);
    zval *zdepth_write = zend_hash_str_find(config, "depthWrite", sizeof("depthWrite") - 1);
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = zdepth_test ? (VkBool32)zval_is_true(zdepth_test) : VK_FALSE,
        .depthWriteEnable = zdepth_write ? (VkBool32)zval_is_true(zdepth_write) : VK_FALSE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
    };

    /* Color blending — default alpha blend */
    VkPipelineColorBlendAttachmentState blend_attachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
    };

    VkPipelineColorBlendStateCreateInfo color_blending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &blend_attachment,
    };

    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = stages,
        .pVertexInputState = &vertex_input,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterization,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depth_stencil,
        .pColorBlendState = &color_blending,
        .pDynamicState = &dynamic_state,
        .layout = layout->layout,
        .renderPass = rp->render_pass,
        .subpass = 0,
    };

    object_init_ex(return_value, vk_pipeline_ce);
    vk_pipeline_object *intern = VK_OBJ(vk_pipeline_object, Z_OBJ_P(return_value));
    ZVAL_COPY(&intern->device_zval, device_zval);

    /* Pipeline cache */
    VkPipelineCache vk_cache = VK_NULL_HANDLE;
    zval *zcache = zend_hash_str_find(config, "cache", sizeof("cache") - 1);
    if (zcache && Z_TYPE_P(zcache) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zcache), vk_pipeline_cache_ce)) {
        vk_pipeline_cache_object *cache = VK_OBJ(vk_pipeline_cache_object, Z_OBJ_P(zcache));
        vk_cache = cache->pipeline_cache;
    }

    VkResult result = vkCreateGraphicsPipelines(dev->device, vk_cache, 1, &pipeline_info, NULL, &intern->pipeline);

    if (vb_descs) efree(vb_descs);
    if (va_descs) efree(va_descs);

    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create graphics pipeline");
    }
}

static const zend_function_entry vk_pipeline_methods[] = {
    PHP_ME(VkPipeline, createCompute,  arginfo_vk_pipeline_createCompute,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(VkPipeline, createGraphics, arginfo_vk_pipeline_createGraphics, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

void php_vk_pipeline_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Pipeline", vk_pipeline_methods);
    vk_pipeline_ce = zend_register_internal_class(&ce);
    vk_pipeline_ce->create_object = vk_pipeline_create_object;
    vk_pipeline_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_pipeline_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_pipeline_handlers.offset = XtOffsetOf(vk_pipeline_object, std);
    vk_pipeline_handlers.free_obj = vk_pipeline_free_object;
    vk_pipeline_handlers.clone_obj = NULL;
}
