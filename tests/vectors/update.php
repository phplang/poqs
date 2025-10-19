<?php declare(strict_types=1);

/**
 * Grabs ACVP test vectors from liboqs source (MIT licensed).
 * Chucks these test vectors by testGroup to help with parallelization.
 * Generates a .phpt stub for each vector from .../phpt.tmpl
 */

if (!isset($_SERVER['argv'][1])) {
	fwrite(STDERR, "Usage: {$_SERVER['argv'][0]} <path-to-liboqs>\n");
	exit(1);
}

$vectors = "{$_SERVER['argv'][1]}/tests/ACVP_Vectors";
if (!is_dir($vectors)) {
	fwrite(STDERR, "liboqs test vectors not found in $vectors\n");
	exit(1);
}

foreach (scandir($vectors) as $file) {
	if ($file[0] === '.') continue;
	if (!is_dir("$vectors/$file")) continue;
	$source = "$vectors/$file/internalProjection.json";
	if (!file_exists($source)) continue;
	foreach (['keyGen', 'sigGen', 'sigVer', 'encapDecap'] as $mode) {
		if (strpos($file, "-{$mode}-") !== false) {
			processFile($source, $file);
			break;
		}
	}
}

function processFile(string $source, string $name): void {
	$json = json_decode(file_get_contents($source));
	foreach ($json->testGroups as $testGroup) {
		$group = clone $json;
		$group->testGroups = [$testGroup];
		$id = intval($testGroup->tgId);
		$groupname = "$name-tg{$id}";
		$testbase = __DIR__ . '/' . $groupname;

		file_put_contents("$testbase.json", json_encode($group, JSON_PRETTY_PRINT));

		file_put_contents("$testbase.phpt",
			str_replace('__VECTOR__', $groupname,
				file_get_contents(__DIR__ . '/phpt.tmpl')));
	}
}

