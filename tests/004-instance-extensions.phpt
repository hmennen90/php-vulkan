--TEST--
Enumerate instance extensions (static, no GPU needed)
--FILE--
<?php
$exts = \Vk\Instance::enumerateExtensions();
echo "type: " . gettype($exts) . "\n";
echo "count > 0: " . (count($exts) > 0 ? 'yes' : 'no') . "\n";
if (count($exts) > 0) {
    echo "has name: " . (isset($exts[0]['name']) ? 'yes' : 'no') . "\n";
    echo "has specVersion: " . (isset($exts[0]['specVersion']) ? 'yes' : 'no') . "\n";
}
?>
--EXPECT--
type: array
count > 0: yes
has name: yes
has specVersion: yes
