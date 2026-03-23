/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkDevice (logical device)                               |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_device_ce;
static zend_object_handlers vk_device_handlers;

/* ------------------------------------------------------------------ */
/*  Object lifecycle                                                   */
/* ------------------------------------------------------------------ */

static zend_object *vk_device_create_object(zend_class_entry *ce) {
    vk_device_object *intern = zend_object_alloc(sizeof(vk_device_object), ce);
    intern->device = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->physical_device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_device_handlers;
    return &intern->std;
}

static void vk_device_free_object(zend_object *object) {
    vk_device_object *intern = VK_OBJ(vk_device_object, object);
    if (intern->device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(intern->device);
        vkDestroyDevice(intern->device, NULL);
        intern->device = VK_NULL_HANDLE;
    }
    zval_ptr_dtor(&intern->physical_device_zval);
    zend_object_std_dtor(&intern->std);
}

/* ------------------------------------------------------------------ */
/*  Methods                                                            */
/* ------------------------------------------------------------------ */

/* Vk\Device::__construct(Vk\PhysicalDevice $physicalDevice, array $queueFamilies,
 *                        array $extensions = [], ?array $features = null) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_vk_device___construct, 0, 0, 2)
    ZEND_ARG_OBJ_INFO(0, physicalDevice, Vk\\PhysicalDevice, 0)
    ZEND_ARG_TYPE_INFO(0, queueFamilies, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, extensions, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, features, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

PHP_METHOD(VkDevice, __construct) {
    zval *physical_device_zval;
    HashTable *queue_families;
    HashTable *extensions = NULL;
    HashTable *features = NULL;

    ZEND_PARSE_PARAMETERS_START(2, 4)
        Z_PARAM_OBJECT_OF_CLASS(physical_device_zval, vk_physical_device_ce)
        Z_PARAM_ARRAY_HT(queue_families)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY_HT(extensions)
        Z_PARAM_ARRAY_HT_OR_NULL(features)
    ZEND_PARSE_PARAMETERS_END();

    vk_device_object *intern = VK_OBJ(vk_device_object, Z_OBJ_P(ZEND_THIS));
    vk_physical_device_object *pd = VK_OBJ(vk_physical_device_object, Z_OBJ_P(physical_device_zval));

    ZVAL_COPY(&intern->physical_device_zval, physical_device_zval);

    /* Build queue create infos
     * Expected format: [[familyIndex => int, count => int, priorities => [float...]], ...] */
    uint32_t qf_count = zend_hash_num_elements(queue_families);
    VkDeviceQueueCreateInfo *queue_infos = ecalloc(qf_count, sizeof(VkDeviceQueueCreateInfo));
    float **priority_arrays = ecalloc(qf_count, sizeof(float *));

    zval *qf;
    uint32_t qi = 0;
    ZEND_HASH_FOREACH_VAL(queue_families, qf) {
        if (Z_TYPE_P(qf) != IS_ARRAY) continue;

        HashTable *qf_ht = Z_ARRVAL_P(qf);
        zval *zfamily = zend_hash_str_find(qf_ht, "familyIndex", sizeof("familyIndex") - 1);
        zval *zcount = zend_hash_str_find(qf_ht, "count", sizeof("count") - 1);

        uint32_t family_index = zfamily ? (uint32_t)zval_get_long(zfamily) : 0;
        uint32_t q_count = zcount ? (uint32_t)zval_get_long(zcount) : 1;

        priority_arrays[qi] = ecalloc(q_count, sizeof(float));
        zval *zpriorities = zend_hash_str_find(qf_ht, "priorities", sizeof("priorities") - 1);
        if (zpriorities && Z_TYPE_P(zpriorities) == IS_ARRAY) {
            zval *zp;
            uint32_t pi = 0;
            ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zpriorities), zp) {
                if (pi < q_count) {
                    priority_arrays[qi][pi++] = (float)zval_get_double(zp);
                }
            } ZEND_HASH_FOREACH_END();
        } else {
            for (uint32_t p = 0; p < q_count; p++) {
                priority_arrays[qi][p] = 1.0f;
            }
        }

        queue_infos[qi] = (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = family_index,
            .queueCount = q_count,
            .pQueuePriorities = priority_arrays[qi],
        };
        qi++;
    } ZEND_HASH_FOREACH_END();

    /* Extensions */
    uint32_t ext_count = 0;
    const char **ext_names = NULL;

#ifdef __APPLE__
    /* Always add portability subset on macOS */
    uint32_t user_ext_count = extensions ? zend_hash_num_elements(extensions) : 0;
    ext_count = user_ext_count + 1;
    ext_names = ecalloc(ext_count, sizeof(const char *));
    ext_names[0] = "VK_KHR_portability_subset";
    if (user_ext_count > 0) {
        zval *zv;
        uint32_t idx = 1;
        ZEND_HASH_FOREACH_VAL(extensions, zv) {
            if (Z_TYPE_P(zv) == IS_STRING) {
                ext_names[idx++] = Z_STRVAL_P(zv);
            }
        } ZEND_HASH_FOREACH_END();
    }
#else
    if (extensions) {
        ext_count = zend_hash_num_elements(extensions);
        ext_names = ecalloc(ext_count, sizeof(const char *));
        zval *zv;
        uint32_t idx = 0;
        ZEND_HASH_FOREACH_VAL(extensions, zv) {
            if (Z_TYPE_P(zv) == IS_STRING) {
                ext_names[idx++] = Z_STRVAL_P(zv);
            }
        } ZEND_HASH_FOREACH_END();
    }
#endif

    VkPhysicalDeviceFeatures device_features = {0};
    if (features) {
        /* Parse features array — keys match VkPhysicalDeviceFeatures field names */
        #define CHECK_FEATURE(name) do { \
            zval *_zv = zend_hash_str_find(features, #name, sizeof(#name) - 1); \
            if (_zv) device_features.name = zval_is_true(_zv) ? VK_TRUE : VK_FALSE; \
        } while(0)

        CHECK_FEATURE(robustBufferAccess);
        CHECK_FEATURE(fullDrawIndexUint32);
        CHECK_FEATURE(imageCubeArray);
        CHECK_FEATURE(independentBlend);
        CHECK_FEATURE(geometryShader);
        CHECK_FEATURE(tessellationShader);
        CHECK_FEATURE(sampleRateShading);
        CHECK_FEATURE(dualSrcBlend);
        CHECK_FEATURE(logicOp);
        CHECK_FEATURE(multiDrawIndirect);
        CHECK_FEATURE(drawIndirectFirstInstance);
        CHECK_FEATURE(depthClamp);
        CHECK_FEATURE(depthBiasClamp);
        CHECK_FEATURE(fillModeNonSolid);
        CHECK_FEATURE(depthBounds);
        CHECK_FEATURE(wideLines);
        CHECK_FEATURE(largePoints);
        CHECK_FEATURE(alphaToOne);
        CHECK_FEATURE(multiViewport);
        CHECK_FEATURE(samplerAnisotropy);
        CHECK_FEATURE(textureCompressionETC2);
        CHECK_FEATURE(textureCompressionASTC_LDR);
        CHECK_FEATURE(textureCompressionBC);
        CHECK_FEATURE(occlusionQueryPrecise);
        CHECK_FEATURE(pipelineStatisticsQuery);
        CHECK_FEATURE(vertexPipelineStoresAndAtomics);
        CHECK_FEATURE(fragmentStoresAndAtomics);
        CHECK_FEATURE(shaderTessellationAndGeometryPointSize);
        CHECK_FEATURE(shaderImageGatherExtended);
        CHECK_FEATURE(shaderStorageImageExtendedFormats);
        CHECK_FEATURE(shaderStorageImageMultisample);
        CHECK_FEATURE(shaderStorageImageReadWithoutFormat);
        CHECK_FEATURE(shaderStorageImageWriteWithoutFormat);
        CHECK_FEATURE(shaderUniformBufferArrayDynamicIndexing);
        CHECK_FEATURE(shaderSampledImageArrayDynamicIndexing);
        CHECK_FEATURE(shaderStorageBufferArrayDynamicIndexing);
        CHECK_FEATURE(shaderStorageImageArrayDynamicIndexing);
        CHECK_FEATURE(shaderClipDistance);
        CHECK_FEATURE(shaderCullDistance);
        CHECK_FEATURE(shaderFloat64);
        CHECK_FEATURE(shaderInt64);
        CHECK_FEATURE(shaderInt16);
        CHECK_FEATURE(shaderResourceResidency);
        CHECK_FEATURE(shaderResourceMinLod);
        CHECK_FEATURE(sparseBinding);
        CHECK_FEATURE(sparseResidencyBuffer);
        CHECK_FEATURE(sparseResidencyImage2D);
        CHECK_FEATURE(sparseResidencyImage3D);
        CHECK_FEATURE(sparseResidency2Samples);
        CHECK_FEATURE(sparseResidency4Samples);
        CHECK_FEATURE(sparseResidency8Samples);
        CHECK_FEATURE(sparseResidency16Samples);
        CHECK_FEATURE(sparseResidencyAliased);
        CHECK_FEATURE(variableMultisampleRate);
        CHECK_FEATURE(inheritedQueries);

        #undef CHECK_FEATURE
    }

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = qi,
        .pQueueCreateInfos = queue_infos,
        .enabledExtensionCount = ext_count,
        .ppEnabledExtensionNames = ext_names,
        .pEnabledFeatures = &device_features,
    };

    VkResult result = vkCreateDevice(pd->physical_device, &create_info, NULL, &intern->device);

    /* Cleanup */
    for (uint32_t i = 0; i < qi; i++) {
        efree(priority_arrays[i]);
    }
    efree(priority_arrays);
    efree(queue_infos);
    if (ext_names) efree(ext_names);

    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to create logical device");
        return;
    }
}

/* Vk\Device::getQueue(int $familyIndex, int $queueIndex = 0): Vk\Queue */
ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_vk_device_getQueue, 0, 1, Vk\\Queue, 0)
    ZEND_ARG_TYPE_INFO(0, familyIndex, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, queueIndex, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

PHP_METHOD(VkDevice, getQueue) {
    zend_long family_index;
    zend_long queue_index = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_LONG(family_index)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(queue_index)
    ZEND_PARSE_PARAMETERS_END();

    vk_device_object *intern = VK_OBJ(vk_device_object, Z_OBJ_P(ZEND_THIS));

    VkQueue queue;
    vkGetDeviceQueue(intern->device, (uint32_t)family_index, (uint32_t)queue_index, &queue);

    object_init_ex(return_value, vk_queue_ce);
    vk_queue_object *q = VK_OBJ(vk_queue_object, Z_OBJ_P(return_value));
    q->queue = queue;
    q->family_index = (uint32_t)family_index;
    q->queue_index = (uint32_t)queue_index;
    ZVAL_COPY(&q->device_zval, ZEND_THIS);
}

/* Vk\Device::waitIdle(): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_device_waitIdle, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkDevice, waitIdle) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_device_object *intern = VK_OBJ(vk_device_object, Z_OBJ_P(ZEND_THIS));
    VkResult result = vkDeviceWaitIdle(intern->device);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to wait for device idle");
    }
}

/* ------------------------------------------------------------------ */
/*  Method table                                                       */
/* ------------------------------------------------------------------ */

static const zend_function_entry vk_device_methods[] = {
    PHP_ME(VkDevice, __construct, arginfo_vk_device___construct, ZEND_ACC_PUBLIC)
    PHP_ME(VkDevice, getQueue,    arginfo_vk_device_getQueue,    ZEND_ACC_PUBLIC)
    PHP_ME(VkDevice, waitIdle,    arginfo_vk_device_waitIdle,    ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* ------------------------------------------------------------------ */
/*  Registration                                                       */
/* ------------------------------------------------------------------ */

void php_vk_device_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Device", vk_device_methods);
    vk_device_ce = zend_register_internal_class(&ce);
    vk_device_ce->create_object = vk_device_create_object;
    vk_device_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_device_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_device_handlers.offset = XtOffsetOf(vk_device_object, std);
    vk_device_handlers.free_obj = vk_device_free_object;
    vk_device_handlers.clone_obj = NULL;
}
