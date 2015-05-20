package com.claw;

class DevLoader implements putked.EditorPluginDescription
{
	@Override
	public String getName() { return "DEV Editor"; }
	@Override
	public String getVersion() { return "None"; }
	@Override
	public PluginType getType() { return PluginType.PLUGIN_PROJECT_DEV_BUILD; }
	@Override
	public void start() {
		java.io.File f = new java.io.File("../ext/putki/putked/interopdll/libputked-java-interop.dylib");
		putked.Main.interopInit(f.getAbsolutePath(), "../build/libclaw-data-dll.dylib");
	}
}

public class DevMain
{
	public static void main(String[] args) 
	{
		putked.Main.addPluginDesc(new DevLoader());
		putked.Main.main(args);
	}
}
