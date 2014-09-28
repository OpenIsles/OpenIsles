package org.openisles.tools.renderingorder.gui;

import org.openisles.tools.renderingorder.RenderingOrder;
import org.openisles.tools.renderingorder.map.MapObject;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.util.Collections;
import java.util.Comparator;

public class MainWindow extends JFrame {

    private GraphPanel graphPanel;
    private MapPanel mapPanel;
    private JSplitPane splitPane;


    public MainWindow() throws HeadlessException, IOException {
        // Fenster
        super("OpenIsles - RenderingOrder");

        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        setSize(1500, 1000);

        // Komponenten
        graphPanel = new GraphPanel();
        graphPanel.setSize(new Dimension(750, 1000));
        mapPanel = new MapPanel();
        mapPanel.setSize(new Dimension(750, 1000));

        splitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, mapPanel, graphPanel);
        splitPane.setDividerLocation(750);
        setContentPane(splitPane);

        // Menü
        MenuItem menuItem = new MenuItem("Sort mapObjects");
        menuItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                long s = System.currentTimeMillis();

                graphPanel.orderMapObjects();

                Collections.sort(RenderingOrder.mapObjects, new Comparator<MapObject>() {
                    @Override
                    public int compare(MapObject o1, MapObject o2) {
                        return Integer.valueOf(o1.getDrawingIndex()).compareTo(o2.getDrawingIndex());
                    }
                });

                long e = System.currentTimeMillis();
                System.out.println("reordered in " + (e - s) + " milliseconds.");

                mapPanel.repaint();
            }
        });

        Menu menu = new Menu("Menu");
        menu.add(menuItem);

        MenuBar menuBar = new MenuBar();
        menuBar.add(menu);

        setMenuBar(menuBar);
    }
}
