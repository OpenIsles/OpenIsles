package org.openisles.tools.renderingorder.map;

public class MapObject {

    private int mapX;
    private int mapY;
    private Graphic.Type graphicType;

    private transient int drawingIndex = -1;

    public MapObject(int mapX, int mapY, Graphic.Type graphicType) {
        this.mapX = mapX;
        this.mapY = mapY;
        this.graphicType = graphicType;
    }

    public int getMapX() {
        return mapX;
    }

    public int getMapY() {
        return mapY;
    }

    public Graphic.Type getGraphicType() {
        return graphicType;
    }

    public int getDrawingIndex() {
        return drawingIndex;
    }

    public void setDrawingIndex(int drawingIndex) {
        this.drawingIndex = drawingIndex;
    }
}
