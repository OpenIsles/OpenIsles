<?php
/**
 * Wird vom Makefile benutzt, um aus der config/tiles.xml die Tile-Grafiken in der richtigen Reihenfolge
 * an make zurückzuliefern, das sie dann in montage benutzt.
 *
 * Das Script prüft zusätzlich, ob die IDs in der XML-Datei auch korrekt sind, da diese von 1 an ohne Lücken
 * durchnummeriert sein müssen, um mit Tiled richtig zusammenzuarbeiten.
 */

$xmlFile = 'data/config/tiles.xml';

$xml = simplexml_load_file($xmlFile);
$tiles = $xml->tiles;

$indexShouldBe = 1;
foreach($tiles->tile as $tile) {
	if ($tile['tile-index'] != $indexShouldBe) {
	  fprintf(STDERR, "Tile index should be $indexShouldBe\n");
	  exit(1); // Fehlercode
	}

	echo $tile['graphic-file']."\n";
	$indexShouldBe++;
}
exit(0); // OK

?>