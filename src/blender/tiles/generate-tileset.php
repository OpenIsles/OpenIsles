<?php
/**
 * Wird vom Makefile benutzt, um aus der config/tiles.xml die Tile-Grafiken auszulesen 
 * und daraus das Tileset (tiled-TSX und eine Komplettgrafik für das Spiel) mittels ImageMagick generieren.
 *
 * Die config/tiles.xml muss die tmx-tile-indexes von 1 auf aufsteigend haben, sonst wird dieses Script
 * einen Fehler melden.
 */

$xmlFile = 'data/config/tiles.xml';

$xml = simplexml_load_file($xmlFile);
$tiles = $xml->tiles;

$sizeX = $sizeY = 0;
$tmxTileIndexShouldBe = 1;
$cmdArgs = array();
foreach($tiles->tile as $tile) {
    foreach($tile->{'tmx-tile'} as $tmxTile) {
	    if ($tmxTile['tmx-tile-index'] != $tmxTileIndexShouldBe) {
	        fprintf(STDERR, "Tile index should be $tmxTileIndexShouldBe\n");
	        exit(1); // Fehlercode -> triggert Abbruch im Makefile
	    }

        $x = $tmxTile['x'];
        $y = $tmxTile['y'];
        
        switch($tmxTile['view']) {
            case 'south':
                $tileFileSuffix = 'angle0.png';
                break;
            case 'east':
                $tileFileSuffix = 'angle90.png';
                break;
            case 'north':
                $tileFileSuffix = 'angle180.png';
                break;
            case 'west':
                $tileFileSuffix = 'angle270.png';
                break;
            default:
                fprintf(STDERR, "Illegal view {$tmxTile['view']}\n");
                exit(1); // Fehlercode -> triggert Abbruch im Makefile
        }
        $tileFile = "src/blender/tiles/render/{$tile['name']}-$tileFileSuffix";

        $cmdArgs[] = $tileFile;

        $cmdArgs[] = '-geometry';
        $cmdArgs[] = "+$x+$y";

        $cmdArgs[] = '-composite';

        if ($x + 64 > $sizeX) {
            $sizeX = $x + 64;
        }
        if ($y + 64 > $sizeY) {
            $sizeY = $y + 64;
        }


	    $tmxTileIndexShouldBe++;
    }
}

$cmd = "convert -size ${sizeX}x${sizeY} xc:none ".implode(' ', $cmdArgs)." src/blender/tiles/render/tileset.png";
exec($cmd, $output, $returnCode);

exit($returnCode); // Return-Code durchschleifen, dass das Makefile einen Fehler von convert auch als Fehler nimmt

?>
