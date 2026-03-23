<?php
/**
 * PHP Vulkan Extension Stubs
 *
 * Auto-generated for IDE autocomplete support.
 * Do NOT include this file at runtime — it is only for static analysis.
 *
 * @package hmennen90/php-vulkan
 * @version 0.2.0
 */

namespace Vk;

/**
 * Vulkan constants.
 *
 * Access via Vk\Vk::CONSTANT_NAME
 */
abstract final class Vk
{
    /* VkFormat */
    public const FORMAT_UNDEFINED = 0;
    public const FORMAT_R8_UNORM = 9;
    public const FORMAT_R8G8B8A8_UNORM = 37;
    public const FORMAT_R8G8B8A8_SRGB = 43;
    public const FORMAT_B8G8R8A8_UNORM = 44;
    public const FORMAT_B8G8R8A8_SRGB = 50;
    public const FORMAT_R32_SFLOAT = 100;
    public const FORMAT_R32G32_SFLOAT = 103;
    public const FORMAT_R32G32B32_SFLOAT = 106;
    public const FORMAT_R32G32B32A32_SFLOAT = 109;
    public const FORMAT_D32_SFLOAT = 126;
    public const FORMAT_D24_UNORM_S8_UINT = 129;
    public const FORMAT_R16G16B16A16_SFLOAT = 97;

    /* VkBufferUsageFlagBits */
    public const BUFFER_USAGE_TRANSFER_SRC = 0x00000001;
    public const BUFFER_USAGE_TRANSFER_DST = 0x00000002;
    public const BUFFER_USAGE_UNIFORM_BUFFER = 0x00000010;
    public const BUFFER_USAGE_STORAGE_BUFFER = 0x00000020;
    public const BUFFER_USAGE_INDEX_BUFFER = 0x00000040;
    public const BUFFER_USAGE_VERTEX_BUFFER = 0x00000080;

    /* VkShaderStageFlagBits */
    public const SHADER_STAGE_VERTEX = 0x00000001;
    public const SHADER_STAGE_FRAGMENT = 0x00000010;
    public const SHADER_STAGE_COMPUTE = 0x00000020;
    public const SHADER_STAGE_ALL_GRAPHICS = 0x0000001F;
    public const SHADER_STAGE_ALL = 0x7FFFFFFF;

    /* VkDescriptorType */
    public const DESCRIPTOR_TYPE_SAMPLER = 0;
    public const DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1;
    public const DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2;
    public const DESCRIPTOR_TYPE_STORAGE_IMAGE = 3;
    public const DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6;
    public const DESCRIPTOR_TYPE_STORAGE_BUFFER = 7;

    /* VkPipelineBindPoint */
    public const PIPELINE_BIND_POINT_GRAPHICS = 0;
    public const PIPELINE_BIND_POINT_COMPUTE = 1;

    /* VkPrimitiveTopology */
    public const PRIMITIVE_TOPOLOGY_POINT_LIST = 0;
    public const PRIMITIVE_TOPOLOGY_LINE_LIST = 1;
    public const PRIMITIVE_TOPOLOGY_LINE_STRIP = 2;
    public const PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3;
    public const PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4;
    public const PRIMITIVE_TOPOLOGY_TRIANGLE_FAN = 5;

    /* VkImageLayout */
    public const IMAGE_LAYOUT_UNDEFINED = 0;
    public const IMAGE_LAYOUT_GENERAL = 1;
    public const IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2;
    public const IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3;
    public const IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = 5;
    public const IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL = 6;
    public const IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL = 7;
    public const IMAGE_LAYOUT_PRESENT_SRC_KHR = 1000001002;

    /* VkImageUsageFlagBits */
    public const IMAGE_USAGE_TRANSFER_SRC = 0x00000001;
    public const IMAGE_USAGE_TRANSFER_DST = 0x00000002;
    public const IMAGE_USAGE_SAMPLED = 0x00000004;
    public const IMAGE_USAGE_STORAGE = 0x00000008;
    public const IMAGE_USAGE_COLOR_ATTACHMENT = 0x00000010;
    public const IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 0x00000020;

    /* VkCommandBufferUsageFlagBits */
    public const COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT = 0x00000001;
    public const COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE = 0x00000004;

    /* VkCommandPoolCreateFlagBits */
    public const COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER = 0x00000002;
    public const COMMAND_POOL_CREATE_TRANSIENT = 0x00000001;

    /* VkPipelineStageFlagBits */
    public const PIPELINE_STAGE_TOP_OF_PIPE = 0x00000001;
    public const PIPELINE_STAGE_VERTEX_INPUT = 0x00000004;
    public const PIPELINE_STAGE_VERTEX_SHADER = 0x00000008;
    public const PIPELINE_STAGE_FRAGMENT_SHADER = 0x00000080;
    public const PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT = 0x00000400;
    public const PIPELINE_STAGE_COMPUTE_SHADER = 0x00000800;
    public const PIPELINE_STAGE_TRANSFER = 0x00001000;
    public const PIPELINE_STAGE_BOTTOM_OF_PIPE = 0x00002000;
    public const PIPELINE_STAGE_ALL_COMMANDS = 0x00010000;

    /* VkAccessFlagBits */
    public const ACCESS_SHADER_READ = 0x00000020;
    public const ACCESS_SHADER_WRITE = 0x00000040;
    public const ACCESS_TRANSFER_READ = 0x00000800;
    public const ACCESS_TRANSFER_WRITE = 0x00001000;
    public const ACCESS_HOST_READ = 0x00002000;
    public const ACCESS_HOST_WRITE = 0x00004000;
    public const ACCESS_MEMORY_READ = 0x00008000;
    public const ACCESS_MEMORY_WRITE = 0x00010000;
    public const ACCESS_COLOR_ATTACHMENT_WRITE = 0x00000100;

    /* VkCullModeFlagBits */
    public const CULL_MODE_NONE = 0;
    public const CULL_MODE_FRONT = 0x00000001;
    public const CULL_MODE_BACK = 0x00000002;
    public const CULL_MODE_FRONT_AND_BACK = 0x00000003;

    /* VkFrontFace */
    public const FRONT_FACE_COUNTER_CLOCKWISE = 0;
    public const FRONT_FACE_CLOCKWISE = 1;

    /* VkIndexType */
    public const INDEX_TYPE_UINT16 = 0;
    public const INDEX_TYPE_UINT32 = 1;

    /* VkImageAspectFlagBits */
    public const IMAGE_ASPECT_COLOR = 0x00000001;
    public const IMAGE_ASPECT_DEPTH = 0x00000002;
    public const IMAGE_ASPECT_STENCIL = 0x00000004;

    /* VkFilter */
    public const FILTER_NEAREST = 0;
    public const FILTER_LINEAR = 1;

    /* VkSamplerAddressMode */
    public const SAMPLER_ADDRESS_MODE_REPEAT = 0;
    public const SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT = 1;
    public const SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE = 2;
    public const SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER = 3;

    /* VkAttachmentLoadOp */
    public const ATTACHMENT_LOAD_OP_LOAD = 0;
    public const ATTACHMENT_LOAD_OP_CLEAR = 1;
    public const ATTACHMENT_LOAD_OP_DONT_CARE = 2;

    /* VkAttachmentStoreOp */
    public const ATTACHMENT_STORE_OP_STORE = 0;
    public const ATTACHMENT_STORE_OP_DONT_CARE = 1;

    /* VkMemoryPropertyFlagBits */
    public const MEMORY_PROPERTY_DEVICE_LOCAL = 0x00000001;
    public const MEMORY_PROPERTY_HOST_VISIBLE = 0x00000002;
    public const MEMORY_PROPERTY_HOST_COHERENT = 0x00000004;
    public const MEMORY_PROPERTY_HOST_CACHED = 0x00000008;

    /* VkPresentModeKHR */
    public const PRESENT_MODE_IMMEDIATE = 0;
    public const PRESENT_MODE_MAILBOX = 1;
    public const PRESENT_MODE_FIFO = 2;
    public const PRESENT_MODE_FIFO_RELAXED = 3;

    /* VkQueryType */
    public const QUERY_TYPE_OCCLUSION = 0;
    public const QUERY_TYPE_PIPELINE_STATISTICS = 1;
    public const QUERY_TYPE_TIMESTAMP = 2;

    /* VkQueryResultFlagBits */
    public const QUERY_RESULT_64 = 0x00000001;
    public const QUERY_RESULT_WAIT = 0x00000002;
    public const QUERY_RESULT_WITH_AVAILABILITY = 0x00000004;

    /* VkStencilFaceFlagBits */
    public const STENCIL_FACE_FRONT = 0x00000001;
    public const STENCIL_FACE_BACK = 0x00000002;
    public const STENCIL_FACE_FRONT_AND_BACK = 0x00000003;

    /* VkDynamicState */
    public const DYNAMIC_STATE_VIEWPORT = 0;
    public const DYNAMIC_STATE_SCISSOR = 1;
    public const DYNAMIC_STATE_LINE_WIDTH = 2;
    public const DYNAMIC_STATE_DEPTH_BIAS = 3;
    public const DYNAMIC_STATE_BLEND_CONSTANTS = 4;
    public const DYNAMIC_STATE_DEPTH_BOUNDS = 5;
    public const DYNAMIC_STATE_STENCIL_COMPARE_MASK = 6;
    public const DYNAMIC_STATE_STENCIL_WRITE_MASK = 7;
    public const DYNAMIC_STATE_STENCIL_REFERENCE = 8;

    /* VkSampleCountFlagBits */
    public const SAMPLE_COUNT_1 = 0x00000001;
    public const SAMPLE_COUNT_2 = 0x00000002;
    public const SAMPLE_COUNT_4 = 0x00000004;
    public const SAMPLE_COUNT_8 = 0x00000008;
    public const SAMPLE_COUNT_16 = 0x00000010;

    /* VkSubpass */
    public const SUBPASS_EXTERNAL = 0xFFFFFFFF;

    /* VkVertexInputRate */
    public const VERTEX_INPUT_RATE_VERTEX = 0;
    public const VERTEX_INPUT_RATE_INSTANCE = 1;

    /* VkSharingMode */
    public const SHARING_MODE_EXCLUSIVE = 0;
    public const SHARING_MODE_CONCURRENT = 1;

    /* Vulkan API versions */
    public const API_VERSION_1_0 = 4194304;
    public const API_VERSION_1_1 = 4198400;
    public const API_VERSION_1_2 = 4202496;
    public const API_VERSION_1_3 = 4206592;
}

/**
 * Vulkan exception thrown on API errors.
 *
 * The exception code corresponds to the VkResult value.
 */
class VulkanException extends \RuntimeException {}

/**
 * Vulkan instance — entry point to the Vulkan API.
 */
final class Instance
{
    public function __construct(
        string $appName = 'PHPolygon',
        int $appVersion = 1,
        ?string $engineName = null,
        int $engineVersion = 0,
        ?int $apiVersion = null,
        bool $enableValidation = false,
        array $extensions = []
    ) {}

    /** @return PhysicalDevice[] */
    public function getPhysicalDevices(): array {}

    public function getVersion(): string {}

    /** @return array{name: string, specVersion: int}[] */
    public static function enumerateExtensions(): array {}

    /** @return array{name: string, description: string, specVersion: int, implementationVersion: int}[] */
    public static function enumerateLayers(): array {}
}

/**
 * Represents a physical GPU device.
 */
final class PhysicalDevice
{
    public function getName(): string {}
    public function getType(): int {}
    public function getTypeName(): string {}
    public function getApiVersion(): string {}
    public function getDriverVersion(): string {}

    /** @return array{index: int, queueCount: int, graphics: bool, compute: bool, transfer: bool, sparseBinding: bool, timestampValidBits: int}[] */
    public function getQueueFamilies(): array {}

    /** @return array{types: array, heaps: array} */
    public function getMemoryProperties(): array {}

    /** @return array{deviceName: string, deviceType: int, vendorID: int, deviceID: int, apiVersion: string, limits: array} */
    public function getProperties(): array {}

    /** @return array{linearTilingFeatures: int, optimalTilingFeatures: int, bufferFeatures: int} */
    public function getFormatProperties(int $format): array {}

    /** @return array{name: string, specVersion: int}[] */
    public function enumerateExtensions(): array {}

    public function getSurfaceSupport(int $queueFamilyIndex, Surface $surface): bool {}
}

/**
 * Logical device — interface to a physical device.
 */
final class Device
{
    /**
     * @param PhysicalDevice $physicalDevice
     * @param array $queueFamilies [['familyIndex' => int, 'count' => int, 'priorities' => float[]], ...]
     * @param string[] $extensions
     * @param array|null $features
     */
    public function __construct(
        PhysicalDevice $physicalDevice,
        array $queueFamilies,
        array $extensions = [],
        ?array $features = null
    ) {}

    public function getQueue(int $familyIndex, int $queueIndex = 0): Queue {}
    public function waitIdle(): void {}
}

/**
 * Device queue for command submission and presentation.
 */
final class Queue
{
    /**
     * @param CommandBuffer[] $commandBuffers
     * @param Semaphore[] $waitSemaphores
     * @param Semaphore[] $signalSemaphores
     */
    public function submit(
        array $commandBuffers,
        ?Fence $fence = null,
        array $waitSemaphores = [],
        array $signalSemaphores = []
    ): void {}

    /**
     * Present a swapchain image.
     *
     * @param Swapchain[] $swapchains
     * @param int[] $imageIndices
     * @param Semaphore[] $waitSemaphores
     * @return int VkResult
     */
    public function present(array $swapchains, array $imageIndices, array $waitSemaphores = []): int {}

    public function waitIdle(): void {}
    public function getFamilyIndex(): int {}
}

/**
 * GPU buffer.
 */
final class Buffer
{
    public function __construct(Device $device, int $size, int $usage, int $sharingMode = 0) {}

    /** @return array{size: int, alignment: int, memoryTypeBits: int} */
    public function getMemoryRequirements(): array {}

    public function getSize(): int {}
    public function bindMemory(DeviceMemory $memory, int $offset = 0): void {}
}

/**
 * Device memory allocation.
 */
final class DeviceMemory
{
    public function __construct(Device $device, int $size, int $memoryTypeIndex) {}
    public function map(int $offset = 0, ?int $size = null): void {}
    public function unmap(): void {}
    public function write(string $data, int $offset = 0): void {}
    public function read(int $size, int $offset = 0): string {}
    public function flush(int $offset = 0, ?int $size = null): void {}
    public function invalidate(int $offset = 0, ?int $size = null): void {}
}

/**
 * Command pool for allocating command buffers.
 */
final class CommandPool
{
    public function __construct(Device $device, int $queueFamilyIndex, int $flags = 0) {}

    /** @return CommandBuffer[] */
    public function allocateBuffers(int $count = 1, bool $primary = true): array {}

    public function reset(int $flags = 0): void {}
}

/**
 * Command buffer for recording GPU commands.
 */
final class CommandBuffer
{
    public function begin(int $flags = 0): void {}
    public function end(): void {}
    public function reset(int $flags = 0): void {}

    /* Pipeline binding */
    public function bindPipeline(int $bindPoint, Pipeline $pipeline): void {}
    /** @param DescriptorSet[] $sets */
    public function bindDescriptorSets(int $bindPoint, PipelineLayout $layout, int $firstSet, array $sets): void {}

    /* Draw commands */
    public function dispatch(int $x, int $y = 1, int $z = 1): void {}
    public function draw(int $vertexCount, int $instanceCount = 1, int $firstVertex = 0, int $firstInstance = 0): void {}
    public function drawIndexed(int $indexCount, int $instanceCount = 1, int $firstIndex = 0, int $vertexOffset = 0, int $firstInstance = 0): void {}

    /* Vertex/Index binding */
    /** @param Buffer[] $buffers */
    public function bindVertexBuffers(int $firstBinding, array $buffers, array $offsets = []): void {}
    public function bindIndexBuffer(Buffer $buffer, int $offset = 0, int $indexType = 1): void {}

    /* Transfer commands */
    public function copyBuffer(Buffer $src, Buffer $dst, int $size, int $srcOffset = 0, int $dstOffset = 0): void {}
    public function copyImageToBuffer(Image $image, int $imageLayout, Buffer $buffer, int $width, int $height, int $bufferOffset = 0, int $imageOffsetX = 0, int $imageOffsetY = 0, int $aspectMask = 1): void {}
    public function copyBufferToImage(Buffer $buffer, Image $image, int $imageLayout, int $width, int $height, int $bufferOffset = 0, int $imageOffsetX = 0, int $imageOffsetY = 0, int $aspectMask = 1): void {}
    public function copyImage(Image $src, int $srcLayout, Image $dst, int $dstLayout, int $width, int $height, int $srcX = 0, int $srcY = 0, int $dstX = 0, int $dstY = 0): void {}
    public function blitImage(Image $srcImage, int $srcLayout, Image $dstImage, int $dstLayout, int $srcX0, int $srcY0, int $srcX1, int $srcY1, int $dstX0, int $dstY0, int $dstX1, int $dstY1, int $filter = 1): void {}
    public function fillBuffer(Buffer $buffer, int $data, int $offset = 0, ?int $size = null): void {}
    public function updateBuffer(Buffer $buffer, string $data, int $offset = 0): void {}

    /* Clear commands */
    public function clearColorImage(Image $image, int $layout, float $r = 0.0, float $g = 0.0, float $b = 0.0, float $a = 1.0): void {}
    public function clearDepthStencilImage(Image $image, int $layout, float $depth = 1.0, int $stencil = 0): void {}

    /* Render pass */
    public function beginRenderPass(RenderPass $renderPass, Framebuffer $framebuffer, int $x, int $y, int $width, int $height, array $clearValues = []): void {}
    public function endRenderPass(): void {}

    /* Dynamic state */
    public function setViewport(float $x, float $y, float $width, float $height, float $minDepth = 0.0, float $maxDepth = 1.0): void {}
    public function setScissor(int $x, int $y, int $width, int $height): void {}
    public function setLineWidth(float $lineWidth): void {}
    public function setDepthBias(float $constantFactor, float $clamp, float $slopeFactor): void {}
    public function setBlendConstants(float $r, float $g, float $b, float $a): void {}
    public function setDepthBounds(float $min, float $max): void {}
    public function setStencilCompareMask(int $faceMask, int $compareMask): void {}
    public function setStencilWriteMask(int $faceMask, int $writeMask): void {}
    public function setStencilReference(int $faceMask, int $reference): void {}

    /* Synchronization */
    public function pipelineBarrier(int $srcStage, int $dstStage, int $srcAccess, int $dstAccess): void {}
    public function imageMemoryBarrier(Image $image, int $oldLayout, int $newLayout, int $srcAccessMask, int $dstAccessMask, int $srcStage, int $dstStage, int $aspectMask = 1): void {}
    public function pushConstants(PipelineLayout $layout, int $stageFlags, int $offset, string $data): void {}

    /* Secondary command buffers */
    /** @param CommandBuffer[] $commandBuffers */
    public function executeCommands(array $commandBuffers): void {}

    /* Query commands */
    public function beginQuery(QueryPool $queryPool, int $query, int $flags = 0): void {}
    public function endQuery(QueryPool $queryPool, int $query): void {}
    public function writeTimestamp(int $pipelineStage, QueryPool $queryPool, int $query): void {}
    public function resetQueryPool(QueryPool $queryPool, int $firstQuery, int $queryCount): void {}

    /* Debug labels (VK_EXT_debug_utils) */
    public function beginDebugLabel(string $name, float $r = 1.0, float $g = 1.0, float $b = 1.0, float $a = 1.0): void {}
    public function endDebugLabel(): void {}
    public function insertDebugLabel(string $name, float $r = 1.0, float $g = 1.0, float $b = 1.0, float $a = 1.0): void {}
}

/**
 * SPIR-V shader module.
 */
final class ShaderModule
{
    /** @param string $spirvCode Raw SPIR-V bytecode (must be multiple of 4 bytes) */
    public function __construct(Device $device, string $spirvCode) {}

    public static function createFromFile(Device $device, string $path): static {}
}

/**
 * Descriptor set layout — defines the binding structure.
 */
final class DescriptorSetLayout
{
    /**
     * @param array $bindings [['binding' => int, 'type' => int, 'count' => int, 'stageFlags' => int], ...]
     */
    public function __construct(Device $device, array $bindings) {}
}

/**
 * Descriptor pool for allocating descriptor sets.
 */
final class DescriptorPool
{
    /**
     * @param array $poolSizes [['type' => int, 'count' => int], ...]
     */
    public function __construct(Device $device, int $maxSets, array $poolSizes, int $flags = 0) {}

    /** @return DescriptorSet[] */
    public function allocateSets(array $layouts): array {}
}

/**
 * Descriptor set — bound resources for shaders.
 */
final class DescriptorSet
{
    public function writeBuffer(int $binding, Buffer $buffer, int $offset = 0, ?int $range = null, int $type = 7): void {}
    public function writeImage(int $binding, ImageView $imageView, Sampler $sampler, int $imageLayout = 5, int $type = 1): void {}
}

/**
 * Pipeline layout — descriptor set layouts + push constant ranges.
 */
final class PipelineLayout
{
    /**
     * @param DescriptorSetLayout[] $setLayouts
     * @param array $pushConstantRanges [['stageFlags' => int, 'offset' => int, 'size' => int], ...]
     */
    public function __construct(Device $device, array $setLayouts = [], array $pushConstantRanges = []) {}
}

/**
 * Compute or graphics pipeline.
 */
final class Pipeline
{
    public static function createCompute(
        Device $device,
        PipelineLayout $layout,
        ShaderModule $shader,
        string $entryPoint = 'main'
    ): static {}

    /**
     * @param array $config Keys: layout, renderPass, vertexShader, fragmentShader,
     *     vertexEntryPoint, fragmentEntryPoint, topology, cullMode, frontFace,
     *     depthTest, depthWrite, vertexBindings, vertexAttributes, dynamicStates
     */
    public static function createGraphics(Device $device, array $config): static {}
}

/**
 * Pipeline cache for shader compilation caching.
 */
final class PipelineCache
{
    public function __construct(Device $device, ?string $initialData = null) {}

    /** Get the binary cache data (can be saved to disk and reloaded). */
    public function getData(): string {}

    /** @param PipelineCache[] $srcCaches */
    public function merge(array $srcCaches): void {}
}

/**
 * Render pass — defines attachments, subpasses, dependencies.
 */
final class RenderPass
{
    /**
     * @param array $attachments [['format', 'samples', 'loadOp', 'storeOp', ...], ...]
     * @param array $subpasses [['bindPoint', 'colorAttachments' => [...], 'depthAttachment' => [...]], ...]
     * @param array $dependencies [['srcSubpass', 'dstSubpass', 'srcStageMask', ...], ...]
     */
    public function __construct(Device $device, array $attachments, array $subpasses, array $dependencies = []) {}
}

/**
 * Framebuffer — render target attachments.
 */
final class Framebuffer
{
    /** @param ImageView[] $attachments */
    public function __construct(Device $device, RenderPass $renderPass, array $attachments, int $width, int $height, int $layers = 1) {}
}

/**
 * GPU image (texture/render target).
 */
final class Image
{
    public function __construct(Device $device, int $width, int $height, int $format, int $usage, int $tiling = 0, int $samples = 1) {}

    /** @return array{size: int, alignment: int, memoryTypeBits: int} */
    public function getMemoryRequirements(): array {}

    public function bindMemory(DeviceMemory $memory, int $offset = 0): void {}
}

/**
 * Image view — a specific view into an image.
 */
final class ImageView
{
    public function __construct(Device $device, Image $image, int $format, int $aspectMask = 1, int $viewType = 1) {}
}

/**
 * Texture sampler.
 */
final class Sampler
{
    /**
     * @param array $config Keys: magFilter, minFilter, addressModeU/V/W, anisotropy, maxAnisotropy
     */
    public function __construct(Device $device, array $config = []) {}
}

/**
 * GPU fence for CPU-GPU synchronization.
 */
final class Fence
{
    public function __construct(Device $device, bool $signaled = false) {}

    /** @return bool true if signaled, false if timeout */
    public function wait(int $timeout = -1): bool {}

    public function reset(): void {}
    public function isSignaled(): bool {}
}

/**
 * GPU semaphore for GPU-GPU synchronization.
 */
final class Semaphore
{
    public function __construct(Device $device) {}
}

/**
 * Swapchain for presenting rendered frames.
 */
final class Swapchain
{
    /**
     * @param array $config Keys: minImageCount, format, colorSpace, width, height,
     *     imageUsage, presentMode, compositeAlpha, clipped, preTransform, imageArrayLayers
     */
    public function __construct(Device $device, Surface $surface, array $config) {}

    /** @return Image[] */
    public function getImages(): array {}

    public function acquireNextImage(Semaphore $semaphore, ?Fence $fence = null, int $timeout = -1): int {}
}

/**
 * Window surface for rendering.
 */
final class Surface
{
    /** @return array{minImageCount: int, maxImageCount: int, currentWidth: int, currentHeight: int, ...} */
    public function getCapabilities(PhysicalDevice $physicalDevice): array {}

    /** @return array{format: int, colorSpace: int}[] */
    public function getFormats(PhysicalDevice $physicalDevice): array {}

    /** @return int[] VkPresentModeKHR values */
    public function getPresentModes(PhysicalDevice $physicalDevice): array {}
}

/**
 * Query pool for GPU profiling (timestamps, occlusion, pipeline statistics).
 */
final class QueryPool
{
    public function __construct(Device $device, int $queryType, int $queryCount, int $pipelineStatistics = 0) {}

    /** @return string Raw binary result data (use unpack() to decode) */
    public function getResults(int $firstQuery, int $queryCount, int $stride = 8, int $flags = 0): string {}

    public function reset(int $firstQuery, int $queryCount): void {}
}

/**
 * GPU event for fine-grained synchronization.
 */
final class Event
{
    public function __construct(Device $device) {}
    public function set(): void {}
    public function reset(): void {}
    public function isSignaled(): bool {}
}
