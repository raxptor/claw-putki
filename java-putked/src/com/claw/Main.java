package com.claw;

public class Main
{
	public static void main(String[] args) 
	{
		putked.Main.addPluginDesc(
				new putked.DefaultBuildLoader(Main.class, 
						"interop.dylib", "libclaw-data-dll.dylib", "claw-databuilder", new String[] {}
				)
		);
		putked.Main.main(args);
	}
}
