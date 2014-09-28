package org.openisles.tools.renderingorder;

import org.openisles.tools.renderingorder.gui.MainWindow;
import org.openisles.tools.renderingorder.map.Graphic;
import org.openisles.tools.renderingorder.map.GraphicsMgr;
import org.openisles.tools.renderingorder.map.MapObject;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

public class RenderingOrder {

    public static MainWindow mainWindow;
    public static GraphicsMgr graphicsMgr;
    public static List<MapObject> mapObjects;

    public RenderingOrder() throws IOException {
        graphicsMgr = new GraphicsMgr();
        mapObjects = new LinkedList<MapObject>();

        mapObjects.add(new MapObject(0, 0, Graphic.Type.OFFICE1));
        mapObjects.add(new MapObject(4, 4, Graphic.Type.MARKETPLACE));
        mapObjects.add(new MapObject(4, 8, Graphic.Type.MARKETPLACE));
        mapObjects.add(new MapObject(4, 7, Graphic.Type.CHAPEL));
        mapObjects.add(new MapObject(6, 7, Graphic.Type.CHAPEL));
        mapObjects.add(new MapObject(0, 5, Graphic.Type.MARKETPLACE));
        mapObjects.add(new MapObject(8, 6, Graphic.Type.MARKETPLACE));
        mapObjects.add(new MapObject(8, 5, Graphic.Type.CHAPEL));
        mapObjects.add(new MapObject(10, 5, Graphic.Type.CHAPEL));
        mapObjects.add(new MapObject(8, 9, Graphic.Type.CHAPEL));
        mapObjects.add(new MapObject(10, 9, Graphic.Type.CHAPEL));
        mapObjects.add(new MapObject(0, 4, Graphic.Type.CHAPEL));
        mapObjects.add(new MapObject(2, 4, Graphic.Type.CHAPEL));
        mapObjects.add(new MapObject(0, 8, Graphic.Type.CHAPEL));
        mapObjects.add(new MapObject(2, 8, Graphic.Type.CHAPEL));
        mapObjects.add(new MapObject(0, 2, Graphic.Type.FORESTERS));
    }

    private void run() throws IOException {
        mainWindow = new MainWindow();
        mainWindow.setVisible(true);
    }

    public static void main(String[] args) throws IOException {
        RenderingOrder renderingOrder = new RenderingOrder();
        renderingOrder.run();
    }

}
