--TEST--
Execute AVCP_Vector SLH-DSA-sigGen-FIPS205-tg54
--FILE--
<?php declare(strict_types=1);

include __DIR__ . '/common.inc';

$vector = \ACVPTestVector::parseFile(__DIR__ . '/SLH-DSA-sigGen-FIPS205-tg54.json');
$vector->runAllTestGroups();

echo "OK\n";
?>
--EXPECT--
OK
