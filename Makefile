SRC_DIRECTORY := src
DATA_DIRECTORY := data

# Für das fertige Tileset für Tiled: Anzahl Kacheln pro Zeile
TILESET_WIDTH := 4

CREATE_TARGET_DIRECTORY = mkdir -p $(@D)

MONTAGE := montage -background transparent
BLENDER := /opt/blender-2.74/blender

.PHONY: all clean build-gui clean-gui render-cart render-coat-of-arms render-blender clean-blender

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
	rm -f $(DATA_DIRECTORY)/img/gui/add-building/add-building-grid.png
	
build-gui: \
	$(DATA_DIRECTORY)/img/gui/panel.png \
	$(DATA_DIRECTORY)/img/gui/statusbar.png \
	$(DATA_DIRECTORY)/img/gui/panel-header.png \
	$(DATA_DIRECTORY)/img/gui/minimap.png \
	$(DATA_DIRECTORY)/img/gui/map-rotate.png \
	$(DATA_DIRECTORY)/img/gui/map-zoom.png \
	$(DATA_DIRECTORY)/img/gui/add-building/add-building-grid.png

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

$(DATA_DIRECTORY)/img/gui/add-building/add-building-grid.png: $(SRC_DIRECTORY)/blender/gui/add-building-grid/add-building-grid.blend
	$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/gui/add-building-grid; $(BLENDER) -b $(notdir $<) -P render.py
	mv $(SRC_DIRECTORY)/blender/gui/add-building-grid/add-building-grid.png $@
	
########################################################################################################################
# Gebäude                                                                                                              #
########################################################################################################################

BUILDINGS := butchers cathedral cattle-farm chapel distillery foresters hunters-hut marketplace \
             office1 office2 pier \
             pioneers-house1 pioneers-house2 pioneers-house3 pioneers-house4 pioneers-house5 \
             settlers-house1 settlers-house2 settlers-house3 settlers-house4 settlers-house5 \
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

HARVESTABLES := northern-forest1 northern-forest2 spices-field sugarcane-field tobacco-field

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

GOODS := alcohol bricks cattle cloth food spices sugar tobacco tobacco-goods tools wood wool

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

ANIMATIONS := carrier

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

$(DATA_DIRECTORY)/img/gui/coin.png: $(SRC_DIRECTORY)/blender/gui/coin/coin.blend
	$(CREATE_TARGET_DIRECTORY)
	blender -b $< -o //coin\#.png -f 1
	mv $(SRC_DIRECTORY)/blender/gui/coin/coin1.png $@
	
$(DATA_DIRECTORY)/img/gui/population-man.png: $(SRC_DIRECTORY)/blender/gui/population-man/population-man.blend
	$(CREATE_TARGET_DIRECTORY)
	blender -b $< -o //population-man\#.png -f 1
	mv $(SRC_DIRECTORY)/blender/gui/population-man/population-man1.png $@

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
	render-cart \
	$(DATA_DIRECTORY)/img/ships/little-ship.png \
	render-coat-of-arms \
	$(DATA_DIRECTORY)/img/gui/coin.png \
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
	rm -f $(DATA_DIRECTORY)/img/animations/cart-without-cargo.png
	rm -f $(DATA_DIRECTORY)/img/animations/cart-with-cargo.png
	rm -rf $(SRC_DIRECTORY)/blender/ships/little-ship/render
	rm -f $(DATA_DIRECTORY)/img/ships/little-ship.png
	rm -rf $(DATA_DIRECTORY)/img/gui/coat-of-arms
	rm -f $(DATA_DIRECTORY)/img/gui/coin.png
	rm -f $(DATA_DIRECTORY)/img/gui/population-man.png
	rm -rf $(DATA_DIRECTORY)/img/animations
	rm -rf $(DATA_DIRECTORY)/img/buildings
	rm -rf $(DATA_DIRECTORY)/img/harvestables
	rm -rf $(DATA_DIRECTORY)/img/ships
	rm -rf $(DATA_DIRECTORY)/img/streets
