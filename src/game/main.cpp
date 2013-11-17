#include <iostream>

#include <putki/pkgloader.h>
#include <putki/pkgmgr.h>
#include <putki/liveupdate/liveupdate.h>

#include <claw/appwindow.h>
#include <claw/render.h>
#include <claw/log.h>
#include <claw/ccgui-renderer.h>

#include <game/session.h>

#include <ccg-ui/uiscreen.h>

#include <outki/types/ccg-ui/Elements.h>
#include <outki/types/ccg-ui/Screen.h>
#include <outki/types/claw/tiles.h>

#include <game/staticdata.h>

#include <cassert>
#include <stdio.h>

// binding up the blob loads.
namespace outki 
{
	void bind_claw_loaders(); 
	void bind_ccg_ui_loaders(); 
}

namespace
{
	claw::appwindow::data *window;
	putki::liveupdate::data *liveupdate;
	outki::globalsettings *settings;
	claw::render::data *renderer;
	claw::claw_ui_renderer *ui_renderer;
	ccgui::uiscreen::instance *s_current_screen = 0;
	ccgui::uicontext s_ui_context;
	
	claw::session::instance *session = 0;
}

void init()
{
	putki::pkgmgr::loaded_package *menu_pkg = putki::pkgloader::from_file("mainmenu.pkg");	
	outki::UIScreen *screen = (outki::UIScreen*) putki::pkgmgr::resolve(menu_pkg, "ui/mainmenu/screen");

	if (screen)
		s_current_screen = ccgui::uiscreen::create(screen, ui_renderer, 0);
}

void frame(claw::appwindow::input_batch *input, float deltatime)
{
	if (LIVE_UPDATE(&settings))
	{
		claw::appwindow::set_title(window, settings->windowtitle);
	}

	claw::render::begin(renderer, true, true, 0xff00ff);
	
	/*
	int w, h;
	if (claw::render::get_size(renderer, &w, &h))
	{
		s_ui_context.input.mouse = &input->mouse;
		
		ccgui::uiscreen::draw(s_current_screen, &s_ui_context, 0, 0, (float)w, (float)h);
	}
	*/

	claw::session::update(session, deltatime);
	claw::session::draw(session, renderer);

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
	outki::bind_ccg_ui_loaders();


	putki::liveupdate::init();

	game::load_static_package();

	settings = game::get_global_settings();
	assert(settings);

	session = claw::session::create();


	window = claw::appwindow::create(settings->windowtitle, settings->window_width, settings->window_height);

	renderer = claw::render::create(window);

	ui_renderer = new claw::claw_ui_renderer(renderer);

	liveupdate = putki::liveupdate::connect();

	init();


	claw::appwindow::run_loop(window, &frame);

	claw::session::free(session);
	claw::render::destroy(renderer);
	return 0;
}

