using System;
using System.Collections.Generic;
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
        Dictionary<string, Gdk.Pixbuf> m_loadedTilesets = new Dictionary<string, Gdk.Pixbuf>();

        Gdk.Point m_pickTileLocation = new Gdk.Point(0, 0);
        bool m_pickTile;

        int m_drawTile = 0;
        int m_selectedTile = 0;

        string m_editLayer;

		public MapWidget(inki.map map)
		{
			// Insert initialization code here.
			m_map = map;
			ModifyBg(Gtk.StateType.Normal, new Gdk.Color(64,64,64));
			ModifyFg(Gtk.StateType.Normal, new Gdk.Color(128,128,128));

			AddEvents((int)Gdk.EventMask.PointerMotionMask);
			AddEvents((int)Gdk.EventMask.ButtonPressMask);
            AddEvents((int)Gdk.EventMask.ButtonReleaseMask);

			CalcSize();
		}

        public void EditLayer(string layerPath)
        {
            m_editLayer = layerPath;
            m_drawTile = 0;
            QueueDraw();
        }

		public void CalcSize()
		{
			int maxW = 32;
			int maxH = 32;

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
							int tw = mlg.get_width() * tm.get_tile_width();
							int th = mlg.get_height() * tm.get_tile_height();
							if (tw > maxW)
								maxW = tw;
							if (th > maxH)
								maxH = th;
						}
					}
				}
			}

			// Calculate desired size here.
			SetSizeRequest(maxW, maxH);
		}
	

		public void PaintTile(bool falsetoerase)
		{
			if (m_hoverTile != -1 && m_hl_layer != null)
			{
				if (m_hoverTile < m_hl_layer.get_data_size())
				{
					if (falsetoerase)
						m_hl_layer.set_data(m_hoverTile, m_drawTile);
					else
						m_hl_layer.set_data(m_hoverTile, -1);
					QueueDraw();
				}
			}
		}

        protected override bool OnButtonReleaseEvent(Gdk.EventButton ev)
        {
            if (ev.Button == 2)
            {
                m_pickTile = false;
                if (m_selectedTile != -1)
                    m_drawTile = m_selectedTile;
                QueueDraw();
            }
            return base.OnButtonReleaseEvent(ev);
        }

		protected override bool OnButtonPressEvent(Gdk.EventButton ev)
        {
			if (ev.Device.HasCursor)
			{
                if (ev.Button == 1)
                {
                    PaintTile(true);
                }
                else if (ev.Button == 3)
                {
                    PaintTile(false);
                }
                else if (ev.Button == 2 && m_editLayer != null)
                {
                    m_pickTile = true;
                    m_selectedTile = m_drawTile;
                    QueueDraw();
                }
			}
			return base.OnButtonPressEvent(ev);
		}

        public Gdk.Pixbuf GetTilemapPixbuf(inki.tilemap tl)
        {
            inki.Texture tex = tl.resolve_texture();
            if (tex == null)
                return null;

            string target = "data/res/" + tex.GetSource();
            if (!m_loadedTilesets.ContainsKey(target))
            {
                Gdk.Pixbuf P = new Gdk.Pixbuf(target);
                if (P == null || P.Width < 10)
                {
                    Console.WriteLine("Failed to load [" + target + "]");
                    return null;
                }
                m_loadedTilesets.Add(target, P);
            }

            return m_loadedTilesets[target];
        }

		protected override bool OnMotionNotifyEvent(Gdk.EventMotion evnt)
		{
			if (evnt.Device.HasCursor) 
			{
                for (int i=0; i<m_map.get_layers_size(); i++)
                {
                    inki.maplayer ml = m_map.resolve_layers(i);

                    if (ml == null || ml.m_mi.GetPath() != m_editLayer)
                        continue;
                   
                    inki.maplayer_graphics mlg = ml as inki.maplayer_graphics;
                    if (mlg == null)
                        continue;

                    inki.tilemap tm = mlg.resolve_tiles();
                    if (tm == null)
                        continue;

                    Gdk.Pixbuf p = GetTilemapPixbuf(tm);
                    if (p == null)
                        continue;

                    if (!m_pickTile)
                    {
                        // Center tile picker
                        int stride = p.Width / tm.get_tile_width();
                        int tpx = m_drawTile % stride;
                        int tpy = m_drawTile / stride;

                        m_pickTileLocation.X = (int)evnt.X - tpx * tm.get_tile_width() - tm.get_tile_width() / 2;
                        m_pickTileLocation.Y = (int)evnt.Y - tpy * tm.get_tile_height() - tm.get_tile_height() / 2;

                        int tile_x = (int)evnt.X / tm.get_tile_width();
                        int tile_y = (int)evnt.Y / tm.get_tile_height();

                        int hovertile = -1;

                        if (tile_x < mlg.get_width() && tile_y < mlg.get_height())
                            hovertile = tile_y * mlg.get_width() + tile_x;						

                        if (m_hoverTile != hovertile)
                        {
                            m_hoverTile = hovertile;

                            QueueDrawArea(m_hl_x0 - 2, m_hl_y0 - 2, (m_hl_x1 - m_hl_x0) + 4, (m_hl_y1 - m_hl_y0) + 4);

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

                            QueueDrawArea(m_hl_x0 - 2, m_hl_y0 - 2, (m_hl_x1 - m_hl_x0) + 4, (m_hl_y1 - m_hl_y0) + 4);
                        }

                        if ((evnt.State & Gdk.ModifierType.Button1Mask) != 0 &&
                                (evnt.State & Gdk.ModifierType.ShiftMask) != 0)
                        {
                            PaintTile(true);
                        }

                        if ((evnt.State & Gdk.ModifierType.Button3Mask) != 0 &&
                                (evnt.State & Gdk.ModifierType.ShiftMask) != 0)
                        {
                            PaintTile(false);
                        }
                    }
                    else
                    {
                        // Pick tile.
                        int selX = (int)(evnt.X - m_pickTileLocation.X) / tm.get_tile_width();
                        int selY = (int)(evnt.Y - m_pickTileLocation.Y) / tm.get_tile_height();
                        int countX = p.Width / tm.get_tile_width();
                        int countY = p.Height / tm.get_tile_height();
                        if (selX >= 0 && selX < countX && selY >= 0 && selY < countY)
                        {
                            m_selectedTile = countY * selY + selX;
                            QueueDrawArea(m_pickTileLocation.X - 1, m_pickTileLocation.Y - 1, p.Width + 2, p.Height + 2);
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

            Gdk.Pixbuf p = GetTilemapPixbuf(tl); 

			// how many tiles per row in the bitmap
			int tilesW = p.Width / tl.get_tile_width();

			int tilewidth = tl.get_tile_width();
			int tileheight = tl.get_tile_height();

            if (layer.m_mi.GetPath() == m_editLayer)
            {
                for (int x=0; x<=layer.get_width(); x++)
                {
                    int cx = x * tilewidth;
                    if (!(cx < ev.Area.Left || cx > ev.Area.Right))
                        GdkWindow.DrawLine(g, cx, 0, cx, tileheight * layer.get_height());
                }
                for (int y=0; y<=layer.get_height(); y++)
                {
                    int cy = y * tileheight;
                    if (!(cy < ev.Area.Top || cy > ev.Area.Bottom))
                        GdkWindow.DrawLine(g, 0, cy, tilewidth * layer.get_width(), cy);
                }
            }
           

			// should really be equals to get_width / get_height
            int maxData = layer.get_width() * layer.get_width();
            while (layer.get_data_size() < maxData)
            {
                layer.data_push_back();
                layer.set_data(layer.get_data_size() - 1, -1);
            }

            int tileHeight = tl.get_tile_height();
            int tileWidth = tl.get_tile_width();
            int layerWidth = layer.get_width();

			for (int y=0;y<layer.get_height();y++)
			{
                int y0 = y * tileHeight;
                int y1 = y0 + tileHeight;

				int dataIndex = y * layer.get_width();

				if (y1 < ev.Area.Top)
					continue;
				if (y0 > ev.Area.Bottom)
					continue;

				for (int x=0;x<layerWidth;x++)
				{
                    int x0 = x * tileWidth;
                    int x1 = x0 + tileWidth;

                    if (x1 < ev.Area.Left || x0 > ev.Area.Right)
                    {
                        dataIndex++;
                        continue;
                    }
				
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

            if (m_pickTile && m_editLayer == layer.m_mi.GetPath())
            {
                int lx = m_pickTileLocation.X;
                int ly = m_pickTileLocation.Y;
          
                g.RgbFgColor = new Gdk.Color(0, 0, 0);
                GdkWindow.DrawRectangle(g, true, new Gdk.Rectangle(lx - 4, ly - 4, p.Width + 8, p.Height + 8));
                GdkWindow.DrawPixbuf(g, p, 0, 0, lx, ly, p.Width, p.Height, Gdk.RgbDither.None, 0, 0);

                int countX = p.Width / tl.get_tile_width();
                int pickX = m_selectedTile % countX;
                int pickY = m_selectedTile / countX;

                g.RgbFgColor = new Gdk.Color(255, 0, 255);
                GdkWindow.DrawRectangle(g, false, new Gdk.Rectangle(lx + pickX * tl.get_tile_width(), ly + pickY * tl.get_tile_height(), tl.get_tile_width(), tl.get_tile_height()));
            }
		}	

		protected override bool OnExposeEvent(Gdk.EventExpose ev)
		{			
            bool ret = base.OnExposeEvent(ev);

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
                    // stop rendering above the selected edit layer.
                    if (mlg.m_mi.GetPath() == m_editLayer)
                    {
                        break;
                    }
				}
			}

			if (m_hoverTile != -1 && !m_pickTile)
			{
				Gdk.GC g = new Gdk.GC(GdkWindow);
				g.RgbFgColor = new Gdk.Color(192,192,192);
				GdkWindow.DrawRectangle(g, false, m_hl_x0, m_hl_y0, m_hl_x1 - m_hl_x0, m_hl_y1 - m_hl_y0);
			}

			return ret;
		}

		protected override void OnSizeAllocated (Gdk.Rectangle allocation)
		{
			base.OnSizeAllocated (allocation);
			// Insert layout code here.
		}
	}
}

