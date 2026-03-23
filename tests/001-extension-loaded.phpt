--TEST--
Check vulkan extension is loaded
--FILE--
<?php
var_dump(extension_loaded('vulkan'));
var_dump(phpversion('vulkan'));
?>
--EXPECT--
bool(true)
string(5) "0.5.0"
