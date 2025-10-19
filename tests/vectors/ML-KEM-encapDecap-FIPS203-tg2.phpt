--TEST--
Execute AVCP_Vector ML-KEM-encapDecap-FIPS203-tg2
--FILE--
<?php declare(strict_types=1);

include __DIR__ . '/common.inc';

$vector = \ACVPTestVector::parseFile(__DIR__ . '/ML-KEM-encapDecap-FIPS203-tg2.json');
$vector->runAllTestGroups();

echo "OK\n";
?>
--EXPECT--
OK
