#include <iostream>

#include <squirrel.h>

#include <putki/pkgloader.h>
#include <putki/pkgmgr.h>
#include <putki/config.h>
#include <putki/liveupdate/liveupdate.h>

#include <environment/appwindow.h>
#include <kosmos/render/render.h>
#include <kosmos/render/render2d.h>
#include <kosmos/render/shader.h>
#include <kosmos/log/log.h>

#include <game/session.h>

#include <ccg-ui/uiscreen.h>

#include <outki/types/ccg-ui/Elements.h>
#include <outki/types/ccg-ui/Screen.h>
#include <outki/types/claw/tiles.h>

#include <game/staticdata.h>

#include <cassert>
#include <stdio.h>

#include <kosmos/glwrap/gl.h>

#include <ccg-ui/glyphcache.h>



// binding up the blob loads.
namespace outki
{
	void bind_kosmos();
	void bind_claw();
	void bind_ccg_ui();
}

namespace
{
	claw::appwindow::data *window;
	putki::liveupdate::data *liveupdate;
	outki::globalsettings *settings;
	ccgui::uiscreen::instance *s_current_screen = 0;
	ccgui::uicontext s_ui_context;
	claw::session::instance *session = 0;
}

kosmos::shader::program *prog = 0;
kosmos::render2d::stream *stream = 0;
ccgui::glyphcache::data *glyphcache = 0;

void init()
{
	putki::pkgmgr::loaded_package *menu_pkg = putki::pkgloader::from_file("mainmenu.pkg");
	outki::ui_screen *screen = (outki::ui_screen*) putki::pkgmgr::resolve(menu_pkg, "ui/mainmenu/screen");

	if (screen)
	{
		s_current_screen = ccgui::uiscreen::create(screen, 0);
	}

	glyphcache = ccgui::glyphcache::create(1024, 1024);
}

void frame(claw::appwindow::input_batch *input, float deltatime)
{
	if (LIVE_UPDATE(&settings))
	{
		claw::appwindow::set_title(window, settings->windowtitle);
	}

	s_ui_context.input.mouse = &input->mouse;

	int x0, y0, x1, y1;
	claw::appwindow::get_client_rect(window, &x0, &y0, &x1, &y1);
	
	kosmos::render::begin(x1-x0, y1-x0, true, true, 0xffffff);
	
	if (!stream)
	{
		stream = kosmos::render2d::stream_create(4096);
	}
	
	kosmos::render2d::set_viewport(stream, x1-x0, y1-y0);
	kosmos::render2d::use_programs(stream,
		 kosmos::shader::program_get(settings->shader_solid),
		 kosmos::shader::program_get(settings->shader_texture)
	);
	
	claw::session::update(session, &s_ui_context, deltatime);
	claw::session::draw(session, stream);

	ccgui::uiscreen::draw(s_current_screen, stream, glyphcache, &s_ui_context, 0, 0, (float)(x1-x0), (float)(y1-y0));

	kosmos::render2d::stream_done(stream);
	kosmos::render::end();
	

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



void squirrel_print_func(HSQUIRRELVM vm, const SQChar *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

int main(int argc, char *argv[])
{
	KOSMOS_INFO("Launching claw [SQ: " << SQUIRREL_VERSION << "]");

	outki::bind_kosmos();
	outki::bind_claw();
	outki::bind_ccg_ui();
	putki::liveupdate::init();
	game::load_static_package();

	settings = game::get_global_settings();
	assert(settings);
	
		
	HSQUIRRELVM vm = sq_open(4096);

	sq_setprintfunc(vm, squirrel_print_func, 0);

	const char *buf = "::print(\"Gurken Schmidt\\n\");";
	if (SQ_FAILED(sq_compilebuffer(vm, buf, strlen(buf), "gurka", false))) {
		KOSMOS_ERROR("Compilation of main script failed");
	} else {
		sq_pushroottable(vm); 
		if (SQ_FAILED(sq_call(vm, 1, false, false))) {
			KOSMOS_ERROR("Call of main script failed");
		}
	}

	sq_close(vm);
	
	session = claw::session::create();

	// icon
	const char *iconFile = kosmos::render::get_texture_file_path(settings->icon);
	if (iconFile)
	{
		char buf[1024];
		iconFile = putki::format_file_path(iconFile, buf);
	}

	window = claw::appwindow::create(settings->windowtitle, settings->window_width, settings->window_height, iconFile);


	liveupdate = putki::liveupdate::connect();
	init();


	claw::appwindow::run_loop(window, &frame);

	ccgui::uiscreen::free(s_current_screen);

	claw::session::free(session);
	return 0;
}
