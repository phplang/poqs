--TEST--
Execute AVCP_Vector SLH-DSA-keyGen-FIPS205-tg1
--FILE--
<?php declare(strict_types=1);

include __DIR__ . '/common.inc';

$vector = \ACVPTestVector::parseFile(__DIR__ . '/SLH-DSA-keyGen-FIPS205-tg1.json');
$vector->runAllTestGroups();

echo "OK\n";
?>
--EXPECT--
OK
