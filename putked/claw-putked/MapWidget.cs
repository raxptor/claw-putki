using System;
using PutkEd;

namespace clawputked
{
	[System.ComponentModel.ToolboxItem (true)]
	public class MapWidget : Gtk.DrawingArea
	{
		inki.map m_map;

		int m_hoverTile = -1;
		int m_hl_x0, m_hl_y0, m_hl_x1, m_hl_y1;
		inki.maplayer_graphics m_hl_layer;

		public MapWidget(DLLLoader.MemInstance mi)
		{
			// Insert initialization code here.
			m_map = DataHelper.CreatePutkEdObj(mi) as inki.map;
			ModifyBg(Gtk.StateType.Normal, new Gdk.Color(64,64,64));
			ModifyFg(Gtk.StateType.Normal, new Gdk.Color(128,128,128));

			AddEvents((int)Gdk.EventMask.PointerMotionMask);
			AddEvents((int)Gdk.EventMask.ButtonPressMask);
		}

		protected override bool OnButtonPressEvent (Gdk.EventButton ev)
		{
			Console.WriteLine("Set on " + m_hoverTile);
			if (ev.Device.HasCursor)
			{
				if (m_hoverTile != -1 && m_hl_layer != null)
				{
					if (m_hoverTile < m_hl_layer.get_data_size())
					{
						m_hl_layer.set_data(m_hoverTile, 1);
						QueueDraw();
					}
				}
			}

			return base.OnButtonPressEvent (ev);		
		}

		protected override bool OnMotionNotifyEvent(Gdk.EventMotion evnt)
		{
			if (evnt.Device.HasCursor)
			{
				for (int i=0;i<m_map.get_layers_size();i++)
				{
					inki.maplayer ml = m_map.resolve_layers(i);
					if (ml != null)
					{
						inki.maplayer_graphics mlg = ml as inki.maplayer_graphics;
						if (mlg != null)
						{
							inki.tilemap tm = mlg.resolve_tiles();
							if (tm != null)
							{
								int tile_x = (int)evnt.X / tm.get_tile_width();
								int tile_y = (int)evnt.Y / tm.get_tile_height();

								int hovertile = -1;

								if (tile_x < mlg.get_width() && tile_y < mlg.get_height())
									 hovertile = tile_y * mlg.get_width() + tile_x;

								if (m_hoverTile != hovertile)
								{
									m_hoverTile = hovertile;

									if (hovertile != -1)
									{
										m_hl_layer = mlg;
										m_hl_x0 = tile_x * tm.get_tile_width();
										m_hl_y0 = tile_y * tm.get_tile_height();
										m_hl_x1 = (tile_x + 1) * tm.get_tile_width();
										m_hl_y1 = (tile_y + 1) * tm.get_tile_height();
									}
									else
									{
										m_hl_layer = null;
									}

									QueueDraw();
								}
							}
						}
					}
				}
			}

			return base.OnMotionNotifyEvent(evnt);
		}

		private void DrawGraphicsLayer(inki.maplayer_graphics layer, Gdk.EventExpose ev)
		{
			Gdk.GC g = new Gdk.GC(GdkWindow);
			g.RgbFgColor = new Gdk.Color(32,32,32);

			inki.tilemap tl = layer.resolve_tiles();
			if (tl == null)
				return;

			inki.Texture tex = tl.resolve_texture();
			if (tex == null)
				return;

			Gdk.Pixbuf p = new Gdk.Pixbuf("data/res/" + tex.GetSource());
			if (p.Width < 10)
				return;

			// how many tiles per row in the bitmap
			int tilesW = p.Width / tl.get_tile_width();

			int tilewidth = tl.get_tile_width();
			int tileheight = tl.get_tile_height();

			for (int x=0;x<=layer.get_width();x++)
				GdkWindow.DrawLine(g, x * tilewidth, 0, x * tilewidth, tileheight * layer.get_height());
			for (int y=0;y<=layer.get_height();y++)
				GdkWindow.DrawLine(g, 0, y * tileheight, tilewidth * layer.get_width(), y * tileheight);

			// should really be equals to get_width / get_height
			int maxData = layer.get_data_size();

			g.RgbFgColor = new Gdk.Color(255,255,255);

			for (int y=0;y<layer.get_height();y++)
			{
				int y0 = y * tl.get_tile_height();
				int y1 = y0 + tl.get_tile_height();

				int dataIndex = y * layer.get_width();

				if (y1 < ev.Area.Top)
					continue;
				if (y0 > ev.Area.Bottom)
					continue;

				for (int x=0;x<layer.get_width();x++)
				{
					int x0 = x * tl.get_tile_width();
					int x1 = x0 + tl.get_tile_width();

					if (x1 < ev.Area.Left || x1 > ev.Area.Right)
						continue;
				
					bool highLighted = (m_hoverTile == dataIndex);

					if (dataIndex < maxData)
					{
						int tileIndex = layer.get_data(dataIndex++);
						if (tileIndex >= 0)
						{

							int tx = (tileIndex % tilesW) * tilewidth;
							int ty = (tileIndex / tilesW) * tileheight;

							GdkWindow.DrawPixbuf(g, p, tx, ty, x0, y0, tilewidth, tileheight, Gdk.RgbDither.None, 0, 0);
						}
					}				
				}
			}
		}	

		protected override bool OnExposeEvent(Gdk.EventExpose ev)
		{
			base.OnExposeEvent (ev);

			for (int i=0;i<m_map.get_layers_size();i++)
			{
				inki.maplayer ml = m_map.resolve_layers(i);
				if (ml != null)
				{
					inki.maplayer_graphics mlg = ml as inki.maplayer_graphics;
					if (mlg != null)
					{
						DrawGraphicsLayer(mlg, ev);
					}
				}
			}

			if (m_hoverTile != -1)
			{
				Gdk.GC g = new Gdk.GC(GdkWindow);
				g.RgbFgColor = new Gdk.Color(192,192,192);
				GdkWindow.DrawRectangle(g, false, m_hl_x0, m_hl_y0, m_hl_x1 - m_hl_x0, m_hl_y1 - m_hl_y0);
			}

			return true;
		}

		protected override void OnSizeAllocated (Gdk.Rectangle allocation)
		{
			base.OnSizeAllocated (allocation);
			// Insert layout code here.
		}

		protected override void OnSizeRequested (ref Gtk.Requisition requisition)
		{
			// Calculate desired size here.
			requisition.Height = 128;
			requisition.Width = 128;
		}
	}
}

