using System;
using PutkEd;

namespace clawputked
{
	public partial class MapEditorWindow : Gtk.Window
	{
		MapWidget m_mapWidget;
		DLLLoader.MemInstance m_mi;

		public MapEditorWindow(DLLLoader.MemInstance mi) : base(Gtk.WindowType.Toplevel)
		{
			this.Build ();		


			m_mi = mi;
			m_mapWidget = new MapWidget(mi);
			m_scrolledWindow.AddWithViewport(m_mapWidget);
			ShowAll();

			this.Title = "Claw/PutkEd Map Editor";
		}

		protected void OnSaveClicked (object sender, EventArgs e)
		{
			m_mi.DiskSave();
		}
	}
}

