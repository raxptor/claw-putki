package claw;

import putked.*;

public class EditorPlugin implements putked.EditorPluginDescription
{
	@Override
	public String getName() { return "ClawPlugin"; }

	@Override
	public String getVersion() { return "1.0"; }

	@Override
	public PluginType getType() { return PluginType.PLUGIN_EDITOR; }

	@Override
	public void start()
	{
		DataHelper.addTypeService(new putked.inki.Claw());
		putked.Main.addEditor(new MapEditor());
	}
}
