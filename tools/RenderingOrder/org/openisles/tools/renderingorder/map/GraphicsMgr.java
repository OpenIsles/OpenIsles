package org.openisles.tools.renderingorder.map;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class GraphicsMgr {

    public static final int TILE_WIDTH = 64;
    public static final int TILE_HEIGHT = 32;
    public static final int TILE_WIDTH_HALF = TILE_WIDTH / 2;
    public static final int TILE_HEIGHT_HALF = TILE_HEIGHT / 2;

    private Map<Graphic.Type, Graphic> graphics;

    public GraphicsMgr() throws IOException {
        graphics = new HashMap<Graphic.Type, Graphic>(10);

        graphics.put(Graphic.Type.CHAPEL, new Graphic("data/img/objects/chapel.png", 2, 1));
        graphics.put(Graphic.Type.PIONEERS_HOUSE1, new Graphic("data/img/objects/pioneers-house1.png", 2, 2));
        graphics.put(Graphic.Type.SIGNALFIRE, new Graphic("data/img/objects/signalfire.png", 1, 1));
        graphics.put(Graphic.Type.HERBARY, new Graphic("data/img/objects/herbary.png", 3, 3));
        graphics.put(Graphic.Type.BRICKYARD, new Graphic("data/img/objects/brickyard.png", 4, 2));
        graphics.put(Graphic.Type.BRICKYARD2, new Graphic("data/img/objects/brickyard2.png", 2, 4));
        graphics.put(Graphic.Type.OFFICE1, new Graphic("data/img/objects/office1.png", 3, 2));
        graphics.put(Graphic.Type.MARKETPLACE, new Graphic("data/img/objects/marketplace.png", 4, 3));
        graphics.put(Graphic.Type.FORESTERS, new Graphic("data/img/objects/foresters.png", 2, 2));
    }

    public Graphic get(Graphic.Type graphicType) {
        return graphics.get(graphicType);
    }
}
