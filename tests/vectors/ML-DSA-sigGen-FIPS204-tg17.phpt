--TEST--
Execute AVCP_Vector ML-DSA-sigGen-FIPS204-tg17
--FILE--
<?php declare(strict_types=1);

include __DIR__ . '/common.inc';

$vector = \ACVPTestVector::parseFile(__DIR__ . '/ML-DSA-sigGen-FIPS204-tg17.json');
$vector->runAllTestGroups();

echo "OK\n";
?>
--EXPECT--
OK
