<?php
/**
 * Generiert die Grafiken für die Buttons, die zum Bauen von Gebäuden sind.
 */

// Pfadeinstellungen (jeweils ohne Slash am Ende)
$tilesetInputFile = '../../data/img/tileset.png';
$buildingInputDir = '../../data/img/buildings';
$outputDir = '../../data/img/gui';

// Array von Buttons, die generiert werden sollen. Dateiname jeweils ohne ".png" am Ende
// "Gebäude-geometry crop" ist zum Ausschneiden der richtigen Ansicht
// "Gebäude-geometry placement" sind die Koordinaten, wo das Gebäude auf den Button platziert wird
// "Tile-geometry" sind die Koordinaten der Kachel in der tileset.png
$buttons = array(
    //     Tile-geometry   Gebäudegrafik  Gebäude-geometry Gebäude-geometry Ausgabegrafik
    //                                    crop             placement
    array('64x32+0+12'  , 'toolsmiths' , '128x96+0+0'   , '44x+2+14'    , 'button-add-building-craftsman'),
    array('64x32+0+12'  , 'marketplace', '224x160+0+0'  , '44x+2+18'    , 'button-add-building-public'),
    array('64x32+0+12'  , 'foresters'  , '128x96+0+0'   , '44x+2+13'    , 'button-add-building-farm'),
    array('64x32+256+96', 'office1'    , '160x112+0+0'  , '44x+2+16'    , 'button-add-building-port'),
);


foreach($buttons as $button) {
    $shellCmd = <<<SHELLCMD
    convert -size 52x64 canvas:transparent -filter box \
      \( $tilesetInputFile -crop {$button[0]} \) -geometry 44x24+2+32 -composite \
      \( $buildingInputDir/{$button[1]}.png -crop {$button[2]} \) -geometry {$button[3]} -composite \
      temp_base.png

    convert temp_base.png \
      \( +clone -geometry +4+8 +level-colors \#5a5a5a -channel A -fx "a=a*0.5" \) -compose DstOver -composite \
      $outputDir/{$button[4]}.png

    convert temp_base.png \
      \( -clone 0 -channel A -blur 0x0.75 -level 0,1% +channel +level-colors \#caad14 \) -compose DstOver -composite \
      temp_base.png \( +clone -geometry +4+8 +level-colors \#5a5a5a -channel A -fx "a=a*0.5" \) \
      -delete 1 -compose DstOver -composite \
      $outputDir/{$button[4]}-pressed.png

    rm temp_base.png
SHELLCMD;

    echo "Generating {$button[4]}...";
    system($shellCmd);
    echo " OK\n";
}

?>