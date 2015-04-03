SRC_DIRECTORY := src
DATA_DIRECTORY := data

# Für das fertige Tileset für Tiled: Anzahl Kacheln pro Zeile
TILESET_WIDTH := 4

CREATE_TARGET_DIRECTORY = mkdir -p $(@D)

MONTAGE := montage -background transparent

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
	mkdir -p $(DATA_DIRECTORY)/img/tiles
	cd $(SRC_DIRECTORY)/blender/tiles; blender -b $(notdir $<) -P render.py
	php $(SRC_DIRECTORY)/blender/tiles/generate-tileset.php
	$(call FIX_TILE_TRANSPARENCY,$(SRC_DIRECTORY)/blender/tiles/render/tileset.png)

	cp $(SRC_DIRECTORY)/blender/tiles/render/tileset.png $(DATA_DIRECTORY)/img/tileset.png

########################################################################################################################
# GUI                                                                                                                  #
########################################################################################################################
	
clean-gui:
	rm -f $(DATA_DIRECTORY)/img/gui/panel.png
	rm -f $(DATA_DIRECTORY)/img/gui/statusbar.png
	rm -f $(DATA_DIRECTORY)/img/gui/add-building/add-building-grid.png
	
build-gui: $(DATA_DIRECTORY)/img/gui/panel.png $(DATA_DIRECTORY)/img/gui/statusbar.png $(DATA_DIRECTORY)/img/gui/add-building/add-building-grid.png


$(DATA_DIRECTORY)/img/gui/panel.png:
	$(CREATE_TARGET_DIRECTORY)
	convert -size 236x748 canvas:"#907f67" -mattecolor "#6f5038" -frame 10x10+5+5 $@
	
$(DATA_DIRECTORY)/img/gui/statusbar.png:
	$(CREATE_TARGET_DIRECTORY)
	convert -size 758x24 canvas:"#907f67" -mattecolor "#6f5038" -frame 5x5+2+2 $@

$(DATA_DIRECTORY)/img/gui/add-building/add-building-grid.png: $(SRC_DIRECTORY)/blender/ui/add-building-grid/add-building-grid.blend
	$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/ui/add-building-grid; blender -b $(notdir $<) -P render.py
	mv $(SRC_DIRECTORY)/blender/ui/add-building-grid/add-building-grid.png $@
	
########################################################################################################################
# Gebäude                                                                                                              #
########################################################################################################################

BUILDINGS := butchers cathedral cattle-farm chapel foresters hunters-hut marketplace office1 office2 \
             pioneers-house1 pioneers-house2 pioneers-house3 pioneers-house4 pioneers-house5 \
             settlers-house1 sheep-farm square1 square2 square3 stonemason tavern toolsmiths weaving-mill1

define RENDER_BUILDING
$(DATA_DIRECTORY)/img/objects/$(1).png: $(SRC_DIRECTORY)/blender/buildings/$(1)/$(1).blend
	$$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/buildings/$(1); blender -b $$(notdir $$<) -P ../render-building.py

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
$(DATA_DIRECTORY)/img/objects/$(1).png: $(SRC_DIRECTORY)/blender/streets/$(1)/$(1).blend
	mkdir -p $(DATA_DIRECTORY)/img/objects
	cd $(SRC_DIRECTORY)/blender/streets/$(1); blender -b $$(notdir $$<) -P ../render-street-tileset.py

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

HARVESTABLES := northern-forest1 northern-forest2

define RENDER_HARVESTABLE
$(DATA_DIRECTORY)/img/harvestables/$(1).png: $(SRC_DIRECTORY)/blender/harvestables/$(1)/$(1).blend
	$$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/harvestables/$(1); blender -b $$(notdir $$<) -P ../render-harvestable.py

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

GOODS := bricks cattle cloth food tools wood wool

define RENDER_GOODS_ICONS
$(DATA_DIRECTORY)/img/goods/marketplace-icon/$(1).png $(DATA_DIRECTORY)/img/goods/icon/$(1).png: \
	$(SRC_DIRECTORY)/blender/goods/$(1)/$(1).blend \
	$(SRC_DIRECTORY)/blender/goods/render.py \
	$(SRC_DIRECTORY)/xcf/goods/marketplace-icon-background.xcf
	
	mkdir -p $(DATA_DIRECTORY)/img/goods/marketplace-icon
	mkdir -p $(DATA_DIRECTORY)/img/goods/icon
	
	cd $$(dir $$<); blender -b $$(notdir $$<) -P ../render.py
	
	mv -f $$(dir $$<)icon.png $(DATA_DIRECTORY)/img/goods/icon/$(1).png
	convert $(SRC_DIRECTORY)/xcf/goods/marketplace-icon-background.xcf -resize 42x42 -flatten $$(dir $$<)marketplace-icon.png -gravity center -composite $(DATA_DIRECTORY)/img/goods/marketplace-icon/$(1).png
	rm -f $$(dir $$<)marketplace-icon.png
endef

$(foreach GOOD,$(GOODS),$(eval $(call RENDER_GOODS_ICONS,$(GOOD))))

########################################################################################################################
# Animationen                                                                                                          #
########################################################################################################################

ANIMATIONS := carrier

define RENDER_ANIMATION
$(DATA_DIRECTORY)/img/objects/$(1).png: $(SRC_DIRECTORY)/blender/animations/$(1)/$(1).blend
	$$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/animations/$(1); blender -b $$(notdir $$<) -P ../render-animation.py

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
	mkdir -p $(DATA_DIRECTORY)/img/objects
	cd $(SRC_DIRECTORY)/blender/animations/cart; blender -b $(notdir $<) -P render-cart.py

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
	    -geometry +0+0 -tile x8 $(DATA_DIRECTORY)/img/objects/cart-without-cargo.png
	$(MONTAGE) \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle0/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle45/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle90/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle135/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle180/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle225/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle270/* \
	    $(SRC_DIRECTORY)/blender/animations/cart/render/with_cargo/angle315/* \
	    -geometry +0+0 -tile x8 $(DATA_DIRECTORY)/img/objects/cart-with-cargo.png

########################################################################################################################
# Banner                                                                                                               #
########################################################################################################################

render-coat-of-arms: $(SRC_DIRECTORY)/blender/ui/coat-of-arms/coat-of-arms.blend $(DATA_DIRECTORY)/img/population-man.png
	mkdir -p $(DATA_DIRECTORY)/img/gui/coat-of-arms
	cd $(SRC_DIRECTORY)/blender/ui/coat-of-arms; blender -b coat-of-arms.blend -P render.py

	cp -rf $(SRC_DIRECTORY)/blender/ui/coat-of-arms/render/* $(DATA_DIRECTORY)/img/gui/coat-of-arms
	rm -rf $(SRC_DIRECTORY)/blender/ui/coat-of-arms/render

	mkdir -p $(DATA_DIRECTORY)/img/gui/coat-of-arms/population
	for color in blue red white yellow; \
		do convert -background transparent $(DATA_DIRECTORY)/img/gui/coat-of-arms/small/$$color.png \
		           -gravity north-west -extent 30x30 \
		           $(DATA_DIRECTORY)/img/population-man.png -geometry +11+0 -composite \
		           $(DATA_DIRECTORY)/img/gui/coat-of-arms/population/$$color.png; \
	done

########################################################################################################################
# Sonstige Blender-Sachen                                                                                              #
########################################################################################################################

$(DATA_DIRECTORY)/img/coin.png: $(SRC_DIRECTORY)/blender/ui/coin/coin.blend
	$(CREATE_TARGET_DIRECTORY)
	blender -b $< -o //coin\#.png -f 1
	mv $(SRC_DIRECTORY)/blender/ui/coin/coin1.png $@
	
$(DATA_DIRECTORY)/img/population-man.png: $(SRC_DIRECTORY)/blender/ui/population-man/population-man.blend
	$(CREATE_TARGET_DIRECTORY)
	blender -b $< -o //population-man\#.png -f 1
	mv $(SRC_DIRECTORY)/blender/ui/population-man/population-man1.png $@

########################################################################################################################
# PHONYs um alle Blender-Sachen zu rendern und zu cleanen                                                              #
########################################################################################################################

render-blender: \
	$(foreach BUILDING,$(BUILDINGS), \
	    $(DATA_DIRECTORY)/img/objects/$(BUILDING).png \
	) \
	$(foreach GOOD,$(GOODS), \
	    $(DATA_DIRECTORY)/img/goods/marketplace-icon/$(GOOD).png \
	    $(DATA_DIRECTORY)/img/goods/icon/$(GOOD).png \
	) \
	$(foreach ANIMATION,$(ANIMATIONS), \
		$(DATA_DIRECTORY)/img/objects/$(ANIMATION).png \
	) \
	$(DATA_DIRECTORY)/img/tileset.png \
	$(foreach STREET_TILESET,$(STREET_TILESETS), \
		$(DATA_DIRECTORY)/img/objects/$(STREET_TILESET).png \
	) \
	$(foreach HARVESTABLE,$(HARVESTABLES), \
		$(DATA_DIRECTORY)/img/harvestables/$(HARVESTABLE).png \
	) \
	render-cart \
	render-coat-of-arms \
	$(DATA_DIRECTORY)/img/coin.png \
	$(DATA_DIRECTORY)/img/population-man.png
	
clean-blender:
	rm -f $(foreach BUILDING,$(BUILDINGS), $(DATA_DIRECTORY)/img/objects/$(BUILDING).png)
	rm -rf $(foreach BUILDING,$(BUILDINGS), $(SRC_DIRECTORY)/blender/buildings/$(BUILDING)/render)
	rm -rf $(DATA_DIRECTORY)/img/goods
	rm -f $(foreach ANIMATION,$(ANIMATIONS), $(DATA_DIRECTORY)/img/objects/$(ANIMATION).png)
	rm -rf $(foreach ANIMATION,$(ANIMATIONS), $(SRC_DIRECTORY)/blender/animations/$(ANIMATION)/render)
	rm -rf $(SRC_DIRECTORY)/blender/animations/cart/render
	rm -f $(DATA_DIRECTORY)/img/tileset.png
	rm -rf $(SRC_DIRECTORY)/blender/tiles/render
	rm -rf $(foreach STREET_TILESET,$(STREET_TILESETS), $(SRC_DIRECTORY)/blender/streets/$(STREET_TILESET)/render)
	rm -f $(foreach STREET_TILESET,$(STREET_TILESETS), $(DATA_DIRECTORY)/img/objects/$(STREET_TILESET).png)
	rm -f $(foreach HARVESTABLE,$(HARVESTABLES), $(DATA_DIRECTORY)/img/harvestables/$(HARVESTABLE).png)
	rm -rf $(foreach HARVESTABLE,$(HARVESTABLES), $(SRC_DIRECTORY)/blender/harvestables/$(HARVESTABLE)/render)
	rm -f $(DATA_DIRECTORY)/img/objects/cart-without-cargo.png
	rm -f $(DATA_DIRECTORY)/img/objects/cart-with-cargo.png
	rm -rf $(DATA_DIRECTORY)/img/gui/coat-of-arms
	rm -f $(DATA_DIRECTORY)/img/coin.png
	rm -f $(DATA_DIRECTORY)/img/population-man.png
