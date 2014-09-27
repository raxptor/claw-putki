#pragma once

#include <kosmos/render/render2d.h>
#include <ccg-ui/uicontext.h>

namespace claw
{
	namespace session
	{
		struct instance;

		instance *create();

		void update(instance *i, ccgui::uicontext *,float timedelta);
		void draw(instance *i, kosmos::render2d::stream *stream);

		void free(instance *);
	}
}
