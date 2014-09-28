package org.openisles.tools.renderingorder.gui;

import org.openisles.tools.renderingorder.RenderingOrder;
import org.openisles.tools.renderingorder.map.Graphic;
import org.openisles.tools.renderingorder.map.GraphicsMgr;
import org.openisles.tools.renderingorder.map.MapObject;
import org.openisles.tools.renderingorder.util.Point;

import javax.swing.*;
import java.awt.*;

public class MapPanel extends JPanel {

    private static final int screenOffsetX = 300;
    private static final int screenOffsetY = 30;

    @Override
    protected void paintComponent(Graphics g) {
        // Hintergrund
        Graphics2D g2D = (Graphics2D) g;

        g2D.setColor(new Color(0.3f, 0.4f, 1.0f));
        g2D.fillRect(0, 0, getWidth(), getHeight());

        // Objekte rendern
        for (MapObject mapObject : RenderingOrder.mapObjects) {
            Graphic graphic = RenderingOrder.graphicsMgr.get(mapObject.getGraphicType());
            Point screenCoords = mapToDrawScreenCoords(mapObject.getMapX(), mapObject.getMapY(), graphic);

            int sx = screenCoords.getX() + screenOffsetX;
            int sy = screenCoords.getY() + screenOffsetY;

            g2D.drawImage(graphic.getImage(), sx, sy, null);
        }
    }

    private static Point mapToDrawScreenCoords(int mapX, int mapY, Graphic graphic) {
        int x, y;

        // Grafik an die richtige Stelle schieben. Das muss ausgehend von der zu belegenden Tile-Fläche berechnet werden.
        Point screenCoords = mapToScreenCoords(mapX, mapY);
        x = screenCoords.getX();
        y = screenCoords.getY();

        x -= graphic.getWidth() - (graphic.getMapWidth() + 1) * GraphicsMgr.TILE_WIDTH_HALF;
        y -= graphic.getHeight() - (graphic.getMapWidth() + graphic.getMapHeight()) * GraphicsMgr.TILE_HEIGHT_HALF;

        return new Point(x, y);
    }

    private static Point mapToScreenCoords(int mapX, int mapY) {
        int screenX = (mapX - mapY) * GraphicsMgr.TILE_WIDTH_HALF;
        int screenY = (mapX + mapY) * GraphicsMgr.TILE_HEIGHT_HALF;

        return new Point(screenX, screenY);
    }

}
