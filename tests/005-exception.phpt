--TEST--
VulkanException is throwable
--FILE--
<?php
$e = new \Vk\VulkanException('test error', 42);
echo get_class($e) . "\n";
echo $e->getMessage() . "\n";
echo $e->getCode() . "\n";
echo ($e instanceof \RuntimeException) ? "is RuntimeException\n" : "not RuntimeException\n";
?>
--EXPECT--
Vk\VulkanException
test error
42
is RuntimeException
