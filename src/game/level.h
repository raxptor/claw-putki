#pragma once

#include <outki/types/claw/tiles.h>
#include <claw/render.h>

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
		
		void draw(instance *, draw_info *di, render::data *);
		void free(instance *);
	}
}
