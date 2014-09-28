package org.openisles.tools.renderingorder.map;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

public class Graphic {

    private String filepath;
    private int mapWidth;
    private int mapHeight;

    private transient BufferedImage image;

    public Graphic(String filepath, int mapWidth, int mapHeight) throws IOException {
        this.filepath = filepath;
        this.mapWidth = mapWidth;
        this.mapHeight = mapHeight;

        loadGraphic();
    }

    private void loadGraphic() throws IOException {
        image = ImageIO.read(new File("../../" + filepath));
    }

    public int getMapWidth() {
        return mapWidth;
    }

    public int getMapHeight() {
        return mapHeight;
    }

    public BufferedImage getImage() {
        return image;
    }

    public int getWidth() {
        return image.getWidth();
    }

    public int getHeight() {
        return image.getHeight();
    }

    public enum Type {
        CHAPEL,
        PIONEERS_HOUSE1,
        SIGNALFIRE,
        HERBARY,
        BRICKYARD,
        BRICKYARD2,
        OFFICE1,
        MARKETPLACE,
        FORESTERS,
        ;
    }

}
