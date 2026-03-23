/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — Vulkan Constants                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

/* Register all Vulkan constants under the Vk namespace.
 * Using class constants on a Vk\Vk abstract class for clean namespacing. */

static zend_class_entry *vk_constants_ce;

static const zend_function_entry vk_constants_methods[] = {
    PHP_FE_END
};

void php_vk_enums_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Vk", vk_constants_methods);
    vk_constants_ce = zend_register_internal_class(&ce);
    vk_constants_ce->ce_flags |= ZEND_ACC_ABSTRACT | ZEND_ACC_FINAL;

    /* ---- VkFormat ---- */
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_UNDEFINED", sizeof("FORMAT_UNDEFINED") - 1, VK_FORMAT_UNDEFINED);
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_R8_UNORM", sizeof("FORMAT_R8_UNORM") - 1, VK_FORMAT_R8_UNORM);
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_R8G8B8A8_UNORM", sizeof("FORMAT_R8G8B8A8_UNORM") - 1, VK_FORMAT_R8G8B8A8_UNORM);
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_R8G8B8A8_SRGB", sizeof("FORMAT_R8G8B8A8_SRGB") - 1, VK_FORMAT_R8G8B8A8_SRGB);
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_B8G8R8A8_UNORM", sizeof("FORMAT_B8G8R8A8_UNORM") - 1, VK_FORMAT_B8G8R8A8_UNORM);
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_B8G8R8A8_SRGB", sizeof("FORMAT_B8G8R8A8_SRGB") - 1, VK_FORMAT_B8G8R8A8_SRGB);
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_R32_SFLOAT", sizeof("FORMAT_R32_SFLOAT") - 1, VK_FORMAT_R32_SFLOAT);
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_R32G32_SFLOAT", sizeof("FORMAT_R32G32_SFLOAT") - 1, VK_FORMAT_R32G32_SFLOAT);
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_R32G32B32_SFLOAT", sizeof("FORMAT_R32G32B32_SFLOAT") - 1, VK_FORMAT_R32G32B32_SFLOAT);
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_R32G32B32A32_SFLOAT", sizeof("FORMAT_R32G32B32A32_SFLOAT") - 1, VK_FORMAT_R32G32B32A32_SFLOAT);
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_D32_SFLOAT", sizeof("FORMAT_D32_SFLOAT") - 1, VK_FORMAT_D32_SFLOAT);
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_D24_UNORM_S8_UINT", sizeof("FORMAT_D24_UNORM_S8_UINT") - 1, VK_FORMAT_D24_UNORM_S8_UINT);
    zend_declare_class_constant_long(vk_constants_ce, "FORMAT_R16G16B16A16_SFLOAT", sizeof("FORMAT_R16G16B16A16_SFLOAT") - 1, VK_FORMAT_R16G16B16A16_SFLOAT);

    /* ---- VkBufferUsageFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "BUFFER_USAGE_TRANSFER_SRC", sizeof("BUFFER_USAGE_TRANSFER_SRC") - 1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "BUFFER_USAGE_TRANSFER_DST", sizeof("BUFFER_USAGE_TRANSFER_DST") - 1, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "BUFFER_USAGE_UNIFORM_BUFFER", sizeof("BUFFER_USAGE_UNIFORM_BUFFER") - 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "BUFFER_USAGE_STORAGE_BUFFER", sizeof("BUFFER_USAGE_STORAGE_BUFFER") - 1, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "BUFFER_USAGE_INDEX_BUFFER", sizeof("BUFFER_USAGE_INDEX_BUFFER") - 1, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "BUFFER_USAGE_VERTEX_BUFFER", sizeof("BUFFER_USAGE_VERTEX_BUFFER") - 1, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    /* ---- VkShaderStageFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "SHADER_STAGE_VERTEX", sizeof("SHADER_STAGE_VERTEX") - 1, VK_SHADER_STAGE_VERTEX_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "SHADER_STAGE_FRAGMENT", sizeof("SHADER_STAGE_FRAGMENT") - 1, VK_SHADER_STAGE_FRAGMENT_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "SHADER_STAGE_COMPUTE", sizeof("SHADER_STAGE_COMPUTE") - 1, VK_SHADER_STAGE_COMPUTE_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "SHADER_STAGE_ALL_GRAPHICS", sizeof("SHADER_STAGE_ALL_GRAPHICS") - 1, VK_SHADER_STAGE_ALL_GRAPHICS);
    zend_declare_class_constant_long(vk_constants_ce, "SHADER_STAGE_ALL", sizeof("SHADER_STAGE_ALL") - 1, VK_SHADER_STAGE_ALL);

    /* ---- VkDescriptorType ---- */
    zend_declare_class_constant_long(vk_constants_ce, "DESCRIPTOR_TYPE_SAMPLER", sizeof("DESCRIPTOR_TYPE_SAMPLER") - 1, VK_DESCRIPTOR_TYPE_SAMPLER);
    zend_declare_class_constant_long(vk_constants_ce, "DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER", sizeof("DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER") - 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    zend_declare_class_constant_long(vk_constants_ce, "DESCRIPTOR_TYPE_SAMPLED_IMAGE", sizeof("DESCRIPTOR_TYPE_SAMPLED_IMAGE") - 1, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
    zend_declare_class_constant_long(vk_constants_ce, "DESCRIPTOR_TYPE_STORAGE_IMAGE", sizeof("DESCRIPTOR_TYPE_STORAGE_IMAGE") - 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    zend_declare_class_constant_long(vk_constants_ce, "DESCRIPTOR_TYPE_UNIFORM_BUFFER", sizeof("DESCRIPTOR_TYPE_UNIFORM_BUFFER") - 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    zend_declare_class_constant_long(vk_constants_ce, "DESCRIPTOR_TYPE_STORAGE_BUFFER", sizeof("DESCRIPTOR_TYPE_STORAGE_BUFFER") - 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

    /* ---- VkPipelineBindPoint ---- */
    zend_declare_class_constant_long(vk_constants_ce, "PIPELINE_BIND_POINT_GRAPHICS", sizeof("PIPELINE_BIND_POINT_GRAPHICS") - 1, VK_PIPELINE_BIND_POINT_GRAPHICS);
    zend_declare_class_constant_long(vk_constants_ce, "PIPELINE_BIND_POINT_COMPUTE", sizeof("PIPELINE_BIND_POINT_COMPUTE") - 1, VK_PIPELINE_BIND_POINT_COMPUTE);

    /* ---- VkPrimitiveTopology ---- */
    zend_declare_class_constant_long(vk_constants_ce, "PRIMITIVE_TOPOLOGY_POINT_LIST", sizeof("PRIMITIVE_TOPOLOGY_POINT_LIST") - 1, VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
    zend_declare_class_constant_long(vk_constants_ce, "PRIMITIVE_TOPOLOGY_LINE_LIST", sizeof("PRIMITIVE_TOPOLOGY_LINE_LIST") - 1, VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
    zend_declare_class_constant_long(vk_constants_ce, "PRIMITIVE_TOPOLOGY_LINE_STRIP", sizeof("PRIMITIVE_TOPOLOGY_LINE_STRIP") - 1, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP);
    zend_declare_class_constant_long(vk_constants_ce, "PRIMITIVE_TOPOLOGY_TRIANGLE_LIST", sizeof("PRIMITIVE_TOPOLOGY_TRIANGLE_LIST") - 1, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    zend_declare_class_constant_long(vk_constants_ce, "PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP", sizeof("PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP") - 1, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
    zend_declare_class_constant_long(vk_constants_ce, "PRIMITIVE_TOPOLOGY_TRIANGLE_FAN", sizeof("PRIMITIVE_TOPOLOGY_TRIANGLE_FAN") - 1, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN);

    /* ---- VkImageLayout ---- */
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_LAYOUT_UNDEFINED", sizeof("IMAGE_LAYOUT_UNDEFINED") - 1, VK_IMAGE_LAYOUT_UNDEFINED);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_LAYOUT_GENERAL", sizeof("IMAGE_LAYOUT_GENERAL") - 1, VK_IMAGE_LAYOUT_GENERAL);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL", sizeof("IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL") - 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL", sizeof("IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL") - 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL", sizeof("IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL") - 1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL", sizeof("IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL") - 1, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL", sizeof("IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL") - 1, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_LAYOUT_PRESENT_SRC_KHR", sizeof("IMAGE_LAYOUT_PRESENT_SRC_KHR") - 1, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    /* ---- VkImageUsageFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_USAGE_TRANSFER_SRC", sizeof("IMAGE_USAGE_TRANSFER_SRC") - 1, VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_USAGE_TRANSFER_DST", sizeof("IMAGE_USAGE_TRANSFER_DST") - 1, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_USAGE_SAMPLED", sizeof("IMAGE_USAGE_SAMPLED") - 1, VK_IMAGE_USAGE_SAMPLED_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_USAGE_STORAGE", sizeof("IMAGE_USAGE_STORAGE") - 1, VK_IMAGE_USAGE_STORAGE_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_USAGE_COLOR_ATTACHMENT", sizeof("IMAGE_USAGE_COLOR_ATTACHMENT") - 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT", sizeof("IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT") - 1, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

    /* ---- VkCommandBufferUsageFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT", sizeof("COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT") - 1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE", sizeof("COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE") - 1, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

    /* ---- VkCommandPoolCreateFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER", sizeof("COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER") - 1, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "COMMAND_POOL_CREATE_TRANSIENT", sizeof("COMMAND_POOL_CREATE_TRANSIENT") - 1, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

    /* ---- VkPipelineStageFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "PIPELINE_STAGE_TOP_OF_PIPE", sizeof("PIPELINE_STAGE_TOP_OF_PIPE") - 1, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "PIPELINE_STAGE_VERTEX_INPUT", sizeof("PIPELINE_STAGE_VERTEX_INPUT") - 1, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "PIPELINE_STAGE_VERTEX_SHADER", sizeof("PIPELINE_STAGE_VERTEX_SHADER") - 1, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "PIPELINE_STAGE_FRAGMENT_SHADER", sizeof("PIPELINE_STAGE_FRAGMENT_SHADER") - 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT", sizeof("PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT") - 1, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "PIPELINE_STAGE_COMPUTE_SHADER", sizeof("PIPELINE_STAGE_COMPUTE_SHADER") - 1, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "PIPELINE_STAGE_TRANSFER", sizeof("PIPELINE_STAGE_TRANSFER") - 1, VK_PIPELINE_STAGE_TRANSFER_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "PIPELINE_STAGE_BOTTOM_OF_PIPE", sizeof("PIPELINE_STAGE_BOTTOM_OF_PIPE") - 1, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "PIPELINE_STAGE_ALL_COMMANDS", sizeof("PIPELINE_STAGE_ALL_COMMANDS") - 1, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    /* ---- VkAccessFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "ACCESS_SHADER_READ", sizeof("ACCESS_SHADER_READ") - 1, VK_ACCESS_SHADER_READ_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "ACCESS_SHADER_WRITE", sizeof("ACCESS_SHADER_WRITE") - 1, VK_ACCESS_SHADER_WRITE_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "ACCESS_TRANSFER_READ", sizeof("ACCESS_TRANSFER_READ") - 1, VK_ACCESS_TRANSFER_READ_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "ACCESS_TRANSFER_WRITE", sizeof("ACCESS_TRANSFER_WRITE") - 1, VK_ACCESS_TRANSFER_WRITE_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "ACCESS_HOST_READ", sizeof("ACCESS_HOST_READ") - 1, VK_ACCESS_HOST_READ_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "ACCESS_HOST_WRITE", sizeof("ACCESS_HOST_WRITE") - 1, VK_ACCESS_HOST_WRITE_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "ACCESS_MEMORY_READ", sizeof("ACCESS_MEMORY_READ") - 1, VK_ACCESS_MEMORY_READ_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "ACCESS_MEMORY_WRITE", sizeof("ACCESS_MEMORY_WRITE") - 1, VK_ACCESS_MEMORY_WRITE_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "ACCESS_COLOR_ATTACHMENT_WRITE", sizeof("ACCESS_COLOR_ATTACHMENT_WRITE") - 1, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    /* ---- VkCullModeFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "CULL_MODE_NONE", sizeof("CULL_MODE_NONE") - 1, VK_CULL_MODE_NONE);
    zend_declare_class_constant_long(vk_constants_ce, "CULL_MODE_FRONT", sizeof("CULL_MODE_FRONT") - 1, VK_CULL_MODE_FRONT_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "CULL_MODE_BACK", sizeof("CULL_MODE_BACK") - 1, VK_CULL_MODE_BACK_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "CULL_MODE_FRONT_AND_BACK", sizeof("CULL_MODE_FRONT_AND_BACK") - 1, VK_CULL_MODE_FRONT_AND_BACK);

    /* ---- VkFrontFace ---- */
    zend_declare_class_constant_long(vk_constants_ce, "FRONT_FACE_COUNTER_CLOCKWISE", sizeof("FRONT_FACE_COUNTER_CLOCKWISE") - 1, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    zend_declare_class_constant_long(vk_constants_ce, "FRONT_FACE_CLOCKWISE", sizeof("FRONT_FACE_CLOCKWISE") - 1, VK_FRONT_FACE_CLOCKWISE);

    /* ---- VkIndexType ---- */
    zend_declare_class_constant_long(vk_constants_ce, "INDEX_TYPE_UINT16", sizeof("INDEX_TYPE_UINT16") - 1, VK_INDEX_TYPE_UINT16);
    zend_declare_class_constant_long(vk_constants_ce, "INDEX_TYPE_UINT32", sizeof("INDEX_TYPE_UINT32") - 1, VK_INDEX_TYPE_UINT32);

    /* ---- VkImageAspectFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_ASPECT_COLOR", sizeof("IMAGE_ASPECT_COLOR") - 1, VK_IMAGE_ASPECT_COLOR_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_ASPECT_DEPTH", sizeof("IMAGE_ASPECT_DEPTH") - 1, VK_IMAGE_ASPECT_DEPTH_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "IMAGE_ASPECT_STENCIL", sizeof("IMAGE_ASPECT_STENCIL") - 1, VK_IMAGE_ASPECT_STENCIL_BIT);

    /* ---- VkFilter ---- */
    zend_declare_class_constant_long(vk_constants_ce, "FILTER_NEAREST", sizeof("FILTER_NEAREST") - 1, VK_FILTER_NEAREST);
    zend_declare_class_constant_long(vk_constants_ce, "FILTER_LINEAR", sizeof("FILTER_LINEAR") - 1, VK_FILTER_LINEAR);

    /* ---- VkSamplerAddressMode ---- */
    zend_declare_class_constant_long(vk_constants_ce, "SAMPLER_ADDRESS_MODE_REPEAT", sizeof("SAMPLER_ADDRESS_MODE_REPEAT") - 1, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    zend_declare_class_constant_long(vk_constants_ce, "SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT", sizeof("SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT") - 1, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
    zend_declare_class_constant_long(vk_constants_ce, "SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE", sizeof("SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE") - 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
    zend_declare_class_constant_long(vk_constants_ce, "SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER", sizeof("SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER") - 1, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);

    /* ---- VkAttachmentLoadOp ---- */
    zend_declare_class_constant_long(vk_constants_ce, "ATTACHMENT_LOAD_OP_LOAD", sizeof("ATTACHMENT_LOAD_OP_LOAD") - 1, VK_ATTACHMENT_LOAD_OP_LOAD);
    zend_declare_class_constant_long(vk_constants_ce, "ATTACHMENT_LOAD_OP_CLEAR", sizeof("ATTACHMENT_LOAD_OP_CLEAR") - 1, VK_ATTACHMENT_LOAD_OP_CLEAR);
    zend_declare_class_constant_long(vk_constants_ce, "ATTACHMENT_LOAD_OP_DONT_CARE", sizeof("ATTACHMENT_LOAD_OP_DONT_CARE") - 1, VK_ATTACHMENT_LOAD_OP_DONT_CARE);

    /* ---- VkAttachmentStoreOp ---- */
    zend_declare_class_constant_long(vk_constants_ce, "ATTACHMENT_STORE_OP_STORE", sizeof("ATTACHMENT_STORE_OP_STORE") - 1, VK_ATTACHMENT_STORE_OP_STORE);
    zend_declare_class_constant_long(vk_constants_ce, "ATTACHMENT_STORE_OP_DONT_CARE", sizeof("ATTACHMENT_STORE_OP_DONT_CARE") - 1, VK_ATTACHMENT_STORE_OP_DONT_CARE);

    /* ---- VkMemoryPropertyFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "MEMORY_PROPERTY_DEVICE_LOCAL", sizeof("MEMORY_PROPERTY_DEVICE_LOCAL") - 1, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "MEMORY_PROPERTY_HOST_VISIBLE", sizeof("MEMORY_PROPERTY_HOST_VISIBLE") - 1, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "MEMORY_PROPERTY_HOST_COHERENT", sizeof("MEMORY_PROPERTY_HOST_COHERENT") - 1, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "MEMORY_PROPERTY_HOST_CACHED", sizeof("MEMORY_PROPERTY_HOST_CACHED") - 1, VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    /* ---- VkPresentModeKHR ---- */
    zend_declare_class_constant_long(vk_constants_ce, "PRESENT_MODE_IMMEDIATE", sizeof("PRESENT_MODE_IMMEDIATE") - 1, VK_PRESENT_MODE_IMMEDIATE_KHR);
    zend_declare_class_constant_long(vk_constants_ce, "PRESENT_MODE_MAILBOX", sizeof("PRESENT_MODE_MAILBOX") - 1, VK_PRESENT_MODE_MAILBOX_KHR);
    zend_declare_class_constant_long(vk_constants_ce, "PRESENT_MODE_FIFO", sizeof("PRESENT_MODE_FIFO") - 1, VK_PRESENT_MODE_FIFO_KHR);
    zend_declare_class_constant_long(vk_constants_ce, "PRESENT_MODE_FIFO_RELAXED", sizeof("PRESENT_MODE_FIFO_RELAXED") - 1, VK_PRESENT_MODE_FIFO_RELAXED_KHR);

    /* ---- VkQueryType ---- */
    zend_declare_class_constant_long(vk_constants_ce, "QUERY_TYPE_OCCLUSION", sizeof("QUERY_TYPE_OCCLUSION") - 1, VK_QUERY_TYPE_OCCLUSION);
    zend_declare_class_constant_long(vk_constants_ce, "QUERY_TYPE_PIPELINE_STATISTICS", sizeof("QUERY_TYPE_PIPELINE_STATISTICS") - 1, VK_QUERY_TYPE_PIPELINE_STATISTICS);
    zend_declare_class_constant_long(vk_constants_ce, "QUERY_TYPE_TIMESTAMP", sizeof("QUERY_TYPE_TIMESTAMP") - 1, VK_QUERY_TYPE_TIMESTAMP);

    /* ---- VkQueryResultFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "QUERY_RESULT_64", sizeof("QUERY_RESULT_64") - 1, VK_QUERY_RESULT_64_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "QUERY_RESULT_WAIT", sizeof("QUERY_RESULT_WAIT") - 1, VK_QUERY_RESULT_WAIT_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "QUERY_RESULT_WITH_AVAILABILITY", sizeof("QUERY_RESULT_WITH_AVAILABILITY") - 1, VK_QUERY_RESULT_WITH_AVAILABILITY_BIT);

    /* ---- VkStencilFaceFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "STENCIL_FACE_FRONT", sizeof("STENCIL_FACE_FRONT") - 1, VK_STENCIL_FACE_FRONT_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "STENCIL_FACE_BACK", sizeof("STENCIL_FACE_BACK") - 1, VK_STENCIL_FACE_BACK_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "STENCIL_FACE_FRONT_AND_BACK", sizeof("STENCIL_FACE_FRONT_AND_BACK") - 1, VK_STENCIL_FACE_FRONT_AND_BACK);

    /* ---- VkDynamicState ---- */
    zend_declare_class_constant_long(vk_constants_ce, "DYNAMIC_STATE_VIEWPORT", sizeof("DYNAMIC_STATE_VIEWPORT") - 1, VK_DYNAMIC_STATE_VIEWPORT);
    zend_declare_class_constant_long(vk_constants_ce, "DYNAMIC_STATE_SCISSOR", sizeof("DYNAMIC_STATE_SCISSOR") - 1, VK_DYNAMIC_STATE_SCISSOR);
    zend_declare_class_constant_long(vk_constants_ce, "DYNAMIC_STATE_LINE_WIDTH", sizeof("DYNAMIC_STATE_LINE_WIDTH") - 1, VK_DYNAMIC_STATE_LINE_WIDTH);
    zend_declare_class_constant_long(vk_constants_ce, "DYNAMIC_STATE_DEPTH_BIAS", sizeof("DYNAMIC_STATE_DEPTH_BIAS") - 1, VK_DYNAMIC_STATE_DEPTH_BIAS);
    zend_declare_class_constant_long(vk_constants_ce, "DYNAMIC_STATE_BLEND_CONSTANTS", sizeof("DYNAMIC_STATE_BLEND_CONSTANTS") - 1, VK_DYNAMIC_STATE_BLEND_CONSTANTS);
    zend_declare_class_constant_long(vk_constants_ce, "DYNAMIC_STATE_DEPTH_BOUNDS", sizeof("DYNAMIC_STATE_DEPTH_BOUNDS") - 1, VK_DYNAMIC_STATE_DEPTH_BOUNDS);
    zend_declare_class_constant_long(vk_constants_ce, "DYNAMIC_STATE_STENCIL_COMPARE_MASK", sizeof("DYNAMIC_STATE_STENCIL_COMPARE_MASK") - 1, VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
    zend_declare_class_constant_long(vk_constants_ce, "DYNAMIC_STATE_STENCIL_WRITE_MASK", sizeof("DYNAMIC_STATE_STENCIL_WRITE_MASK") - 1, VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
    zend_declare_class_constant_long(vk_constants_ce, "DYNAMIC_STATE_STENCIL_REFERENCE", sizeof("DYNAMIC_STATE_STENCIL_REFERENCE") - 1, VK_DYNAMIC_STATE_STENCIL_REFERENCE);

    /* ---- VkSampleCountFlagBits ---- */
    zend_declare_class_constant_long(vk_constants_ce, "SAMPLE_COUNT_1", sizeof("SAMPLE_COUNT_1") - 1, VK_SAMPLE_COUNT_1_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "SAMPLE_COUNT_2", sizeof("SAMPLE_COUNT_2") - 1, VK_SAMPLE_COUNT_2_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "SAMPLE_COUNT_4", sizeof("SAMPLE_COUNT_4") - 1, VK_SAMPLE_COUNT_4_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "SAMPLE_COUNT_8", sizeof("SAMPLE_COUNT_8") - 1, VK_SAMPLE_COUNT_8_BIT);
    zend_declare_class_constant_long(vk_constants_ce, "SAMPLE_COUNT_16", sizeof("SAMPLE_COUNT_16") - 1, VK_SAMPLE_COUNT_16_BIT);

    /* ---- VkSubpassExternal ---- */
    zend_declare_class_constant_long(vk_constants_ce, "SUBPASS_EXTERNAL", sizeof("SUBPASS_EXTERNAL") - 1, VK_SUBPASS_EXTERNAL);

    /* ---- VkVertexInputRate ---- */
    zend_declare_class_constant_long(vk_constants_ce, "VERTEX_INPUT_RATE_VERTEX", sizeof("VERTEX_INPUT_RATE_VERTEX") - 1, VK_VERTEX_INPUT_RATE_VERTEX);
    zend_declare_class_constant_long(vk_constants_ce, "VERTEX_INPUT_RATE_INSTANCE", sizeof("VERTEX_INPUT_RATE_INSTANCE") - 1, VK_VERTEX_INPUT_RATE_INSTANCE);

    /* ---- VkSharingMode ---- */
    zend_declare_class_constant_long(vk_constants_ce, "SHARING_MODE_EXCLUSIVE", sizeof("SHARING_MODE_EXCLUSIVE") - 1, VK_SHARING_MODE_EXCLUSIVE);
    zend_declare_class_constant_long(vk_constants_ce, "SHARING_MODE_CONCURRENT", sizeof("SHARING_MODE_CONCURRENT") - 1, VK_SHARING_MODE_CONCURRENT);

    /* ---- Vulkan API version helpers ---- */
    zend_declare_class_constant_long(vk_constants_ce, "API_VERSION_1_0", sizeof("API_VERSION_1_0") - 1, VK_API_VERSION_1_0);
    zend_declare_class_constant_long(vk_constants_ce, "API_VERSION_1_1", sizeof("API_VERSION_1_1") - 1, VK_API_VERSION_1_1);
    zend_declare_class_constant_long(vk_constants_ce, "API_VERSION_1_2", sizeof("API_VERSION_1_2") - 1, VK_API_VERSION_1_2);
    zend_declare_class_constant_long(vk_constants_ce, "API_VERSION_1_3", sizeof("API_VERSION_1_3") - 1, VK_API_VERSION_1_3);
}
