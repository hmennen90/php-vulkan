/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkRenderPass                                            |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_render_pass_ce;
static zend_object_handlers vk_render_pass_handlers;

static zend_object *vk_render_pass_create_object(zend_class_entry *ce) {
    vk_render_pass_object *intern = zend_object_alloc(sizeof(vk_render_pass_object), ce);
    intern->render_pass = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_render_pass_handlers;
    return &intern->std;
}

static void vk_render_pass_free_object(zend_object *object) {
    vk_render_pass_object *intern = VK_OBJ(vk_render_pass_object, object);
    if (intern->render_pass != VK_NULL_HANDLE && !Z_ISUNDEF(intern->device_zval)) {
        vk_device_object *dev = VK_OBJ_FROM_ZVAL(vk_device_object, &intern->device_zval);
        if (dev->device != VK_NULL_HANDLE) {
            vkDestroyRenderPass(dev->device, intern->render_pass, NULL);
        }
    }
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\RenderPass::__construct(Vk\Device $device, array $attachments, array $subpasses, array $dependencies = [])
 * attachment: [format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout]
 * subpass: [bindPoint, colorAttachments => [[attachment, layout]], depthAttachment => [attachment, layout]]
 * dependency: [srcSubpass, dstSubpass, srcStageMask, dstStageMask, srcAccessMask, dstAccessMask] */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_rp___construct, 0, 0, 3)
    ZEND_ARG_OBJ_INFO(0, device, Vk\\Device, 0)
    ZEND_ARG_TYPE_INFO(0, attachments, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, subpasses, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, dependencies, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkRenderPass, __construct) {
    zval *device_zval;
    HashTable *attachments, *subpasses;
    HashTable *dependencies = NULL;

    ZEND_PARSE_PARAMETERS_START(3, 4)
        Z_PARAM_OBJECT_OF_CLASS(device_zval, vk_device_ce)
        Z_PARAM_ARRAY_HT(attachments)
        Z_PARAM_ARRAY_HT(subpasses)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY_HT(dependencies)
    ZEND_PARSE_PARAMETERS_END();

    vk_render_pass_object *intern = VK_OBJ(vk_render_pass_object, Z_OBJ_P(ZEND_THIS));
    vk_device_object *dev = VK_OBJ(vk_device_object, Z_OBJ_P(device_zval));
    ZVAL_COPY(&intern->device_zval, device_zval);

    /* Attachments */
    uint32_t att_count = zend_hash_num_elements(attachments);
    VkAttachmentDescription *att_descs = ecalloc(att_count, sizeof(VkAttachmentDescription));
    zval *zv;
    uint32_t idx = 0;
    ZEND_HASH_FOREACH_VAL(attachments, zv) {
        if (Z_TYPE_P(zv) != IS_ARRAY) continue;
        HashTable *a = Z_ARRVAL_P(zv);

        zval *zformat = zend_hash_str_find(a, "format", sizeof("format") - 1);
        zval *zsamples = zend_hash_str_find(a, "samples", sizeof("samples") - 1);
        zval *zload = zend_hash_str_find(a, "loadOp", sizeof("loadOp") - 1);
        zval *zstore = zend_hash_str_find(a, "storeOp", sizeof("storeOp") - 1);
        zval *zsload = zend_hash_str_find(a, "stencilLoadOp", sizeof("stencilLoadOp") - 1);
        zval *zsstore = zend_hash_str_find(a, "stencilStoreOp", sizeof("stencilStoreOp") - 1);
        zval *zinitial = zend_hash_str_find(a, "initialLayout", sizeof("initialLayout") - 1);
        zval *zfinal = zend_hash_str_find(a, "finalLayout", sizeof("finalLayout") - 1);

        att_descs[idx] = (VkAttachmentDescription){
            .format = zformat ? (VkFormat)zval_get_long(zformat) : VK_FORMAT_B8G8R8A8_SRGB,
            .samples = zsamples ? (VkSampleCountFlagBits)zval_get_long(zsamples) : VK_SAMPLE_COUNT_1_BIT,
            .loadOp = zload ? (VkAttachmentLoadOp)zval_get_long(zload) : VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = zstore ? (VkAttachmentStoreOp)zval_get_long(zstore) : VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = zsload ? (VkAttachmentLoadOp)zval_get_long(zsload) : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = zsstore ? (VkAttachmentStoreOp)zval_get_long(zsstore) : VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = zinitial ? (VkImageLayout)zval_get_long(zinitial) : VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = zfinal ? (VkImageLayout)zval_get_long(zfinal) : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };
        idx++;
    } ZEND_HASH_FOREACH_END();
    att_count = idx;

    /* Subpasses — simplified: support one color attachment ref per subpass */
    uint32_t sp_count = zend_hash_num_elements(subpasses);

    /* Allocate arrays for color attachment refs */
    VkAttachmentReference **color_refs = ecalloc(sp_count, sizeof(VkAttachmentReference *));
    uint32_t *color_ref_counts = ecalloc(sp_count, sizeof(uint32_t));
    VkAttachmentReference *depth_refs = ecalloc(sp_count, sizeof(VkAttachmentReference));
    zend_bool *has_depth = ecalloc(sp_count, sizeof(zend_bool));
    VkSubpassDescription *sp_descs = ecalloc(sp_count, sizeof(VkSubpassDescription));

    idx = 0;
    ZEND_HASH_FOREACH_VAL(subpasses, zv) {
        if (Z_TYPE_P(zv) != IS_ARRAY) continue;
        HashTable *sp = Z_ARRVAL_P(zv);

        zval *zbind = zend_hash_str_find(sp, "bindPoint", sizeof("bindPoint") - 1);
        zval *zcolors = zend_hash_str_find(sp, "colorAttachments", sizeof("colorAttachments") - 1);
        zval *zdepth = zend_hash_str_find(sp, "depthAttachment", sizeof("depthAttachment") - 1);

        /* Color attachments */
        if (zcolors && Z_TYPE_P(zcolors) == IS_ARRAY) {
            uint32_t cc = zend_hash_num_elements(Z_ARRVAL_P(zcolors));
            color_refs[idx] = ecalloc(cc, sizeof(VkAttachmentReference));
            color_ref_counts[idx] = cc;
            zval *cr;
            uint32_t ci = 0;
            ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zcolors), cr) {
                if (Z_TYPE_P(cr) == IS_ARRAY) {
                    zval *ca = zend_hash_str_find(Z_ARRVAL_P(cr), "attachment", sizeof("attachment") - 1);
                    zval *cl = zend_hash_str_find(Z_ARRVAL_P(cr), "layout", sizeof("layout") - 1);
                    color_refs[idx][ci] = (VkAttachmentReference){
                        .attachment = ca ? (uint32_t)zval_get_long(ca) : 0,
                        .layout = cl ? (VkImageLayout)zval_get_long(cl) : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    };
                    ci++;
                }
            } ZEND_HASH_FOREACH_END();
            color_ref_counts[idx] = ci;
        }

        /* Depth attachment */
        if (zdepth && Z_TYPE_P(zdepth) == IS_ARRAY) {
            zval *da = zend_hash_str_find(Z_ARRVAL_P(zdepth), "attachment", sizeof("attachment") - 1);
            zval *dl = zend_hash_str_find(Z_ARRVAL_P(zdepth), "layout", sizeof("layout") - 1);
            depth_refs[idx] = (VkAttachmentReference){
                .attachment = da ? (uint32_t)zval_get_long(da) : 0,
                .layout = dl ? (VkImageLayout)zval_get_long(dl) : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            };
            has_depth[idx] = 1;
        }

        sp_descs[idx] = (VkSubpassDescription){
            .pipelineBindPoint = zbind ? (VkPipelineBindPoint)zval_get_long(zbind) : VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = color_ref_counts[idx],
            .pColorAttachments = color_refs[idx],
            .pDepthStencilAttachment = has_depth[idx] ? &depth_refs[idx] : NULL,
        };
        idx++;
    } ZEND_HASH_FOREACH_END();
    sp_count = idx;

    /* Dependencies */
    uint32_t dep_count = dependencies ? zend_hash_num_elements(dependencies) : 0;
    VkSubpassDependency *dep_descs = NULL;
    if (dep_count > 0) {
        dep_descs = ecalloc(dep_count, sizeof(VkSubpassDependency));
        idx = 0;
        ZEND_HASH_FOREACH_VAL(dependencies, zv) {
            if (Z_TYPE_P(zv) != IS_ARRAY) continue;
            HashTable *d = Z_ARRVAL_P(zv);
            zval *zsrc_sp = zend_hash_str_find(d, "srcSubpass", sizeof("srcSubpass") - 1);
            zval *zdst_sp = zend_hash_str_find(d, "dstSubpass", sizeof("dstSubpass") - 1);
            zval *zsrc_stage = zend_hash_str_find(d, "srcStageMask", sizeof("srcStageMask") - 1);
            zval *zdst_stage = zend_hash_str_find(d, "dstStageMask", sizeof("dstStageMask") - 1);
            zval *zsrc_access = zend_hash_str_find(d, "srcAccessMask", sizeof("srcAccessMask") - 1);
            zval *zdst_access = zend_hash_str_find(d, "dstAccessMask", sizeof("dstAccessMask") - 1);

            dep_descs[idx] = (VkSubpassDependency){
                .srcSubpass = zsrc_sp ? (uint32_t)zval_get_long(zsrc_sp) : VK_SUBPASS_EXTERNAL,
                .dstSubpass = zdst_sp ? (uint32_t)zval_get_long(zdst_sp) : 0,
                .srcStageMask = zsrc_stage ? (VkPipelineStageFlags)zval_get_long(zsrc_stage) : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = zdst_stage ? (VkPipelineStageFlags)zval_get_long(zdst_stage) : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = zsrc_access ? (VkAccessFlags)zval_get_long(zsrc_access) : 0,
                .dstAccessMask = zdst_access ? (VkAccessFlags)zval_get_long(zdst_access) : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            };
            idx++;
        } ZEND_HASH_FOREACH_END();
        dep_count = idx;
    }

    VkRenderPassCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = att_count,
        .pAttachments = att_descs,
        .subpassCount = sp_count,
        .pSubpasses = sp_descs,
        .dependencyCount = dep_count,
        .pDependencies = dep_descs,
    };

    VkResult result = vkCreateRenderPass(dev->device, &create_info, NULL, &intern->render_pass);

    /* Cleanup */
    for (uint32_t i = 0; i < sp_count; i++) {
        if (color_refs[i]) efree(color_refs[i]);
    }
    efree(color_refs);
    efree(color_ref_counts);
    efree(depth_refs);
    efree(has_depth);
    efree(sp_descs);
    efree(att_descs);
    if (dep_descs) efree(dep_descs);

    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create render pass");
    }
}

static const zend_function_entry vk_render_pass_methods[] = {
    PHP_ME(VkRenderPass, __construct, arginfo_vk_rp___construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_render_pass_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "RenderPass", vk_render_pass_methods);
    vk_render_pass_ce = zend_register_internal_class(&ce);
    vk_render_pass_ce->create_object = vk_render_pass_create_object;
    vk_render_pass_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_render_pass_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_render_pass_handlers.offset = XtOffsetOf(vk_render_pass_object, std);
    vk_render_pass_handlers.free_obj = vk_render_pass_free_object;
    vk_render_pass_handlers.clone_obj = NULL;
}
