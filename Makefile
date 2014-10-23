SRC_DIRECTORY := src
DATA_DIRECTORY := data

# Für das fertige Tileset für Tiled: Anzahl Kacheln pro Zeile
TILESET_WIDTH := 4

CREATE_TARGET_DIRECTORY = mkdir -p $(@D)


.PHONY: all clean build-tiles clean-tiles build-gui clean-gui render-blender clean-blender

all: build-tiles build-gui render-blender

clean: clean-tiles clean-gui clean-blender


build-tiles:
	awk 'NF > 0 && /^[^#]/ { print "convert $(SRC_DIRECTORY)/xcf/tileset-src.xcf -crop 64x32+" $$2 "+" $$3 " $(DATA_DIRECTORY)/img/tiles/alpha-mask.png -alpha Off -compose CopyOpacity -composite $(DATA_DIRECTORY)/img/tiles/" $$4 }' $(DATA_DIRECTORY)/img/tiles/tiles.txt | bash
	convert -size $$(($(TILESET_WIDTH)*64))x256 canvas:transparent $(DATA_DIRECTORY)/img/tiles/tileset.png
	awk 'NF > 0 && /^[^#]/ { print "convert $(DATA_DIRECTORY)/img/tiles/tileset.png $(DATA_DIRECTORY)/img/tiles/" $$4 " -geometry +" (($$1-1)%$(TILESET_WIDTH))*64 "+" int(($$1-1)/$(TILESET_WIDTH))*32 " -composite $(DATA_DIRECTORY)/img/tiles/tileset.png" }' $(DATA_DIRECTORY)/img/tiles/tiles.txt | bash
	
clean-tiles:
	awk 'NF > 0 && /^[^#]/ { print "rm -f $(DATA_DIRECTORY)/img/tiles/" $$4 }' $(DATA_DIRECTORY)/img/tiles/tiles.txt | bash
	rm -f $(DATA_DIRECTORY)/img/tiles/tileset.png

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

BUILDINGS := chapel foresters marketplace office1 pioneers-house1 sheep-farm weaving-mill1

define RENDER_BUILDING
$(DATA_DIRECTORY)/img/objects/$(1).png: $(SRC_DIRECTORY)/blender/$(1)/$(1).blend
	$$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/$(1); blender -b $$(notdir $$<) -P ../render-building.py
	cp $(SRC_DIRECTORY)/blender/$(1)/render/angle0.png $$@
endef

$(foreach BUILDING,$(BUILDINGS),$(eval $(call RENDER_BUILDING,$(BUILDING))))

########################################################################################################################
# Straßen                                                                                                              #
########################################################################################################################

render-streets: $(SRC_DIRECTORY)/blender/streets/streets.blend
	mkdir -p $(DATA_DIRECTORY)/img/objects
	cd $(SRC_DIRECTORY)/blender/streets; blender -b $(notdir $<) -P render-streets.py
	cp $(SRC_DIRECTORY)/blender/streets/render/* $(DATA_DIRECTORY)/img/objects

########################################################################################################################
# Gütersymbole                                                                                                         #
########################################################################################################################

GOODS := bricks cattle cloth tools wood wool

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
# Animationen                                                                                                         #
########################################################################################################################

ANIMATIONS := carrier

define RENDER_ANIMATION
$(DATA_DIRECTORY)/img/objects/$(1).png: $(SRC_DIRECTORY)/blender/$(1)/$(1).blend
	$$(CREATE_TARGET_DIRECTORY)
	cd $(SRC_DIRECTORY)/blender/$(1); blender -b $$(notdir $$<) -P ../render-animation.py

	# geometry muss angegeben werden, sonst greift der Default von 120x120
	montage -background transparent $(SRC_DIRECTORY)/blender/$(1)/render/angle0/* -geometry +0+0 -tile x1 $$@
endef

$(foreach ANIMATION,$(ANIMATIONS),$(eval $(call RENDER_ANIMATION,$(ANIMATION))))

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
	render-streets
	
clean-blender:
	rm -f $(foreach BUILDING,$(BUILDINGS), $(DATA_DIRECTORY)/img/objects/$(BUILDING).png)
	rm -rf $(foreach BUILDING,$(BUILDINGS), $(SRC_DIRECTORY)/blender/$(BUILDING)/render)
	rm -rf $(DATA_DIRECTORY)/img/goods
	rm -f $(foreach ANIMATION,$(ANIMATIONS), $(DATA_DIRECTORY)/img/objects/$(ANIMATION).png)
	rm -rf $(foreach ANIMATION,$(ANIMATIONS), $(SRC_DIRECTORY)/blender/$(ANIMATION)/render)
	rm -rf $(SRC_DIRECTORY)/blender/streets/render
	rm -rf $(DATA_DIRECTORY)/img/objects/street*.png
