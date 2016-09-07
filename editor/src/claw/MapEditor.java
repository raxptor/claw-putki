package claw;

import javafx.scene.paint.Color;
import javafx.scene.transform.Affine;

import java.awt.geom.AffineTransform;
import java.util.HashMap;

import javafx.event.EventHandler;
import javafx.geometry.Point2D;
import javafx.scene.Node;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import putked.Editor;
import putked.Main;
import putked.ObjectEditor;
import putked.inki.Claw;
import putked.inki.Kosmos;
import putked.DataHelper;
import putked.DataObject;;

public class MapEditor implements Editor
{
	@Override
	public String getName()
	{
		return "MapEditor";
	}

	@Override
	public int getPriority()
	{
		return -1;
	}

	@Override
	public boolean canEdit(putki.Compiler.ParsedStruct type)
	{
		return type.hasParent(Main.s_compiler.getTypeByName("Map"));
	}

	public class LayerCanvas extends Canvas
	{
		Claw.Map m_tilemap;
		boolean m_isDragging = false;
		double m_dragStartX, m_dragStartY;
		double m_originX = 0, m_originY = 0;
		double m_scale = 1.0f;
		int m_tileHLX = -1, m_tileHLY = -1;
		int m_editLayer = 0;
		Affine m_mapTransform = new Affine();

		boolean isPan(MouseEvent evt)
		{
			return (evt.isShiftDown() && evt.isPrimaryButtonDown() || evt.isMiddleButtonDown());
		}

		public LayerCanvas(Claw.Map tilemap)
		{
			m_tilemap = tilemap;
			setMouseTransparent(false);

			setOnMousePressed(new EventHandler<MouseEvent>() {
				public void handle(MouseEvent evt)
				{
					if (isPan(evt))
					{
						m_dragStartX = evt.getX();
						m_dragStartY = evt.getY();
						m_isDragging = true;
					}
				}
			});
			setOnMouseMoved(new EventHandler<MouseEvent>() {
				public void handle(MouseEvent evt)
				{
					int oldx = m_tileHLX;
					int oldy = m_tileHLY;
					if (!isPan(evt))
					{
						m_tileHLX = -1;
						m_tileHLY = -1;
						Claw.MapLayerGraphics editLayer = getEditLayer();
						if (editLayer != null && editLayer.getTiles() != null)
						{
							try
							{
								Point2D pt = m_mapTransform.inverseTransform(new Point2D(evt.getX(), evt.getY()));
								m_tileHLX = (int)(pt.getX() / editLayer.getTiles().getTileWidth());
								m_tileHLY = (int)(pt.getY() / editLayer.getTiles().getTileWidth());
								if (m_tileHLX >= editLayer.getWidth() || m_tileHLY >= editLayer.getHeight() || m_tileHLY < 0)
									m_tileHLX = -1;
							}
							catch (Exception e)
							{

							}
						}
					}
					if (m_tileHLX != oldx || m_tileHLY != oldy)
					{
						draw();
					}
				}
			});
			setOnMouseDragged(new EventHandler<MouseEvent>() {
				public void handle(MouseEvent evt)
				{
					if (isPan(evt) && m_isDragging)
					{
						m_originX -= (evt.getX() - m_dragStartX);
						m_originY -= (evt.getY() - m_dragStartY);
						m_dragStartX = evt.getX();
						m_dragStartY = evt.getY();
						draw();
					}
					else
					{
						m_isDragging = false;
					}
				}
			});
		}

		Claw.MapLayerGraphics getEditLayer()
		{
			if (m_editLayer < m_tilemap.getLayersSize())
			{
				Claw.MapLayer layer = m_tilemap.getLayers(m_editLayer);
				if (layer != null)
				{
					if (layer instanceof Claw.MapLayerGraphics)
					{
						return (Claw.MapLayerGraphics) layer;
					}
				}
			}
			return null;
		}


		@Override
		public double minHeight(double width)
		{
		    return 64;
		}

		@Override
		public double maxHeight(double width)
		{
		    return 1000;
		}

		@Override
		public double prefHeight(double width)
		{
		    return minHeight(width);
		}

		@Override
		public double minWidth(double height)
		{
		    return 0;
		}

		@Override
		public double maxWidth(double height)
		{
		    return 10000;
		}

		@Override
		public boolean isResizable()
		{
		    return true;
		}

		@Override
		public void resize(double width, double height)
		{
		    super.setWidth(width);
		    super.setHeight(height);
		    draw();
		}

		HashMap<String, Image> m_textureCache = new HashMap<String, Image>();

		public void draw()
		{
			GraphicsContext ctx = getGraphicsContext2D();
			ctx.setTransform(new Affine());
			ctx.setFill(new Color(0,0,0,1));
			ctx.fillRect(0, 0, getWidth(), getHeight());

			m_mapTransform = new Affine();
			m_mapTransform.appendTranslation(-m_originX, -m_originY);
			ctx.setTransform(m_mapTransform);

			int layers = m_tilemap.getLayersSize();
			for (int i=0;i<layers;i++)
			{
				Claw.MapLayer layer = m_tilemap.getLayers(i);
				if (layer != null)
				{
					if (layer instanceof Claw.MapLayerGraphics)
					{
						Claw.MapLayerGraphics gl = (Claw.MapLayerGraphics) layer;
						Claw.TileMap tiles = gl.getTiles();
						if (tiles != null)
						{
							Kosmos.Texture tex = tiles.getTexture();
							Image img = null;
							if (tex != null)
							{
								String source = Main.s_instance.translateResPathURI(tex.getSource());
								if (!m_textureCache.containsKey(source))
								{
									System.out.println("Loading texture [" + source + "]");
									Image load = new Image(source, false);
									m_textureCache.put(source, load);
								}
								img = m_textureCache.get(source);
							}
							int tileWidth = tiles.getTileWidth();
							int tileHeight = tiles.getTileHeight();

							int tilesX = gl.getWidth();
							int tilesY = gl.getHeight();

							int dataLen = gl.getDataSize();
							for (int y=0;y<tilesY;y++)
							{
								for (int x=0;x<tilesX;x++)
								{
									int tile = y * tilesX + x;
									if (tile < dataLen)
									{
										int piece = gl.getData(tile);
										if (piece >= 0)
										{
											double tw = tiles.getTileWidth();
											double th = tiles.getTileHeight();
											int texTilesX = (int)(img.getWidth() / tw);
											int u0 = piece % texTilesX;
											int v0 = piece / texTilesX;
											ctx.drawImage(img, tw * u0, th * v0, tw, th, x*tileWidth, y*tileHeight, tw, th);
										}
									}
								}
							}

							ctx.setStroke(new Color(0.3,0.3,0.3,1));
							for (int y=1;y<tilesY;y++)
							{
								ctx.strokeLine(0,  y*tileHeight, tilesX*tileWidth, y*tileHeight);
							}
							for (int x=1;x<tilesX;x++)
							{
								ctx.strokeLine(x*tileWidth, 0, x*tileWidth, tilesY*tileHeight);
							}
							ctx.strokeRect(0, 0, tilesX*tileWidth, tilesY*tileHeight);


							if (m_editLayer == i && m_tileHLX >= 0)
							{
								ctx.setStroke(new Color(1,1,1,1));
								ctx.strokeRect(m_tileHLX * tileWidth, m_tileHLY * tileHeight,
											   tileWidth, tileHeight);
							}



						}
					}
				}
			}
		}
	}

	LayerCanvas m_canvas;

	@Override
	public Node createUI(DataObject obj)
	{
		putked.ProxyObject wrap = DataHelper.createPutkEdObj(obj);
		Claw.Map tilemap = (Claw.Map) wrap;
		if (tilemap == null)
		{
			return null;
		}

		HBox hbox = new HBox();
		m_canvas = new LayerCanvas(tilemap);

		ObjectEditor oe = new ObjectEditor(obj);
		hbox.getChildren().add(m_canvas);
		hbox.getChildren().add(oe.getRoot());
		HBox.setHgrow(m_canvas,  Priority.ALWAYS);
		m_canvas.draw();
		return hbox;
	}
}
