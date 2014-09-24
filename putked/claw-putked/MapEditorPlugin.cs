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
			System.Console.WriteLine ("can edit? " + pt.Name);
			return pt.Name == "map";
		}

		public void LaunchEditor(PutkEd.DLLLoader.MemInstance mi)
		{
			System.Console.WriteLine ("Launch!");
			MapEditorWindow win = new MapEditorWindow(mi);
			win.Show();
		}
	}
}

