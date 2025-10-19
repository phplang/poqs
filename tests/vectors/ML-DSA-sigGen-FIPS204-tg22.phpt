--TEST--
Execute AVCP_Vector ML-DSA-sigGen-FIPS204-tg22
--FILE--
<?php declare(strict_types=1);

include __DIR__ . '/common.inc';

$vector = \ACVPTestVector::parseFile(__DIR__ . '/ML-DSA-sigGen-FIPS204-tg22.json');
$vector->runAllTestGroups();

echo "OK\n";
?>
--EXPECT--
OK
