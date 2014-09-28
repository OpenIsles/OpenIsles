package org.openisles.tools.renderingorder.gui;

import edu.uci.ics.jung.algorithms.layout.Layout;
import edu.uci.ics.jung.algorithms.layout.StaticLayout;
import edu.uci.ics.jung.visualization.VisualizationViewer;
import edu.uci.ics.jung.visualization.control.DefaultModalGraphMouse;
import edu.uci.ics.jung.visualization.control.ModalGraphMouse;
import edu.uci.ics.jung.visualization.renderers.Renderer;
import org.apache.commons.collections15.Transformer;
import org.openisles.tools.renderingorder.RenderingOrder;
import org.openisles.tools.renderingorder.map.MapObject;
import org.openisles.tools.renderingorder.map.OrderingGraph;

import javax.swing.*;
import java.awt.*;
import java.awt.geom.Point2D;

public class GraphPanel extends JPanel {

    private OrderingGraph orderingGraph;
    private VisualizationViewer<MapObject, Object> visualizationViewer;

    public GraphPanel() {
        orderingGraph = new OrderingGraph(RenderingOrder.mapObjects);

        Layout<MapObject, Object> graphLayout = new StaticLayout<MapObject, Object>(orderingGraph) {
            @Override
            public Point2D transform(MapObject mapObject) {
                return new Point2D.Double(40 + mapObject.getMapX()*50, 40 + mapObject.getMapY()*50);
            }
        };

        visualizationViewer = new VisualizationViewer<MapObject, Object>(graphLayout);
        add(visualizationViewer);

        DefaultModalGraphMouse graphMouse = new DefaultModalGraphMouse();
        graphMouse.setMode(ModalGraphMouse.Mode.TRANSFORMING);
        visualizationViewer.setGraphMouse(graphMouse);

        visualizationViewer.getRenderer().getVertexLabelRenderer().setPosition(Renderer.VertexLabel.Position.CNTR);
        visualizationViewer.getRenderContext().setVertexFillPaintTransformer(new Transformer<MapObject, Paint>() {
            @Override
            public Paint transform(MapObject mapObject) {
                return new Color(0.8f, 0.8f, 1.0f);
            }
        });
        visualizationViewer.getRenderContext().setVertexLabelTransformer(new Transformer<MapObject, String>() {
            @Override
            public String transform(MapObject mapObject) {
                return String.valueOf(mapObject.getDrawingIndex());
            }
        });
    }

    public void orderMapObjects() {
        orderingGraph.orderMapObjects();
        visualizationViewer.repaint();
    }
}
