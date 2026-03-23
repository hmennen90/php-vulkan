--TEST--
CommandBuffer has all expected methods
--FILE--
<?php
$rc = new ReflectionClass('Vk\CommandBuffer');
$expected = [
    'begin', 'end', 'reset',
    'bindPipeline', 'bindDescriptorSets',
    'dispatch', 'draw', 'drawIndexed',
    'bindVertexBuffers', 'bindIndexBuffer',
    'copyBuffer', 'copyImageToBuffer', 'copyBufferToImage', 'copyImage',
    'blitImage', 'fillBuffer', 'updateBuffer',
    'clearColorImage', 'clearDepthStencilImage',
    'beginRenderPass', 'endRenderPass',
    'setViewport', 'setScissor', 'setLineWidth', 'setDepthBias',
    'setBlendConstants', 'setDepthBounds',
    'setStencilCompareMask', 'setStencilWriteMask', 'setStencilReference',
    'pipelineBarrier', 'imageMemoryBarrier', 'pushConstants',
    'executeCommands',
    'beginQuery', 'endQuery', 'writeTimestamp', 'resetQueryPool',
    'beginDebugLabel', 'endDebugLabel', 'insertDebugLabel',
];

$missing = [];
foreach ($expected as $m) {
    if (!$rc->hasMethod($m)) $missing[] = $m;
}

if (empty($missing)) {
    echo "All " . count($expected) . " methods present\n";
} else {
    echo "Missing: " . implode(', ', $missing) . "\n";
}
?>
--EXPECT--
All 41 methods present
