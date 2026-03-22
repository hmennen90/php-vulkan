/*
  +----------------------------------------------------------------------+
  | PHP Vulkan — VkQueue                                                 |
  +----------------------------------------------------------------------+
  | Copyright (c) 2026 Hendrik Mennen                                    |
  +----------------------------------------------------------------------+
*/

#include "php_vulkan.h"

zend_class_entry *vk_queue_ce;
static zend_object_handlers vk_queue_handlers;

static zend_object *vk_queue_create_object(zend_class_entry *ce) {
    vk_queue_object *intern = zend_object_alloc(sizeof(vk_queue_object), ce);
    intern->queue = VK_NULL_HANDLE;
    ZVAL_UNDEF(&intern->device_zval);
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &vk_queue_handlers;
    return &intern->std;
}

static void vk_queue_free_object(zend_object *object) {
    vk_queue_object *intern = VK_OBJ(vk_queue_object, object);
    zval_ptr_dtor(&intern->device_zval);
    zend_object_std_dtor(&intern->std);
}

/* Vk\Queue::submit(array<Vk\CommandBuffer> $commandBuffers, ?Vk\Fence $fence = null,
 *                  array<Vk\Semaphore> $waitSemaphores = [],
 *                  array<Vk\Semaphore> $signalSemaphores = []): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_queue_submit, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, commandBuffers, IS_ARRAY, 0)
    ZEND_ARG_OBJ_INFO(0, fence, Vk\\Fence, 1)
    ZEND_ARG_TYPE_INFO(0, waitSemaphores, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, signalSemaphores, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkQueue, submit) {
    HashTable *cmd_buffers;
    zval *fence_zval = NULL;
    HashTable *wait_sems = NULL;
    HashTable *signal_sems = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 4)
        Z_PARAM_ARRAY_HT(cmd_buffers)
        Z_PARAM_OPTIONAL
        Z_PARAM_OBJECT_OF_CLASS_OR_NULL(fence_zval, vk_fence_ce)
        Z_PARAM_ARRAY_HT(wait_sems)
        Z_PARAM_ARRAY_HT(signal_sems)
    ZEND_PARSE_PARAMETERS_END();

    vk_queue_object *intern = VK_OBJ(vk_queue_object, Z_OBJ_P(ZEND_THIS));

    /* Command buffers */
    uint32_t cb_count = zend_hash_num_elements(cmd_buffers);
    VkCommandBuffer *cbs = ecalloc(cb_count, sizeof(VkCommandBuffer));
    zval *zv;
    uint32_t idx = 0;
    ZEND_HASH_FOREACH_VAL(cmd_buffers, zv) {
        if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), vk_command_buffer_ce)) {
            vk_command_buffer_object *cb = VK_OBJ(vk_command_buffer_object, Z_OBJ_P(zv));
            cbs[idx++] = cb->command_buffer;
        }
    } ZEND_HASH_FOREACH_END();

    /* Wait semaphores */
    uint32_t wait_count = wait_sems ? zend_hash_num_elements(wait_sems) : 0;
    VkSemaphore *wait_sem_handles = NULL;
    VkPipelineStageFlags *wait_stages = NULL;
    if (wait_count > 0) {
        wait_sem_handles = ecalloc(wait_count, sizeof(VkSemaphore));
        wait_stages = ecalloc(wait_count, sizeof(VkPipelineStageFlags));
        uint32_t wi = 0;
        ZEND_HASH_FOREACH_VAL(wait_sems, zv) {
            if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), vk_semaphore_ce)) {
                vk_semaphore_object *s = VK_OBJ(vk_semaphore_object, Z_OBJ_P(zv));
                wait_sem_handles[wi] = s->semaphore;
                wait_stages[wi] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                wi++;
            }
        } ZEND_HASH_FOREACH_END();
        wait_count = wi;
    }

    /* Signal semaphores */
    uint32_t signal_count = signal_sems ? zend_hash_num_elements(signal_sems) : 0;
    VkSemaphore *signal_sem_handles = NULL;
    if (signal_count > 0) {
        signal_sem_handles = ecalloc(signal_count, sizeof(VkSemaphore));
        uint32_t si = 0;
        ZEND_HASH_FOREACH_VAL(signal_sems, zv) {
            if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), vk_semaphore_ce)) {
                vk_semaphore_object *s = VK_OBJ(vk_semaphore_object, Z_OBJ_P(zv));
                signal_sem_handles[si++] = s->semaphore;
            }
        } ZEND_HASH_FOREACH_END();
        signal_count = si;
    }

    VkFence fence = VK_NULL_HANDLE;
    if (fence_zval) {
        vk_fence_object *f = VK_OBJ(vk_fence_object, Z_OBJ_P(fence_zval));
        fence = f->fence;
    }

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = wait_count,
        .pWaitSemaphores = wait_sem_handles,
        .pWaitDstStageMask = wait_stages,
        .commandBufferCount = idx,
        .pCommandBuffers = cbs,
        .signalSemaphoreCount = signal_count,
        .pSignalSemaphores = signal_sem_handles,
    };

    VkResult result = vkQueueSubmit(intern->queue, 1, &submit_info, fence);

    efree(cbs);
    if (wait_sem_handles) efree(wait_sem_handles);
    if (wait_stages) efree(wait_stages);
    if (signal_sem_handles) efree(signal_sem_handles);

    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to submit queue");
    }
}

/* Vk\Queue::waitIdle(): void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_queue_waitIdle, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkQueue, waitIdle) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_queue_object *intern = VK_OBJ(vk_queue_object, Z_OBJ_P(ZEND_THIS));
    VkResult result = vkQueueWaitIdle(intern->queue);
    if (result != VK_SUCCESS) {
        vk_throw_exception(result, "Failed to wait for queue idle");
    }
}

/* Vk\Queue::getFamilyIndex(): int */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_queue_getFamilyIndex, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkQueue, getFamilyIndex) {
    ZEND_PARSE_PARAMETERS_NONE();
    vk_queue_object *intern = VK_OBJ(vk_queue_object, Z_OBJ_P(ZEND_THIS));
    RETURN_LONG((zend_long)intern->family_index);
}

/* Vk\Queue::present(array<Vk\Swapchain> $swapchains, array<int> $imageIndices,
 *                    array<Vk\Semaphore> $waitSemaphores = []): int */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_vk_queue_present, 0, 2, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, swapchains, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, imageIndices, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, waitSemaphores, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(VkQueue, present) {
    HashTable *swapchains, *image_indices;
    HashTable *wait_sems = NULL;

    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_ARRAY_HT(swapchains)
        Z_PARAM_ARRAY_HT(image_indices)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY_HT(wait_sems)
    ZEND_PARSE_PARAMETERS_END();

    vk_queue_object *intern = VK_OBJ(vk_queue_object, Z_OBJ_P(ZEND_THIS));

    /* Swapchains */
    uint32_t sc_count = zend_hash_num_elements(swapchains);
    VkSwapchainKHR *sc_handles = ecalloc(sc_count, sizeof(VkSwapchainKHR));
    zval *zv;
    uint32_t idx = 0;
    ZEND_HASH_FOREACH_VAL(swapchains, zv) {
        if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), vk_swapchain_ce)) {
            vk_swapchain_object *sc = VK_OBJ(vk_swapchain_object, Z_OBJ_P(zv));
            sc_handles[idx++] = sc->swapchain;
        }
    } ZEND_HASH_FOREACH_END();
    sc_count = idx;

    /* Image indices */
    uint32_t *indices = ecalloc(sc_count, sizeof(uint32_t));
    idx = 0;
    ZEND_HASH_FOREACH_VAL(image_indices, zv) {
        if (idx < sc_count) {
            indices[idx++] = (uint32_t)zval_get_long(zv);
        }
    } ZEND_HASH_FOREACH_END();

    /* Wait semaphores */
    uint32_t wait_count = wait_sems ? zend_hash_num_elements(wait_sems) : 0;
    VkSemaphore *wait_sem_handles = NULL;
    if (wait_count > 0) {
        wait_sem_handles = ecalloc(wait_count, sizeof(VkSemaphore));
        uint32_t wi = 0;
        ZEND_HASH_FOREACH_VAL(wait_sems, zv) {
            if (Z_TYPE_P(zv) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zv), vk_semaphore_ce)) {
                vk_semaphore_object *s = VK_OBJ(vk_semaphore_object, Z_OBJ_P(zv));
                wait_sem_handles[wi++] = s->semaphore;
            }
        } ZEND_HASH_FOREACH_END();
        wait_count = wi;
    }

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = wait_count,
        .pWaitSemaphores = wait_sem_handles,
        .swapchainCount = sc_count,
        .pSwapchains = sc_handles,
        .pImageIndices = indices,
    };

    VkResult result = vkQueuePresentKHR(intern->queue, &present_info);

    efree(sc_handles);
    efree(indices);
    if (wait_sem_handles) efree(wait_sem_handles);

    RETURN_LONG((zend_long)result);
}

static const zend_function_entry vk_queue_methods[] = {
    PHP_ME(VkQueue, submit,         arginfo_vk_queue_submit,         ZEND_ACC_PUBLIC)
    PHP_ME(VkQueue, present,        arginfo_vk_queue_present,        ZEND_ACC_PUBLIC)
    PHP_ME(VkQueue, waitIdle,       arginfo_vk_queue_waitIdle,       ZEND_ACC_PUBLIC)
    PHP_ME(VkQueue, getFamilyIndex, arginfo_vk_queue_getFamilyIndex, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_vk_queue_register(void) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Vk", "Queue", vk_queue_methods);
    vk_queue_ce = zend_register_internal_class(&ce);
    vk_queue_ce->create_object = vk_queue_create_object;
    vk_queue_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    memcpy(&vk_queue_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    vk_queue_handlers.offset = XtOffsetOf(vk_queue_object, std);
    vk_queue_handlers.free_obj = vk_queue_free_object;
    vk_queue_handlers.clone_obj = NULL;
}
