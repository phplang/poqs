--TEST--
Execute AVCP_Vector SLH-DSA-sigVer-FIPS205-tg27
--FILE--
<?php declare(strict_types=1);

include __DIR__ . '/common.inc';

$vector = \ACVPTestVector::parseFile(__DIR__ . '/SLH-DSA-sigVer-FIPS205-tg27.json');
$vector->runAllTestGroups();

echo "OK\n";
?>
--EXPECT--
OK
