using System;
using PutkEd;

namespace clawputked
{
	public partial class MapEditorWindow : Gtk.Window
	{
		public MapEditorWindow(DLLLoader.MemInstance mi) : base(Gtk.WindowType.Toplevel)
		{
			this.Build ();

			Add(new MapWidget(mi));
			ShowAll();

			this.Title = "Claw/PutkEd Map Editor";
		}
	}
}

