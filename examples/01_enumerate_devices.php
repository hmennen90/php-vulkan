<?php
/**
 * Example 01: Enumerate GPU Devices
 *
 * This is the simplest php-vulkan example. It creates a Vulkan instance
 * and lists all available physical devices with their properties.
 */

$instance = new Vk\Instance(appName: 'PHPolygon DeviceInfo');

echo "Vulkan API Version: " . $instance->getVersion() . "\n\n";

$devices = $instance->getPhysicalDevices();

if (empty($devices)) {
    echo "No Vulkan-capable devices found!\n";
    exit(1);
}

echo "Found " . count($devices) . " device(s):\n\n";

foreach ($devices as $i => $device) {
    echo "--- Device #$i ---\n";
    echo "  Name:           " . $device->getName() . "\n";
    echo "  Type:           " . $device->getTypeName() . "\n";
    echo "  API Version:    " . $device->getApiVersion() . "\n";
    echo "  Driver Version: " . $device->getDriverVersion() . "\n";

    $props = $device->getProperties();
    echo "  Vendor ID:      0x" . dechex($props['vendorID']) . "\n";
    echo "  Device ID:      0x" . dechex($props['deviceID']) . "\n";

    echo "\n  Limits:\n";
    foreach ($props['limits'] as $key => $value) {
        echo "    $key: $value\n";
    }

    echo "\n  Queue Families:\n";
    foreach ($device->getQueueFamilies() as $family) {
        $caps = [];
        if ($family['graphics']) $caps[] = 'Graphics';
        if ($family['compute']) $caps[] = 'Compute';
        if ($family['transfer']) $caps[] = 'Transfer';
        echo "    Family #{$family['index']}: " . implode(', ', $caps)
            . " (queues: {$family['queueCount']})\n";
    }

    echo "\n  Memory:\n";
    $mem = $device->getMemoryProperties();
    foreach ($mem['heaps'] as $hi => $heap) {
        $sizeMB = round($heap['size'] / 1024 / 1024);
        $local = $heap['deviceLocal'] ? ' (device local)' : '';
        echo "    Heap #$hi: {$sizeMB} MB{$local}\n";
    }

    echo "\n";
}
