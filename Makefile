SRC_DIRECTORY := src
DATA_DIRECTORY := data

# Zielverzeichnis, in dem die fertige Anwendung kompiliert wurde
# @see https://youtrack.jetbrains.com/issue/CPP-3374
TARGET_DIRECTORY := ~/.clion/system/cmake/generated/OpenIsles-1b82d0b6/1b82d0b6/Debug
TARGET_EXECUTABLE := $(TARGET_DIRECTORY)/OpenIsles

# Für das fertige Tileset für Tiled: Anzahl Kacheln pro Zeile
TILESET_WIDTH := 4

CREATE_TARGET_DIRECTORY = mkdir -p $(@D)

MONTAGE := montage -background transparent
BLENDER := /opt/blender-2.74/blender

.PHONY: all clean build-gui clean-gui render-sheep render-cart render-build-menu \
        render-coat-of-arms render-coin render-public-building-icons render-blender clean-blender \
        build-add-building-gui

all: build-gui render-blender

clean: clean-gui clean-blender


########################################################################################################################
# Funktion, um transparente Ränder in Tiles zu fixen.                                                                  #
# Da wir Blender's Anti-Aliasing wollen, sind die Ränder der Kacheln teildurchsichtig, was zu Lücken beim              #
# Zusammensetzen führt. Deshalb stacken wir eine solche Grafik einfach 4x auf sich selber.                             #
#                                                                                                                      #
# Wichtig: Wir machen das nicht 8x (=log2(256)), sonst würde aus einem Alpha-Wert 1 ein 255 werden. Diese Pixel sind   #
# aber nervig, weil sie auch noch die falsche Farbe haben. Wir nehmen nur Pixel, die schon einigermaßen sichtbar sind. #
########################################################################################################################

FIX_TILE_TRANSPARENCY = \
	for i in `seq 1 4`; \
	do \
		convert $1 $1 -composite $1; \
	done

########################################################################################################################
# Gelände-Kacheln                                                                                                      #
########################################################################################################################

$(DATA_DIRECTORY)/img/tileset.png: $(SRC_DIRECTORY)/blender/tiles/tiles.blend
	cd $(SRC_DIRECTORY)/blender/tiles; $(BLENDER) -b $(notdir $<) -P render.py
	php $(SRC_DIRECTORY)/blender/tiles/generate-tileset.php
	$(call FIX_TILE_TRANSPARENCY,$(SRC_DIRECTORY)/blender/tiles/render/tileset.png)

	cp $(SRC_DIRECTORY)/blender/tiles/render/tileset.png $(DATA_DIRECTORY)/img/tileset.png

########################################################################################################################
# GUI                                                                                                                  #
########################################################################################################################
	
clean-gui:
	rm -f $(DATA_DIRECTORY)/img/gui/panel.png
	rm -f $(DATA_DIRECTORY)/img/gui/statusbar.png
	rm -f $(DATA_DIRECTORY)/img/gui/panel-header.png
	rm -f $(DATA_DIRECTORY)/img/gui/minimap.png \
	rm -f $(DATA_DIRECTORY)/img/gui/map-rotate.png \
	rm -f $(DATA_DIRECTORY)/img/gui/map-zoom.png \
	rm -rf $(DATA_DIRECTORY)/img/gui/add-building
	
build-gui: \
	$(DATA_DIRECTORY)/img/gui/panel.png \
	$(DATA_DIRECTORY)/img/gui/statusbar.png \
	$(DATA_DIRECTORY)/img/gui/panel-header.png \
	$(DATA_DIRECTORY)/img/gui/minimap.png \
	$(DATA_DIRECTORY)/img/gui/map-rotate.png \
	$(DATA_DIRECTORY)/img/gui/map-zoom.png \
	$(DATA_DIRECTORY)/img/gui/add-building/panel-top.png \
	$(DATA_DIRECTORY)/img/gui/add-building/panel.png \
	$(DATA_DIRECTORY)/img/gui/add-building/panel-bottom.png \
	build-add-building-gui

$(DATA_DIRECTORY)/img/gui/panel.png: $(SRC_DIRECTORY)/psd/marble-panels.psd
	$(CREATE_TARGET_DIRECTORY)
	convert $< -crop 256x768+18+12 +repage -flatten \
		\( \
		  -size 256x768 xc:none -stroke none \
		  -fill rgba\(255,255,255,0.7\) \
		  -draw "polyline 0,0 255,0 252,3 3,3" \
		  -draw "polyline 0,0 3,3 3,764 0,767" \
		  -fill rgba\(0,0,0,0.6\) \
		  -draw "polyline 0,767 255,767 252,764 3,764" \
		  -draw "polyline 255,0 252,3 252,764 255,767" \
		  -fill none -stroke rgba\(255,255,255,0.85\) -strokewidth 0.5 \
		  -draw "polyline 4,4 251,4 251,763 4,763 4,4" \
		\) -compose softlight -composite $@

$(DATA_DIRECTORY)/img/gui/statusbar.png: $(SRC_DIRECTORY)/psd/marble-panels.psd
	$(CREATE_TARGET_DIRECTORY)
	convert $< -rotate 90 -crop 768x34+18+214 +repage -flatten \
		\( \
		  -size 768x34 xc:none -stroke none \
		  -fill rgba\(255,255,255,0.7\) \
		  -draw "polyline 0,0 767,0 764,3 3,3" \
		  -draw "polyline 0,0 3,3 3,30 0,33" \
		  -fill rgba\(0,0,0,0.6\) \
		  -draw "polyline 0,33 767,33 764,30 3,30" \
		  -draw "polyline 767,0 764,3 764,30 767,33" \
		  -fill none -stroke rgba\(255,255,255,0.85\) -strokewidth 0.5 \
		  -draw "polyline 4,4 763,4 763,29 4,29 4,4" \
		\) -compose softlight -composite $@

$(DATA_DIRECTORY)/img/gui/panel-header.png: $(SRC_DIRECTORY)/psd/panel-header.psd
	convert -background transparent $< -flatten $@

$(DATA_DIRECTORY)/img/gui/minimap.png: $(SRC_DIRECTORY)/blender/gui/minimap/minimap.blend
	$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/gui/minimap; $(BLENDER) -b $(notdir $<) -P render.py
	mv $(SRC_DIRECTORY)/blender/gui/minimap/render/minimap.png $@

$(DATA_DIRECTORY)/img/gui/map-rotate.png: $(SRC_DIRECTORY)/blender/gui/map-rotate/map-rotate.blend
	$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/gui/map-rotate; $(BLENDER) -b $(notdir $<) -P render.py
	convert \
		$(SRC_DIRECTORY)/blender/gui/map-rotate/render/map-rotate-south.png \
		$(SRC_DIRECTORY)/blender/gui/map-rotate/render/map-rotate-south-highlight-left.png \
		$(SRC_DIRECTORY)/blender/gui/map-rotate/render/map-rotate-south-highlight-right.png \
		$(SRC_DIRECTORY)/blender/gui/map-rotate/render/map-rotate-east.png \
		$(SRC_DIRECTORY)/blender/gui/map-rotate/render/map-rotate-east-highlight-left.png \
		$(SRC_DIRECTORY)/blender/gui/map-rotate/render/map-rotate-east-highlight-right.png \
		$(SRC_DIRECTORY)/blender/gui/map-rotate/render/map-rotate-north.png \
		$(SRC_DIRECTORY)/blender/gui/map-rotate/render/map-rotate-north-highlight-left.png \
		$(SRC_DIRECTORY)/blender/gui/map-rotate/render/map-rotate-north-highlight-right.png \
		$(SRC_DIRECTORY)/blender/gui/map-rotate/render/map-rotate-west.png \
		$(SRC_DIRECTORY)/blender/gui/map-rotate/render/map-rotate-west-highlight-left.png \
		$(SRC_DIRECTORY)/blender/gui/map-rotate/render/map-rotate-west-highlight-right.png \
		-crop 50%x100% \
		-swap 1,2 -delete 3,4 \
		-swap 5,6 -delete 7,8 \
		-swap 9,10 -delete 11,12 \
		-swap 13,14 -delete 15,16 \
		+append $@

$(DATA_DIRECTORY)/img/gui/map-zoom.png: $(SRC_DIRECTORY)/blender/gui/map-zoom/map-zoom.blend
	$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/gui/map-zoom; $(BLENDER) -b $(notdir $<) -P render.py
	convert \
		$(SRC_DIRECTORY)/blender/gui/map-zoom/render/map-zoom.png \
		$(SRC_DIRECTORY)/blender/gui/map-zoom/render/map-zoom-highlight-plus.png \
		$(SRC_DIRECTORY)/blender/gui/map-zoom/render/map-zoom-highlight-minus.png \
		-crop 50%x100% \
		-swap 1,2 -delete 3,4 \
		+append $@

$(DATA_DIRECTORY)/img/gui/add-building/panel-top.png: $(SRC_DIRECTORY)/psd/add-building-panel.psd
	$(CREATE_TARGET_DIRECTORY)
	convert $< -flatten -crop 245x5+0+0 $@

$(DATA_DIRECTORY)/img/gui/add-building/panel.png: $(SRC_DIRECTORY)/psd/add-building-panel.psd
	$(CREATE_TARGET_DIRECTORY)
	convert $< -flatten -crop 245x60+0+5 $@

$(DATA_DIRECTORY)/img/gui/add-building/panel-bottom.png: $(SRC_DIRECTORY)/psd/add-building-panel.psd
	$(CREATE_TARGET_DIRECTORY)
	convert $< -flatten -crop 245x5+0+65 $@

SCREENSHOT=$(DATA_DIRECTORY)/img/gui/add-building/screenshot-add-building-gui.bmp
build-add-building-gui: $(TARGET_EXECUTABLE)
	mkdir -p $(DATA_DIRECTORY)/img/gui/add-building
	# TODO später, wenn der Zustand "Gras abgefressen" in der Map gespeichert ist, können wir vorhersehbar machen, wo Schafe/Rinder hinlaufen, damit sie immer an derselben Stelle im Screenshot sind
	# Alternative wäre Boden-Kacheln zu nehmen, die kein Gras anbieten
	# TODO später, wenn Gebäude-Lagerbestand in der Map gespeichert ist, dem Förster etwas Lagerbestand geben, um Baumstämme vor dem Haus liegen zu haben
	$(TARGET_EXECUTABLE) -m $(DATA_DIRECTORY)/map/map-for-add-building-gui.tmx \
	                     -t 19000 -s $(SCREENSHOT)
	convert $(SCREENSHOT) \
	    \( +clone -crop 53x53+2013+1602 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/bakery.png +delete \) \
	    \( +clone -crop 75x75+2632+1752 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/butchers.png +delete \) \
	    \( +clone -crop 102x102+3019+1112 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/cathedral.png +delete \) \
	    \( +clone -crop 103x103+3524+1316 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/cattle-farm.png +delete \) \
	    \( +clone -crop 67x67+3310+1621 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/chapel.png +delete \) \
	    \( +clone -crop 55x55+3620+1513 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/cobbled-street.png +delete \) \
	    \( +clone -crop 78x78+2937+1780 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/distillery.png +delete \) \
	    \( +clone -crop 55x55+3497+1577 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/farm-road.png +delete \) \
	    \( +clone -crop 66x66+3220+1999 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/foresters.png +delete \) \
	    \( +clone -crop 69x69+2973+2001 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/forest.png +delete \) \
	    \( +clone -crop 73x73+2258+1928 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/grain-farm.png +delete \) \
	    \( +clone -crop 63x63+2170+1844 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/grain-field.png +delete \) \
	    \( +clone -crop 72x72+1925+1757 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/grain-mill.png +delete \) \
	    \( +clone -crop 50x50+3169+1699 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/house.png +delete \) \
	    \( +clone -crop 61x61+3410+1530 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/hunters-hut.png +delete \) \
	    \( +clone -crop 89x89+3038+1525 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/marketplace.png +delete \) \
	    \( +clone -crop 80x80+3636+1955 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/office1.png +delete \) \
	    \( +clone -crop 64x64+3836+1800 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/office2.png +delete \) \
	    \( +clone -crop 57x57+4234+1578 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/pier.png +delete \) \
	    \( +clone -crop 69x69+3319+1807 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/school.png +delete \) \
	    \( +clone -crop 92x92+3210+1332 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/sheep-farm.png +delete \) \
	    \( +clone -crop 65x65+2485+1615 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/spices-field.png +delete \) \
	    \( +clone -crop 74x74+2316+1533 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/spices-plantation.png +delete \) \
	    \( +clone -crop 56x56+3748+1572 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/square1.png +delete \) \
	    \( +clone -crop 56x56+3812+1540 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/square2.png +delete \) \
	    \( +clone -crop 56x56+3876+1508 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/square3.png +delete \) \
	    \( +clone -crop 72x72+2452+1834 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/stonemason.png +delete \) \
	    \( +clone -crop 65x65+2647+1502 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/sugarcane-field.png +delete \) \
	    \( +clone -crop 67x67+2531+1392 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/sugarcane-plantation.png +delete \) \
	    \( +clone -crop 64x64+3098+1872 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/tavern.png +delete \) \
	    \( +clone -crop 63x63+2972+1413 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/tobacco-field.png +delete \) \
	    \( +clone -crop 60x60+2786+1863 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/tobacco-goods.png +delete \) \
	    \( +clone -crop 76x76+2789+1318 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/tobacco-plantation.png +delete \) \
	    \( +clone -crop 56x56+2594+1963 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/toolsmiths.png +delete \) \
	    \( +clone -crop 68x68+2787+1664 -resize 46x46 -write $(DATA_DIRECTORY)/img/gui/add-building/weaving-mill1.png +delete \) \
	    null:
	rm $(SCREENSHOT)
	
########################################################################################################################
# Gebäude (statisch)                                                                                                   #
########################################################################################################################

BUILDINGS := aristocrats-house1 bakery burghers-house1 butchers cathedral cattle-farm chapel distillery \
             foresters grain-farm hunters-hut marketplace merchants-house1 office1 office2 pier \
             pioneers-house1 pioneers-house2 pioneers-house3 pioneers-house4 pioneers-house5 \
             school settlers-house1 settlers-house2 settlers-house3 settlers-house4 settlers-house5 \
             sheep-farm square1 spices-plantation square2 square3 stonemason sugarcane-plantation tavern \
             tobacco-goods tobacco-plantation toolsmiths weaving-mill1

define RENDER_BUILDING
$(DATA_DIRECTORY)/img/buildings/$(1).png: $(SRC_DIRECTORY)/blender/buildings/$(1)/$(1).blend
	$$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/buildings/$(1); $(BLENDER) -b $$(notdir $$<) -P ../render-building.py

	# geometry muss angegeben werden, sonst greift der Default von 120x120
	$(MONTAGE) \
	    $(SRC_DIRECTORY)/blender/buildings/$(1)/render/angle0.png \
	    $(SRC_DIRECTORY)/blender/buildings/$(1)/render/angle90.png \
	    $(SRC_DIRECTORY)/blender/buildings/$(1)/render/angle180.png \
	    $(SRC_DIRECTORY)/blender/buildings/$(1)/render/angle270.png \
	    -geometry +0+0 -tile 4x1 $$@
endef

$(foreach BUILDING,$(BUILDINGS),$(eval $(call RENDER_BUILDING,$(BUILDING))))

########################################################################################################################
# Gebäude (animiert)                                                                                                   #
########################################################################################################################

BUILDINGS_WITH_ANIMATION := grain-mill

define RENDER_BUILDING_WITH_ANIMATION
$(DATA_DIRECTORY)/img/buildings/$(1).png: $(SRC_DIRECTORY)/blender/buildings/$(1)/$(1).blend
	$$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/buildings/$(1); $(BLENDER) -b $$(notdir $$<) -P ../render-building-with-animation.py

	# geometry muss angegeben werden, sonst greift der Default von 120x120
	$(MONTAGE) \
	    $(SRC_DIRECTORY)/blender/buildings/$(1)/render/angle0/* \
	    $(SRC_DIRECTORY)/blender/buildings/$(1)/render/angle90/* \
	    $(SRC_DIRECTORY)/blender/buildings/$(1)/render/angle180/* \
	    $(SRC_DIRECTORY)/blender/buildings/$(1)/render/angle270/* \
	    -geometry +0+0 -tile x4 $$@
endef

$(foreach BUILDING_WITH_ANIMATION, \
  $(BUILDINGS_WITH_ANIMATION),\
  $(eval $(call RENDER_BUILDING_WITH_ANIMATION,$(BUILDING_WITH_ANIMATION))) \
)

########################################################################################################################
# Straßen                                                                                                              #
########################################################################################################################

STREET_TILESETS := cobbled-street farm-road

define RENDER_STREET_TILESET
$(DATA_DIRECTORY)/img/streets/$(1).png: $(SRC_DIRECTORY)/blender/streets/$(1)/$(1).blend
	$$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/streets/$(1); $(BLENDER) -b $$(notdir $$<) -P ../render-street-tileset.py

	# geometry muss angegeben werden, sonst greift der Default von 120x120
	$(MONTAGE) \
	    $(SRC_DIRECTORY)/blender/streets/$(1)/render/straight0.png \
	    $(SRC_DIRECTORY)/blender/streets/$(1)/render/straight90.png \
	    $(SRC_DIRECTORY)/blender/streets/$(1)/render/curve0.png \
	    $(SRC_DIRECTORY)/blender/streets/$(1)/render/curve90.png \
	    $(SRC_DIRECTORY)/blender/streets/$(1)/render/curve180.png \
	    $(SRC_DIRECTORY)/blender/streets/$(1)/render/curve270.png \
	    $(SRC_DIRECTORY)/blender/streets/$(1)/render/tee0.png \
	    $(SRC_DIRECTORY)/blender/streets/$(1)/render/tee90.png \
	    $(SRC_DIRECTORY)/blender/streets/$(1)/render/tee180.png \
	    $(SRC_DIRECTORY)/blender/streets/$(1)/render/tee270.png \
	    $(SRC_DIRECTORY)/blender/streets/$(1)/render/cross.png \
	    -geometry +0+0 -tile x1 $$@

	$(call FIX_TILE_TRANSPARENCY,$$@)
endef

$(foreach STREET_TILESET,$(STREET_TILESETS),$(eval $(call RENDER_STREET_TILESET,$(STREET_TILESET))))

########################################################################################################################
# Erntebare Landschaften                                                                                               #
########################################################################################################################

HARVESTABLES := grain-field northern-forest1 northern-forest2 spices-field sugarcane-field tobacco-field

define RENDER_HARVESTABLE
$(DATA_DIRECTORY)/img/harvestables/$(1).png: $(SRC_DIRECTORY)/blender/harvestables/$(1)/$(1).blend
	$$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/harvestables/$(1); $(BLENDER) -b $$(notdir $$<) -P ../render-harvestable.py

	# geometry muss angegeben werden, sonst greift der Default von 120x120
	$(MONTAGE) \
        $(SRC_DIRECTORY)/blender/harvestables/$(1)/render/angle0/* \
        $(SRC_DIRECTORY)/blender/harvestables/$(1)/render/angle90/* \
        $(SRC_DIRECTORY)/blender/harvestables/$(1)/render/angle180/* \
        $(SRC_DIRECTORY)/blender/harvestables/$(1)/render/angle270/* \
        -geometry +0+0 -tile x4 $$@
endef

$(foreach HARVESTABLE,$(HARVESTABLES),$(eval $(call RENDER_HARVESTABLE,$(HARVESTABLE))))

########################################################################################################################
# Gütersymbole                                                                                                         #
########################################################################################################################

GOODS := alcohol bricks cattle cloth flour food grain spices sugar tobacco tobacco-goods tools wood wool

define RENDER_GOODS_ICONS
$(DATA_DIRECTORY)/img/goods/marketplace-icon/$(1).png $(DATA_DIRECTORY)/img/goods/icon/$(1).png: \
	$(SRC_DIRECTORY)/blender/goods/$(1)/$(1).blend \
	$(SRC_DIRECTORY)/blender/goods/render.py \
	$(SRC_DIRECTORY)/xcf/goods/marketplace-icon-background.png
	
	mkdir -p $(DATA_DIRECTORY)/img/goods/marketplace-icon
	mkdir -p $(DATA_DIRECTORY)/img/goods/icon
	
	cd $$(dir $$<); $(BLENDER) -b $$(notdir $$<) -P ../render.py
	
	mv -f $$(dir $$<)icon.png $(DATA_DIRECTORY)/img/goods/icon/$(1).png
	convert $(SRC_DIRECTORY)/xcf/goods/marketplace-icon-background.png -resize 42x42 $$(dir $$<)marketplace-icon.png -gravity center -composite $(DATA_DIRECTORY)/img/goods/marketplace-icon/$(1).png
	rm -f $$(dir $$<)marketplace-icon.png
endef

$(foreach GOOD,$(GOODS),$(eval $(call RENDER_GOODS_ICONS,$(GOOD))))

########################################################################################################################
# Animationen                                                                                                          #
########################################################################################################################

ANIMATIONS := carrier cattle

define RENDER_ANIMATION
$(DATA_DIRECTORY)/img/animations/$(1).png: $(SRC_DIRECTORY)/blender/animations/$(1)/$(1).blend
	$$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/animations/$(1); $(BLENDER) -b $$(notdir $$<) -P ../render-animation.py

	# geometry muss angegeben werden, sonst greift der Default von 120x120
	$(MONTAGE) \
        $(SRC_DIRECTORY)/blender/animations/$(1)/render/angle0/* \
        $(SRC_DIRECTORY)/blender/animations/$(1)/render/angle45/* \
        $(SRC_DIRECTORY)/blender/animations/$(1)/render/angle90/* \
        $(SRC_DIRECTORY)/blender/animations/$(1)/render/angle135/* \
        $(SRC_DIRECTORY)/blender/animations/$(1)/render/angle180/* \
        $(SRC_DIRECTORY)/blender/animations/$(1)/render/angle225/* \
        $(SRC_DIRECTORY)/blender/animations/$(1)/render/angle270/* \
        $(SRC_DIRECTORY)/blender/animations/$(1)/render/angle315/* \
        -geometry +0+0 -tile x8 $$@
endef

$(foreach ANIMATION,$(ANIMATIONS),$(eval $(call RENDER_ANIMATION,$(ANIMATION))))

########################################################################################################################
# Animation: Schaf                                                                                                     #
########################################################################################################################

render-sheep: $(SRC_DIRECTORY)/blender/animations/sheep/sheep.blend
	mkdir -p $(DATA_DIRECTORY)/img/animations
	cd $(SRC_DIRECTORY)/blender/animations/sheep; $(BLENDER) -b $(notdir $<) -P render.py

	# geometry muss angegeben werden, sonst greift der Default von 120x120
	for i in `seq 0 1`; \
	do \
	    $(MONTAGE) \
	        $(SRC_DIRECTORY)/blender/animations/sheep/render/angle0/sheep$$i-* \
	        $(SRC_DIRECTORY)/blender/animations/sheep/render/angle45/sheep$$i-* \
	        $(SRC_DIRECTORY)/blender/animations/sheep/render/angle90/sheep$$i-* \
	        $(SRC_DIRECTORY)/blender/animations/sheep/render/angle135/sheep$$i-* \
	        $(SRC_DIRECTORY)/blender/animations/sheep/render/angle180/sheep$$i-* \
	        $(SRC_DIRECTORY)/blender/animations/sheep/render/angle225/sheep$$i-* \
	        $(SRC_DIRECTORY)/blender/animations/sheep/render/angle270/sheep$$i-* \
	        $(SRC_DIRECTORY)/blender/animations/sheep/render/angle315/sheep$$i-* \
	        -geometry +0+0 -tile x8 $(DATA_DIRECTORY)/img/animations/sheep$$i.png; \
	done

########################################################################################################################
# Marktkarren-Animation                                                                                                #
########################################################################################################################

render-cart: $(SRC_DIRECTORY)/blender/animations/cart/cart.blend
	mkdir -p $(DATA_DIRECTORY)/img/animations
	cd $(SRC_DIRECTORY)/blender/animations/cart; $(BLENDER) -b $(notdir $<) -P render-cart.py

	# geometry muss angegeben werden, sonst greift der Default von 120x120
	$(MONTAGE) \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/without_cargo/angle0/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/without_cargo/angle45/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/without_cargo/angle90/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/without_cargo/angle135/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/without_cargo/angle180/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/without_cargo/angle225/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/without_cargo/angle270/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/without_cargo/angle315/* \
	    -geometry +0+0 -tile x8 $(DATA_DIRECTORY)/img/animations/cart-without-cargo.png
	$(MONTAGE) \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle0/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle45/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle90/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle135/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle180/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle225/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle270/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle315/* \
	    -geometry +0+0 -tile x8 $(DATA_DIRECTORY)/img/animations/cart-with-cargo.png

########################################################################################################################
# Schiffe                                                                                                              #
########################################################################################################################

$(DATA_DIRECTORY)/img/ships/little-ship.png: $(SRC_DIRECTORY)/blender/ships/little-ship/little-ship.blend
	$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/ships/little-ship; $(BLENDER) -b $(notdir $<) -P render.py

	# geometry muss angegeben werden, sonst greift der Default von 120x120
	$(MONTAGE) \
	    $(SRC_DIRECTORY)/blender/ships/little-ship/render/angle0.png \
	    $(SRC_DIRECTORY)/blender/ships/little-ship/render/angle45.png \
	    $(SRC_DIRECTORY)/blender/ships/little-ship/render/angle90.png \
	    $(SRC_DIRECTORY)/blender/ships/little-ship/render/angle135.png \
	    $(SRC_DIRECTORY)/blender/ships/little-ship/render/angle180.png \
	    $(SRC_DIRECTORY)/blender/ships/little-ship/render/angle225.png \
	    $(SRC_DIRECTORY)/blender/ships/little-ship/render/angle270.png \
	    $(SRC_DIRECTORY)/blender/ships/little-ship/render/angle315.png \
	    -geometry +0+0 -tile x8 $@

########################################################################################################################
# Banner                                                                                                               #
########################################################################################################################

render-coat-of-arms: $(SRC_DIRECTORY)/blender/gui/coat-of-arms/coat-of-arms.blend $(DATA_DIRECTORY)/img/gui/population-man.png
	mkdir -p $(DATA_DIRECTORY)/img/gui/coat-of-arms
	cd $(SRC_DIRECTORY)/blender/gui/coat-of-arms; $(BLENDER) -b coat-of-arms.blend -P render.py

	cp -rf $(SRC_DIRECTORY)/blender/gui/coat-of-arms/render/* $(DATA_DIRECTORY)/img/gui/coat-of-arms
	rm -rf $(SRC_DIRECTORY)/blender/gui/coat-of-arms/render

	mkdir -p $(DATA_DIRECTORY)/img/gui/coat-of-arms/population
	for color in blue red white yellow; \
		do convert -background transparent $(DATA_DIRECTORY)/img/gui/coat-of-arms/small/$$color.png \
		           -gravity north-west -extent 30x30 \
		           $(DATA_DIRECTORY)/img/gui/population-man.png -geometry +11+0 -composite \
		           $(DATA_DIRECTORY)/img/gui/coat-of-arms/population/$$color.png; \
	done

########################################################################################################################
# Sonstige Blender-Sachen                                                                                              #
########################################################################################################################

render-coin: $(SRC_DIRECTORY)/blender/gui/coin/coin.blend
	mkdir -p $(DATA_DIRECTORY)/img/gui/coin
	cd $(SRC_DIRECTORY)/blender/gui/coin; $(BLENDER) -b coin.blend -P render.py
	mv $(SRC_DIRECTORY)/blender/gui/coin/render/coin*.png $(DATA_DIRECTORY)/img/gui/coin
	
$(DATA_DIRECTORY)/img/gui/population-man.png: $(SRC_DIRECTORY)/blender/gui/population-man/population-man.blend
	$(CREATE_TARGET_DIRECTORY)
	$(BLENDER) -b $< -o //population-man\#.png -f 1
	mv $(SRC_DIRECTORY)/blender/gui/population-man/population-man1.png $@

render-build-menu: $(SRC_DIRECTORY)/blender/gui/build-menu/demolish.blend \
                   $(SRC_DIRECTORY)/blender/gui/build-menu/port.blend \
                   $(SRC_DIRECTORY)/blender/gui/build-menu/public.blend
	mkdir -p $(DATA_DIRECTORY)/img/gui/build-menu
	cd $(SRC_DIRECTORY)/blender/gui/build-menu; $(BLENDER) -b demolish.blend -P render-demolish.py
	cd $(SRC_DIRECTORY)/blender/gui/build-menu; $(BLENDER) -b port.blend -P render.py
	cd $(SRC_DIRECTORY)/blender/gui/build-menu; $(BLENDER) -b public.blend -P render.py
	mv $(SRC_DIRECTORY)/blender/gui/build-menu/render/* $(DATA_DIRECTORY)/img/gui/build-menu

render-public-building-icons: $(SRC_DIRECTORY)/blender/gui/public-building-icon/public-building-icon.blend
	mkdir -p $(DATA_DIRECTORY)/img/gui/public-building-icon
	cd $(SRC_DIRECTORY)/blender/gui/public-building-icon; $(BLENDER) -b public-building-icon.blend -P render.py
	mv $(SRC_DIRECTORY)/blender/gui/public-building-icon/render/* $(DATA_DIRECTORY)/img/gui/public-building-icon

########################################################################################################################
# PHONYs um alle Blender-Sachen zu rendern und zu cleanen                                                              #
########################################################################################################################

render-blender: \
	$(foreach BUILDING,$(BUILDINGS), \
	    $(DATA_DIRECTORY)/img/buildings/$(BUILDING).png \
	) \
	$(foreach GOOD,$(GOODS), \
	    $(DATA_DIRECTORY)/img/goods/marketplace-icon/$(GOOD).png \
	    $(DATA_DIRECTORY)/img/goods/icon/$(GOOD).png \
	) \
	$(foreach ANIMATION,$(ANIMATIONS), \
		$(DATA_DIRECTORY)/img/animations/$(ANIMATION).png \
	) \
	$(DATA_DIRECTORY)/img/tileset.png \
	$(foreach STREET_TILESET,$(STREET_TILESETS), \
		$(DATA_DIRECTORY)/img/streets/$(STREET_TILESET).png \
	) \
	$(foreach HARVESTABLE,$(HARVESTABLES), \
		$(DATA_DIRECTORY)/img/harvestables/$(HARVESTABLE).png \
	) \
	render-sheep \
	render-cart \
	$(DATA_DIRECTORY)/img/ships/little-ship.png \
	render-coat-of-arms \
	render-coin \
	render-build-menu \
	render-public-building-icons \
	$(DATA_DIRECTORY)/img/gui/population-man.png
	
clean-blender:
	rm -f $(foreach BUILDING,$(BUILDINGS), $(DATA_DIRECTORY)/img/buildings/$(BUILDING).png)
	rm -rf $(foreach BUILDING,$(BUILDINGS), $(SRC_DIRECTORY)/blender/buildings/$(BUILDING)/render)
	rm -rf $(DATA_DIRECTORY)/img/goods
	rm -f $(foreach ANIMATION,$(ANIMATIONS), $(DATA_DIRECTORY)/img/animations/$(ANIMATION).png)
	rm -rf $(foreach ANIMATION,$(ANIMATIONS), $(SRC_DIRECTORY)/blender/animations/$(ANIMATION)/render)
	rm -rf $(SRC_DIRECTORY)/blender/animations/cart/render
	rm -f $(DATA_DIRECTORY)/img/tileset.png
	rm -rf $(SRC_DIRECTORY)/blender/tiles/render
	rm -rf $(foreach STREET_TILESET,$(STREET_TILESETS), $(SRC_DIRECTORY)/blender/streets/$(STREET_TILESET)/render)
	rm -f $(foreach STREET_TILESET,$(STREET_TILESETS), $(DATA_DIRECTORY)/img/streets/$(STREET_TILESET).png)
	rm -f $(foreach HARVESTABLE,$(HARVESTABLES), $(DATA_DIRECTORY)/img/harvestables/$(HARVESTABLE).png)
	rm -rf $(foreach HARVESTABLE,$(HARVESTABLES), $(SRC_DIRECTORY)/blender/harvestables/$(HARVESTABLE)/render)
	rm -rf $(SRC_DIRECTORY)/blender/animations/sheep/render
	rm -f $(DATA_DIRECTORY)/img/animations/cart-without-cargo.png
	rm -f $(DATA_DIRECTORY)/img/animations/cart-with-cargo.png
	rm -rf $(SRC_DIRECTORY)/blender/ships/little-ship/render
	rm -f $(DATA_DIRECTORY)/img/ships/little-ship.png
	rm -rf $(DATA_DIRECTORY)/img/gui/coat-of-arms
	rm -rf $(SRC_DIRECTORY)/blender/gui/coins/render
	rm -rf $(DATA_DIRECTORY)/img/gui/coin
	rm -rf $(SRC_DIRECTORY)/blender/gui/build-menu/render
	rm -rf $(SRC_DIRECTORY)/blender/gui/public-building-icon/render
	rm -rf $(DATA_DIRECTORY)/img/gui/public-building-icon
	rm -f $(DATA_DIRECTORY)/img/gui/population-man.png
	rm -rf $(DATA_DIRECTORY)/img/animations
	rm -rf $(DATA_DIRECTORY)/img/buildings
	rm -rf $(DATA_DIRECTORY)/img/harvestables
	rm -rf $(DATA_DIRECTORY)/img/ships
	rm -rf $(DATA_DIRECTORY)/img/streets
