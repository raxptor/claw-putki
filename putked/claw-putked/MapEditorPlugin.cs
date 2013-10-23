using System;

namespace clawputked
{
	public class MapEditorPlugin : PutkEd.EditorPlugin
	{
		public MapEditorPlugin ()
		{

		}

		public string GetDescription()
		{
			return "Map editor";
		}

		public bool CanEditType(PutkEd.DLLLoader.Types pt)
		{
			return pt.Name == "map";
		}

		public void LaunchEditor(PutkEd.DLLLoader.MemInstance mi)
		{
			MapEditorWindow win = new MapEditorWindow(mi);
			win.Show();
		}
	}
}

