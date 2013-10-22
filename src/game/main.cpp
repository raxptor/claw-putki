#include <iostream>

#include <putki/pkgloader.h>
#include <putki/pkgmgr.h>
#include <putki/liveupdate/liveupdate.h>

#include <claw/appwindow.h>
#include <claw/render.h>
#include <claw/log.h>

#include <game/staticdata.h>

#include <cassert>
#include <stdio.h>

// binding up the blob loads.
namespace outki { void bind_claw_loaders(); }

namespace
{
	claw::appwindow::data *window;
	putki::liveupdate::data *liveupdate;
	outki::globalsettings *settings;
	claw::render::data *renderer;
}

void init()
{
	putki::pkgmgr::loaded_package *menu_pkg = putki::pkgloader::from_file("mainmenu.pkg");
}

void frame()
{
	if (LIVE_UPDATE(&settings))
	{
		claw::appwindow::set_title(window, settings->windowtitle);
	}

	claw::render::begin(renderer, true, true, 0xff00ff);

	claw::render::end(renderer);
	claw::render::present(renderer);

	if (liveupdate)
	{
		if (!putki::liveupdate::connected(liveupdate))
		{
			putki::liveupdate::disconnect(liveupdate);
			liveupdate = 0;
		}
		else
		{
			putki::liveupdate::update(liveupdate);
		}
	}	
}

int main(int argc, char *argv[])
{
	outki::bind_claw_loaders();


	putki::liveupdate::init();
	

	game::load_static_package();

	settings = game::get_global_settings();
	assert(settings);

	window = claw::appwindow::create(settings->windowtitle, settings->window_width, settings->window_height);
	renderer = claw::render::create(window);
//	liveupdate = putki::liveupdate::connect();

	init();

	claw::appwindow::run_loop(window, &frame);

	claw::render::destroy(renderer);
	return 0;
}

