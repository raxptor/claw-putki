package com.claw;

public class Main
{
	public static void main(String[] args) 
	{
		if (System.getProperty("os.name").contains("Windows")) {
			putked.Main.addPluginDesc(
					new putked.DefaultBuildLoader(Main.class, "interop.dll", "claw-data-dll.dll", "claw-databuilder.exe", new String[] {})
				);			
		} else {
			putked.Main.addPluginDesc(
					new putked.DefaultBuildLoader(Main.class, "interop.dylib", "libclaw-data-dll.dylib", "claw-databuilder", new String[] {})
				);
		}
		putked.Main.main(args);
	}
}
