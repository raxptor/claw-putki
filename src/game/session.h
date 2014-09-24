#pragma once

#include <kosmos/render/render.h>
#include <ccg-ui/uicontext.h>

namespace claw
{
	namespace session
	{
		struct instance;

		instance *create();

		void update(instance *i, ccgui::uicontext *,float timedelta);
		void draw(instance *i);

		void free(instance *);
	}
}
