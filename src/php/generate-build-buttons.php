<?php
/**
 * Generiert die Grafiken für die Buttons, die zum Bauen von Gebäuden sind.
 */

// Pfadeinstellungen (jeweils ohne Slash am Ende)
$tileInputDir = '../../data/img/tiles';
$buildingInputDir = '../../data/img/objects';
$outputDir = '../../data/img/gui';

// Array von Buttons, die generiert werden sollen. Dateiname jeweils ohne ".png" am Ende
// Gebäude-Geometry berechnet sich gewöhnlich aus: "40x"+(HöheBild/BreiteBild*40)+"+4+"+(47-(BreiteBild/HöheBild*40))
$buttons = array(
    //     Tilegrafik   Gebäudegrafik  Gebäude-geometry Ausgabegrafik
    array('grass'    , 'weaponsmith', '40x40+4+7'    , 'button-add-building-craftsman'),
    array('grass'    , 'marketplace', '40x29+4+18'   , 'button-add-building-public'),
    array('grass'    , 'foresters'  , '40x25+4+22'   , 'button-add-building-farm'),
    array('water'    , 'office1'    , '40x28+4+19'   , 'button-add-building-port'),
);

foreach($buttons as $button) {
    $shellCmd = <<<SHELLCMD
    convert -size 52x64 canvas:transparent -filter box \
      $tileInputDir/{$button[0]}.png -geometry 44x24+2+32 -composite \
      $buildingInputDir/{$button[1]}.png -geometry {$button[2]} -composite \
      temp_base.png

    convert temp_base.png \
      \( +clone -geometry +4+8 +level-colors \#5a5a5a -channel A -fx "a=a*0.5" \) -compose DstOver -composite \
      $outputDir/{$button[3]}.png

    convert temp_base.png \
      \( -clone 0 -channel A -blur 0x0.75 -level 0,1% +channel +level-colors \#caad14 \) -compose DstOver -composite \
      temp_base.png \( +clone -geometry +4+8 +level-colors \#5a5a5a -channel A -fx "a=a*0.5" \) \
      -delete 1 -compose DstOver -composite \
      $outputDir/{$button[3]}-pressed.png

    rm temp_base.png
SHELLCMD;

    echo "Generating {$button[3]}...";
    system($shellCmd);
    echo " OK\n";
}

?>