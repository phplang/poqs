--TEST--
Execute AVCP_Vector ML-KEM-keyGen-FIPS203-tg3
--FILE--
<?php declare(strict_types=1);

include __DIR__ . '/common.inc';

$vector = \ACVPTestVector::parseFile(__DIR__ . '/ML-KEM-keyGen-FIPS203-tg3.json');
$vector->runAllTestGroups();

echo "OK\n";
?>
--EXPECT--
OK
