#pragma once

#include <outki/types/claw/tiles.h>
#include <kosmos/render/render.h>
#include <kosmos/core/line.h>

namespace claw
{
	namespace level
	{
		struct instance;

		struct draw_info
		{
			float ofsx, ofsy;
		};

		instance *create(outki::map *level);

		bool line_map_intersection(instance *d, float x0, float y0, float x1, float y1, kosmos::intersection *res);

		void draw(instance *, draw_info *di);
		void free(instance *);
	}
}
