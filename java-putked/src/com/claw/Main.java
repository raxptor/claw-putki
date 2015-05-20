package com.claw;

public class Main
{
	public static void main(String[] args) 
	{
		putked.Main.addPluginDesc(
				new putked.DefaultBuildLoader(Main.class, 
						"interop.dylib", "libclaw-data-dll.dylib", new String[] {}
				)
		);
		putked.Main.main(args);
	}
}
