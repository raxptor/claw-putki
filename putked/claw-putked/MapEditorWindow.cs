using System;
using PutkEd;
using Gtk;

namespace clawputked
{
	public partial class MapEditorWindow : Gtk.Window
	{
		MapWidget m_mapWidget;
		inki.map m_map;

		public MapEditorWindow(DLLLoader.MemInstance mi) : base(Gtk.WindowType.Toplevel)
		{
			this.Build ();		

			m_map = DataHelper.CreatePutkEdObj(mi) as inki.map;

			m_mapWidget = new MapWidget(m_map);
			m_scrolledWindow.AddWithViewport(m_mapWidget);
			ShowAll();
            		
			this.Title = "Claw/PutkEd Map Editor";

			// This is for layer selection.
			Gtk.CellRendererText r0 = new Gtk.CellRendererText();
			TreeViewColumn c0 = new TreeViewColumn("LayerName", r0);
			c0.AddAttribute(r0, "text", 0);
			m_layers.InsertColumn(c0, 0);

			// Path
			Gtk.CellRendererText r1 = new Gtk.CellRendererText();
			TreeViewColumn c1 = new TreeViewColumn("Path", r1);
			c1.AddAttribute(r1, "text", 1);
			m_layers.InsertColumn(c1, 1);

			ReloadLayers();
		}

		protected void OnSaveClicked (object sender, EventArgs e)
		{
			m_map.m_mi.DiskSave();
		}

		public void ReloadLayers()
		{
			Gtk.ListStore ls = new Gtk.ListStore (typeof(string), typeof(string));
			for (int i=0;i<m_map.get_layers_size();i++)
			{
				inki.maplayer ml = m_map.resolve_layers(i);
				ls.AppendValues(ml.get_name(), ml.m_mi.GetPath());
			}

			m_layers.Model = ls;
		}
        
        public void OnRowActivated(object o, RowActivatedArgs args)
        {
    
        }

        protected void OnCursorchanged(object sender, EventArgs e)
        {
            TreeModel model;
            TreeIter iter;
            if (m_layers.Selection.GetSelected(out model, out iter))
            {       
                m_mapWidget.EditLayer(model.GetValue(iter, 1).ToString());
            }
        }
	}

}

