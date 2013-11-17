#include <game/level.h>
#include <putki/pkgloader.h>
#include <putki/pkgmgr.h>

#include <claw/render.h>
#include <claw/log.h>

#include <math.h>

namespace claw
{
	namespace session
	{
		struct instance
		{
			putki::pkgmgr::loaded_package *level_pkg;
			claw::level::instance *level;
			claw::level::draw_info leveldraw;
			float time;
		};
		
		void start_level(instance *i, const char *package, const char *level)
		{
			i->level_pkg = putki::pkgloader::from_file(package);
			i->level = level::create((outki::map*)(putki::pkgmgr::resolve(i->level_pkg, level)));
		}
		
		instance *create()
		{
			instance *i = new instance();
			i->time = 0;
			
			start_level(i, "testmap.pkg", "maps/testmap/testmap");
			return i;
		}
		
		void free(instance *i)
		{
			delete i;
		}
		
		void update(instance *i, float deltatime)
		{
			i->time += deltatime;
			i->leveldraw.ofsx = 0; //floorf(sinf(0.4f * i->time) * 300);
			i->leveldraw.ofsy = 0; //floorf(cosf(0.4f * i->time) * 100);
		}
		
		void draw(instance *i, claw::render::data *renderer)
		{
			level::draw(i->level, &i->leveldraw, renderer);
		}
	}
}
