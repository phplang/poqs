--TEST--
Execute AVCP_Vector SLH-DSA-sigGen-FIPS205-tg40
--FILE--
<?php declare(strict_types=1);

include __DIR__ . '/common.inc';

$vector = \ACVPTestVector::parseFile(__DIR__ . '/SLH-DSA-sigGen-FIPS205-tg40.json');
$vector->runAllTestGroups();

echo "OK\n";
?>
--EXPECT--
OK
