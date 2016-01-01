<?php

/**
 * Hilfsscript, dass sprachabhängige Phrasen in den XML-Dateien im Data-Verzeichnis findet und diese
 * als .pot-Datei zur Verfügung stellt. CMake verarbeitet diese Datei dann weiter.
 *
 * Zwei Parameter werden erwartet:
 * - Das data-Verzeichnis
 * - Die Zieldatei
 */

$dataDir = $_SERVER['argv'][1];
$outputFile = $_SERVER['argv'][2];
$msgIds = array();

//// msgIds sammeln ////////////////////////////////////////////////////////////////////////////////////////////////////

// Güter

$goodsXml = simplexml_load_file("$dataDir/config/goods.xml");
$nodes = $goodsXml->xpath('/goods/good/@name');

foreach ($nodes as $node) {
	$msgIds[] = array(
		'file' => 'data/config/goods.xml',
		'msgid' => "good|$node"
	);
}

// Map-Objekte

$mapObjectsXml = simplexml_load_file("$dataDir/config/map-objects.xml");
$nodes = $mapObjectsXml->xpath('/map-objects/*/@name');

foreach ($nodes as $node) {
	$msgIds[] = array(
		'file' => 'data/config/map-objects.xml',
		'msgid' => "mapObjectType|$node"
	);
}

// Bevölkerungsgruppen

$populationTiersXml = simplexml_load_file("$dataDir/config/population-tiers.xml");
$nodes = $populationTiersXml->xpath('/population-tiers/population-tier/@name');

foreach ($nodes as $node) {
	$msgIds[] = array(
		'file' => 'data/config/population-tiers.xml',
		'msgid' => "populationTier|$node|singular",
		'msgid_plural' => "populationTier|$node|plural"
	);
}


//// .pot-File zusammenbauen ///////////////////////////////////////////////////////////////////////////////////////////

$potFile = '';
foreach($msgIds as $msgId) {
	$potFile .= "\n";
	$potFile .= "# automatically generated out of data xmls\n";
	$potFile .= "#: ${msgId['file']}\n";
	$potFile .= "msgid \"{$msgId['msgid']}\"\n";

	if (isset($msgId['msgid_plural'])) {
	    $potFile .= "msgid_plural \"{$msgId['msgid_plural']}\"\n";
	    $potFile .= "msgstr[0] \"\"\n";
	    $potFile .= "msgstr[1] \"\"\n";
	}
	else {
	    $potFile .= "msgstr \"\"\n";
	}
}

file_put_contents($outputFile, $potFile);

?>