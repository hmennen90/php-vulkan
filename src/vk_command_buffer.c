/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkCommandBuffer                                         |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_command_buffer_ce;
static zend_object_handlers vk_command_buffer_handlers;

static zend_object *vk_command_buffer_create_object(zend_class_entry *ce) {
    vk_command_buffer_object *intern = zend_object_alloc(sizeof(vk_command_buffer_object), ce);
    intern->command_buffer = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->pool_zval);
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_command_buffer_handlers;
    return &intern->std;
}

static void vk_command_buffer_free_object(zend_object *object) {
    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, object);
    /* Command buffers are freed with the pool, no explicit free needed */
    zval_ptr_dtor(&intern->pool_zval);
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\CommandBuffer::begin(int $flags = 0): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_begin, 0, 0, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, begin) {
    zend_long flags = 0;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(flags)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = (VkCommandBufferUsageFlags)flags,
    };

    VkResult result = vkBeginCommandBuffer(intern->command_buffer, &begin_info);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to begin command buffer");
    }
}

/* Vk\CommandBuffer::end(): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_end, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, end) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    VkResult result = vkEndCommandBuffer(intern->command_buffer);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to end command buffer");
    }
}

/* Vk\CommandBuffer::reset(int $flags = 0): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_reset, 0, 0, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, reset) {
    zend_long flags = 0;
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(flags)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    VkResult result = vkResetCommandBuffer(intern->command_buffer, (VkCommandBufferResetFlags)flags);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to reset command buffer");
    }
}

/* Vk\CommandBuffer::bindPipeline(int $bindPoint, Vk\Pipeline $pipeline): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_bindPipeline, 0, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, bindPoint, IS_LONG, 0)
    ZEND_ARG_OBJ_INFO(0, pipeline, Vk\\Pipeline, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, bindPipeline) {
    zend_long bind_point;
    zval *pipeline_zval;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(bind_point)
        Z_PARAM_OBJECT_OF_CLASS(pipeline_zval, vk_pipeline_ce)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vk_pipeline_object *pip = VK_OBJ(vk_pipeline_object, Z_OBJ_P(pipeline_zval));

    vkCmdBindPipeline(intern->command_buffer, (VkPipelineBindPoint)bind_point, pip->pipeline);
}

/* Vk\CommandBuffer::bindDescriptorSets(int $bindPoint, Vk\PipelineLayout $layout,
 *                                       int $firstSet, array<Vk\DescriptorSet> $sets): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_bindDescriptorSets, 0, 4, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, bindPoint, IS_LONG, 0)
    ZEND_ARG_OBJ_INFO(0, layout, Vk\\PipelineLayout, 0)
    ZEND_ARG_TYPE_INFO(0, firstSet, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, sets, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, bindDescriptorSets) {
    zend_long bind_point, first_set;
    zval *layout_zval;
    HashTable *sets;

    ZEND_PARSE_PARAMETERS_START(4, 4)
        Z_PARAM_LONG(bind_point)
        Z_PARAM_OBJECT_OF_CLASS(layout_zval, vk_pipeline_layout_ce)
        Z_PARAM_LONG(first_set)
        Z_PARAM_ARRAY_HT(sets)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vk_pipeline_layout_object *layout = VK_OBJ(vk_pipeline_layout_object, Z_OBJ_P(layout_zval));

    uint32_t set_count = zend_hash_num_elements(sets);
    VkDescriptorSet *ds = ecalloc(set_count, sizeof(VkDescriptorSet));
    zval *zv;
    uint32_t idx = 0;
    ZEND_HASH_FOREACH_VAL(sets, zv) {
        if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), vk_descriptor_set_ce)) {
            vk_descriptor_set_object *s = VK_OBJ(vk_descriptor_set_object, Z_OBJ_P(zv));
            ds[idx++] = s->set;
        }
    } ZEND_HASH_FOREACH_END();

    vkCmdBindDescriptorSets(intern->command_buffer, (VkPipelineBindPoint)bind_point,
        layout->layout, (uint32_t)first_set, idx, ds, 0, NULL);

    efree(ds);
}

/* Vk\CommandBuffer::dispatch(int $x, int $y = 1, int $z = 1): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_dispatch, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, y, IS_LONG, 0, "1")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, z, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, dispatch) {
    zend_long x, y = 1, z = 1;

    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_LONG(x)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(y)
        Z_PARAM_LONG(z)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vkCmdDispatch(intern->command_buffer, (uint32_t)x, (uint32_t)y, (uint32_t)z);
}

/* Vk\CommandBuffer::draw(int $vertexCount, int $instanceCount = 1,
 *                        int $firstVertex = 0, int $firstInstance = 0): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_draw, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, vertexCount, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, instanceCount, IS_LONG, 0, "1")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, firstVertex, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, firstInstance, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, draw) {
    zend_long vertex_count, instance_count = 1, first_vertex = 0, first_instance = 0;

    ZEND_PARSE_PARAMETERS_START(1, 4)
        Z_PARAM_LONG(vertex_count)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(instance_count)
        Z_PARAM_LONG(first_vertex)
        Z_PARAM_LONG(first_instance)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vkCmdDraw(intern->command_buffer, (uint32_t)vertex_count, (uint32_t)instance_count,
        (uint32_t)first_vertex, (uint32_t)first_instance);
}

/* Vk\CommandBuffer::drawIndexed(int $indexCount, int $instanceCount = 1,
 *                                int $firstIndex = 0, int $vertexOffset = 0,
 *                                int $firstInstance = 0): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_drawIndexed, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, indexCount, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, instanceCount, IS_LONG, 0, "1")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, firstIndex, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, vertexOffset, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, firstInstance, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, drawIndexed) {
    zend_long index_count, instance_count = 1, first_index = 0, vertex_offset = 0, first_instance = 0;

    ZEND_PARSE_PARAMETERS_START(1, 5)
        Z_PARAM_LONG(index_count)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(instance_count)
        Z_PARAM_LONG(first_index)
        Z_PARAM_LONG(vertex_offset)
        Z_PARAM_LONG(first_instance)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vkCmdDrawIndexed(intern->command_buffer, (uint32_t)index_count, (uint32_t)instance_count,
        (uint32_t)first_index, (int32_t)vertex_offset, (uint32_t)first_instance);
}

/* Vk\CommandBuffer::bindVertexBuffers(int $firstBinding, array<Vk\Buffer> $buffers,
 *                                      array<int> $offsets = []): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_bindVertexBuffers, 0, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, firstBinding, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, buffers, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, offsets, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, bindVertexBuffers) {
    zend_long first_binding;
    HashTable *buffers;
    HashTable *offsets = NULL;

    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_LONG(first_binding)
        Z_PARAM_ARRAY_HT(buffers)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY_HT(offsets)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));

    uint32_t count = zend_hash_num_elements(buffers);
    VkBuffer *buf_handles = ecalloc(count, sizeof(VkBuffer));
    VkDeviceSize *buf_offsets = ecalloc(count, sizeof(VkDeviceSize));

    zval *zv;
    uint32_t idx = 0;
    ZEND_HASH_FOREACH_VAL(buffers, zv) {
        if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), vk_buffer_ce)) {
            vk_buffer_object *b = VK_OBJ(vk_buffer_object, Z_OBJ_P(zv));
            buf_handles[idx] = b->buffer;
            buf_offsets[idx] = 0;
            idx++;
        }
    } ZEND_HASH_FOREACH_END();

    if (offsets) {
        uint32_t oi = 0;
        ZEND_HASH_FOREACH_VAL(offsets, zv) {
            if (oi < idx) {
                buf_offsets[oi++] = (VkDeviceSize)zval_get_long(zv);
            }
        } ZEND_HASH_FOREACH_END();
    }

    vkCmdBindVertexBuffers(intern->command_buffer, (uint32_t)first_binding, idx, buf_handles, buf_offsets);

    efree(buf_handles);
    efree(buf_offsets);
}

/* Vk\CommandBuffer::bindIndexBuffer(Vk\Buffer $buffer, int $offset = 0, int $indexType = 1): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_bindIndexBuffer, 0, 1, IS_VOID, 0)
    ZEND_ARG_OBJ_INFO(0, buffer, Vk\\Buffer, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, indexType, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, bindIndexBuffer) {
    zval *buffer_zval;
    zend_long offset = 0;
    zend_long index_type = VK_INDEX_TYPE_UINT32;

    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_OBJECT_OF_CLASS(buffer_zval, vk_buffer_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(offset)
        Z_PARAM_LONG(index_type)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vk_buffer_object *buf = VK_OBJ(vk_buffer_object, Z_OBJ_P(buffer_zval));

    vkCmdBindIndexBuffer(intern->command_buffer, buf->buffer, (VkDeviceSize)offset, (VkIndexType)index_type);
}

/* Vk\CommandBuffer::copyBuffer(Vk\Buffer $src, Vk\Buffer $dst, int $size,
 *                               int $srcOffset = 0, int $dstOffset = 0): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_copyBuffer, 0, 3, IS_VOID, 0)
    ZEND_ARG_OBJ_INFO(0, src, Vk\\Buffer, 0)
    ZEND_ARG_OBJ_INFO(0, dst, Vk\\Buffer, 0)
    ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, srcOffset, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, dstOffset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, copyBuffer) {
    zval *src_zval, *dst_zval;
    zend_long size, src_offset = 0, dst_offset = 0;

    ZEND_PARSE_PARAMETERS_START(3, 5)
        Z_PARAM_OBJECT_OF_CLASS(src_zval, vk_buffer_ce)
        Z_PARAM_OBJECT_OF_CLASS(dst_zval, vk_buffer_ce)
        Z_PARAM_LONG(size)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(src_offset)
        Z_PARAM_LONG(dst_offset)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vk_buffer_object *src = VK_OBJ(vk_buffer_object, Z_OBJ_P(src_zval));
    vk_buffer_object *dst = VK_OBJ(vk_buffer_object, Z_OBJ_P(dst_zval));

    VkBufferCopy region = {
        .srcOffset = (VkDeviceSize)src_offset,
        .dstOffset = (VkDeviceSize)dst_offset,
        .size = (VkDeviceSize)size,
    };

    vkCmdCopyBuffer(intern->command_buffer, src->buffer, dst->buffer, 1, &region);
}

/* Vk\CommandBuffer::beginRenderPass(Vk\RenderPass $renderPass, Vk\Framebuffer $framebuffer,
 *                                    int $x, int $y, int $width, int $height,
 *                                    array $clearValues = []): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_beginRenderPass, 0, 6, IS_VOID, 0)
    ZEND_ARG_OBJ_INFO(0, renderPass, Vk\\RenderPass, 0)
    ZEND_ARG_OBJ_INFO(0, framebuffer, Vk\\Framebuffer, 0)
    ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, clearValues, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, beginRenderPass) {
    zval *rp_zval, *fb_zval;
    zend_long x, y, width, height;
    HashTable *clear_values = NULL;

    ZEND_PARSE_PARAMETERS_START(6, 7)
        Z_PARAM_OBJECT_OF_CLASS(rp_zval, vk_render_pass_ce)
        Z_PARAM_OBJECT_OF_CLASS(fb_zval, vk_framebuffer_ce)
        Z_PARAM_LONG(x)
        Z_PARAM_LONG(y)
        Z_PARAM_LONG(width)
        Z_PARAM_LONG(height)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY_HT(clear_values)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vk_render_pass_object *rp = VK_OBJ(vk_render_pass_object, Z_OBJ_P(rp_zval));
    vk_framebuffer_object *fb = VK_OBJ(vk_framebuffer_object, Z_OBJ_P(fb_zval));

    uint32_t cv_count = clear_values ? zend_hash_num_elements(clear_values) : 0;
    VkClearValue *cvs = NULL;
    if (cv_count > 0) {
        cvs = ecalloc(cv_count, sizeof(VkClearValue));
        zval *zv;
        uint32_t idx = 0;
        ZEND_HASH_FOREACH_VAL(clear_values, zv) {
            if (Z_TYPE_P(zv) == IS_ARRAY) {
                HashTable *cv_ht = Z_ARRVAL_P(zv);
                /* Color: [r, g, b, a] or depth: [depth, stencil] */
                uint32_t cv_elem_count = zend_hash_num_elements(cv_ht);
                if (cv_elem_count == 4) {
                    zval *r = zend_hash_index_find(cv_ht, 0);
                    zval *g = zend_hash_index_find(cv_ht, 1);
                    zval *b = zend_hash_index_find(cv_ht, 2);
                    zval *a = zend_hash_index_find(cv_ht, 3);
                    cvs[idx].color.float32[0] = r ? (float)zval_get_double(r) : 0.0f;
                    cvs[idx].color.float32[1] = g ? (float)zval_get_double(g) : 0.0f;
                    cvs[idx].color.float32[2] = b ? (float)zval_get_double(b) : 0.0f;
                    cvs[idx].color.float32[3] = a ? (float)zval_get_double(a) : 1.0f;
                } else if (cv_elem_count == 2) {
                    zval *d = zend_hash_index_find(cv_ht, 0);
                    zval *s = zend_hash_index_find(cv_ht, 1);
                    cvs[idx].depthStencil.depth = d ? (float)zval_get_double(d) : 1.0f;
                    cvs[idx].depthStencil.stencil = s ? (uint32_t)zval_get_long(s) : 0;
                }
            }
            idx++;
        } ZEND_HASH_FOREACH_END();
    }

    VkRenderPassBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = rp->render_pass,
        .framebuffer = fb->framebuffer,
        .renderArea = {
            .offset = {(int32_t)x, (int32_t)y},
            .extent = {(uint32_t)width, (uint32_t)height},
        },
        .clearValueCount = cv_count,
        .pClearValues = cvs,
    };

    vkCmdBeginRenderPass(intern->command_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

    if (cvs) efree(cvs);
}

/* Vk\CommandBuffer::endRenderPass(): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_endRenderPass, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, endRenderPass) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vkCmdEndRenderPass(intern->command_buffer);
}

/* Vk\CommandBuffer::setViewport(float $x, float $y, float $width, float $height,
 *                                float $minDepth = 0.0, float $maxDepth = 1.0): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_setViewport, 0, 4, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, x, IS_DOUBLE, 0)
    ZEND_ARG_TYPE_INFO(0, y, IS_DOUBLE, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_DOUBLE, 0)
    ZEND_ARG_TYPE_INFO(0, height, IS_DOUBLE, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, minDepth, IS_DOUBLE, 0, "0.0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, maxDepth, IS_DOUBLE, 0, "1.0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, setViewport) {
    double x, y, w, h, min_depth = 0.0, max_depth = 1.0;

    ZEND_PARSE_PARAMETERS_START(4, 6)
        Z_PARAM_DOUBLE(x)
        Z_PARAM_DOUBLE(y)
        Z_PARAM_DOUBLE(w)
        Z_PARAM_DOUBLE(h)
        Z_PARAM_OPTIONAL
        Z_PARAM_DOUBLE(min_depth)
        Z_PARAM_DOUBLE(max_depth)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));

    VkViewport viewport = {
        .x = (float)x, .y = (float)y,
        .width = (float)w, .height = (float)h,
        .minDepth = (float)min_depth, .maxDepth = (float)max_depth,
    };

    vkCmdSetViewport(intern->command_buffer, 0, 1, &viewport);
}

/* Vk\CommandBuffer::setScissor(int $x, int $y, int $width, int $height): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_setScissor, 0, 4, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, setScissor) {
    zend_long x, y, w, h;

    ZEND_PARSE_PARAMETERS_START(4, 4)
        Z_PARAM_LONG(x)
        Z_PARAM_LONG(y)
        Z_PARAM_LONG(w)
        Z_PARAM_LONG(h)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));

    VkRect2D scissor = {
        .offset = {(int32_t)x, (int32_t)y},
        .extent = {(uint32_t)w, (uint32_t)h},
    };

    vkCmdSetScissor(intern->command_buffer, 0, 1, &scissor);
}

/* Vk\CommandBuffer::pipelineBarrier(int $srcStage, int $dstStage,
 *                                    int $srcAccess, int $dstAccess): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_pipelineBarrier, 0, 4, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, srcStage, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, dstStage, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, srcAccess, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, dstAccess, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, pipelineBarrier) {
    zend_long src_stage, dst_stage, src_access, dst_access;

    ZEND_PARSE_PARAMETERS_START(4, 4)
        Z_PARAM_LONG(src_stage)
        Z_PARAM_LONG(dst_stage)
        Z_PARAM_LONG(src_access)
        Z_PARAM_LONG(dst_access)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));

    VkMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
        .srcAccessMask = (VkAccessFlags)src_access,
        .dstAccessMask = (VkAccessFlags)dst_access,
    };

    vkCmdPipelineBarrier(intern->command_buffer,
        (VkPipelineStageFlags)src_stage,
        (VkPipelineStageFlags)dst_stage,
        0, 1, &barrier, 0, NULL, 0, NULL);
}

/* Vk\CommandBuffer::pushConstants(Vk\PipelineLayout $layout, int $stageFlags,
 *                                  int $offset, string $data): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_pushConstants, 0, 4, IS_VOID, 0)
    ZEND_ARG_OBJ_INFO(0, layout, Vk\\PipelineLayout, 0)
    ZEND_ARG_TYPE_INFO(0, stageFlags, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, pushConstants) {
    zval *layout_zval;
    zend_long stage_flags, offset;
    zend_string *data;

    ZEND_PARSE_PARAMETERS_START(4, 4)
        Z_PARAM_OBJECT_OF_CLASS(layout_zval, vk_pipeline_layout_ce)
        Z_PARAM_LONG(stage_flags)
        Z_PARAM_LONG(offset)
        Z_PARAM_STR(data)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vk_pipeline_layout_object *layout = VK_OBJ(vk_pipeline_layout_object, Z_OBJ_P(layout_zval));

    vkCmdPushConstants(intern->command_buffer, layout->layout,
        (VkShaderStageFlags)stage_flags, (uint32_t)offset, (uint32_t)ZSTR_LEN(data), ZSTR_VAL(data));
}

/* Vk\CommandBuffer::copyImageToBuffer(Vk\Image $image, int $imageLayout,
 *     Vk\Buffer $buffer, int $width, int $height,
 *     int $bufferOffset = 0, int $imageOffsetX = 0, int $imageOffsetY = 0,
 *     int $aspectMask = 1): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_copyImageToBuffer, 0, 5, IS_VOID, 0)
    ZEND_ARG_OBJ_INFO(0, image, Vk\\Image, 0)
    ZEND_ARG_TYPE_INFO(0, imageLayout, IS_LONG, 0)
    ZEND_ARG_OBJ_INFO(0, buffer, Vk\\Buffer, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, bufferOffset, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, imageOffsetX, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, imageOffsetY, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, aspectMask, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, copyImageToBuffer) {
    zval *image_zval, *buffer_zval;
    zend_long image_layout, width, height;
    zend_long buffer_offset = 0, image_offset_x = 0, image_offset_y = 0;
    zend_long aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;

    ZEND_PARSE_PARAMETERS_START(5, 9)
        Z_PARAM_OBJECT_OF_CLASS(image_zval, vk_image_ce)
        Z_PARAM_LONG(image_layout)
        Z_PARAM_OBJECT_OF_CLASS(buffer_zval, vk_buffer_ce)
        Z_PARAM_LONG(width)
        Z_PARAM_LONG(height)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(buffer_offset)
        Z_PARAM_LONG(image_offset_x)
        Z_PARAM_LONG(image_offset_y)
        Z_PARAM_LONG(aspect_mask)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vk_image_object *img = VK_OBJ(vk_image_object, Z_OBJ_P(image_zval));
    vk_buffer_object *buf = VK_OBJ(vk_buffer_object, Z_OBJ_P(buffer_zval));

    VkBufferImageCopy region = {
        .bufferOffset = (VkDeviceSize)buffer_offset,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = (VkImageAspectFlags)aspect_mask,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .imageOffset = {(int32_t)image_offset_x, (int32_t)image_offset_y, 0},
        .imageExtent = {(uint32_t)width, (uint32_t)height, 1},
    };

    vkCmdCopyImageToBuffer(intern->command_buffer, img->image,
        (VkImageLayout)image_layout, buf->buffer, 1, &region);
}

/* Vk\CommandBuffer::copyBufferToImage(Vk\Buffer $buffer, Vk\Image $image,
 *     int $imageLayout, int $width, int $height,
 *     int $bufferOffset = 0, int $imageOffsetX = 0, int $imageOffsetY = 0,
 *     int $aspectMask = 1): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_copyBufferToImage, 0, 5, IS_VOID, 0)
    ZEND_ARG_OBJ_INFO(0, buffer, Vk\\Buffer, 0)
    ZEND_ARG_OBJ_INFO(0, image, Vk\\Image, 0)
    ZEND_ARG_TYPE_INFO(0, imageLayout, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, bufferOffset, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, imageOffsetX, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, imageOffsetY, IS_LONG, 0, "0")
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, aspectMask, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, copyBufferToImage) {
    zval *buffer_zval, *image_zval;
    zend_long image_layout, width, height;
    zend_long buffer_offset = 0, image_offset_x = 0, image_offset_y = 0;
    zend_long aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;

    ZEND_PARSE_PARAMETERS_START(5, 9)
        Z_PARAM_OBJECT_OF_CLASS(buffer_zval, vk_buffer_ce)
        Z_PARAM_OBJECT_OF_CLASS(image_zval, vk_image_ce)
        Z_PARAM_LONG(image_layout)
        Z_PARAM_LONG(width)
        Z_PARAM_LONG(height)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(buffer_offset)
        Z_PARAM_LONG(image_offset_x)
        Z_PARAM_LONG(image_offset_y)
        Z_PARAM_LONG(aspect_mask)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vk_buffer_object *buf = VK_OBJ(vk_buffer_object, Z_OBJ_P(buffer_zval));
    vk_image_object *img = VK_OBJ(vk_image_object, Z_OBJ_P(image_zval));

    VkBufferImageCopy region = {
        .bufferOffset = (VkDeviceSize)buffer_offset,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = (VkImageAspectFlags)aspect_mask,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .imageOffset = {(int32_t)image_offset_x, (int32_t)image_offset_y, 0},
        .imageExtent = {(uint32_t)width, (uint32_t)height, 1},
    };

    vkCmdCopyBufferToImage(intern->command_buffer, buf->buffer, img->image,
        (VkImageLayout)image_layout, 1, &region);
}

/* Vk\CommandBuffer::imageMemoryBarrier(Vk\Image $image, int $oldLayout, int $newLayout,
 *     int $srcAccessMask, int $dstAccessMask, int $srcStage, int $dstStage,
 *     int $aspectMask = 1): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_cb_imageMemoryBarrier, 0, 7, IS_VOID, 0)
    ZEND_ARG_OBJ_INFO(0, image, Vk\\Image, 0)
    ZEND_ARG_TYPE_INFO(0, oldLayout, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, newLayout, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, srcAccessMask, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, dstAccessMask, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, srcStage, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, dstStage, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, aspectMask, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

PHP_METHOD(VkCommandBuffer, imageMemoryBarrier) {
    zval *image_zval;
    zend_long old_layout, new_layout, src_access, dst_access, src_stage, dst_stage;
    zend_long aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;

    ZEND_PARSE_PARAMETERS_START(7, 8)
        Z_PARAM_OBJECT_OF_CLASS(image_zval, vk_image_ce)
        Z_PARAM_LONG(old_layout)
        Z_PARAM_LONG(new_layout)
        Z_PARAM_LONG(src_access)
        Z_PARAM_LONG(dst_access)
        Z_PARAM_LONG(src_stage)
        Z_PARAM_LONG(dst_stage)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(aspect_mask)
    ZEND_PARSE_PARAMETERS_END();

    vk_command_buffer_object *intern = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(ZEND_THIS));
    vk_image_object *img = VK_OBJ(vk_image_object, Z_OBJ_P(image_zval));

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = (VkImageLayout)old_layout,
        .newLayout = (VkImageLayout)new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = img->image,
        .subresourceRange = {
            .aspectMask = (VkImageAspectFlags)aspect_mask,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .srcAccessMask = (VkAccessFlags)src_access,
        .dstAccessMask = (VkAccessFlags)dst_access,
    };

    vkCmdPipelineBarrier(intern->command_buffer,
        (VkPipelineStageFlags)src_stage,
        (VkPipelineStageFlags)dst_stage,
        0, 0, NULL, 0, NULL, 1, &barrier);
}

static const zend_function_entry vk_command_buffer_methods[] = {
    PHP_ME(VkCommandBuffer, begin,              arginfo_vk_cb_begin,              ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, end,                arginfo_vk_cb_end,                ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, reset,              arginfo_vk_cb_reset,              ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, bindPipeline,       arginfo_vk_cb_bindPipeline,       ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, bindDescriptorSets, arginfo_vk_cb_bindDescriptorSets, ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, dispatch,           arginfo_vk_cb_dispatch,           ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, draw,               arginfo_vk_cb_draw,              ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, drawIndexed,        arginfo_vk_cb_drawIndexed,       ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, bindVertexBuffers,  arginfo_vk_cb_bindVertexBuffers,  ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, bindIndexBuffer,    arginfo_vk_cb_bindIndexBuffer,    ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, copyBuffer,         arginfo_vk_cb_copyBuffer,         ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, beginRenderPass,    arginfo_vk_cb_beginRenderPass,    ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, endRenderPass,      arginfo_vk_cb_endRenderPass,      ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, setViewport,        arginfo_vk_cb_setViewport,        ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, setScissor,         arginfo_vk_cb_setScissor,         ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, pipelineBarrier,    arginfo_vk_cb_pipelineBarrier,    ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, pushConstants,      arginfo_vk_cb_pushConstants,      ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, copyImageToBuffer,  arginfo_vk_cb_copyImageToBuffer,  ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, copyBufferToImage,  arginfo_vk_cb_copyBufferToImage,  ZEND_ACC_PUBLIC)
    PHP_ME(VkCommandBuffer, imageMemoryBarrier, arginfo_vk_cb_imageMemoryBarrier, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_command_buffer_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "CommandBuffer", vk_command_buffer_methods);
    vk_command_buffer_ce = zend_register_internal_class(&ce);
    vk_command_buffer_ce->create_object = vk_command_buffer_create_object;
    vk_command_buffer_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_command_buffer_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_command_buffer_handlers.offset = XtOffsetOf(vk_command_buffer_object, std);
    vk_command_buffer_handlers.free_obj = vk_command_buffer_free_object;
    vk_command_buffer_handlers.clone_obj = NULL;
}
