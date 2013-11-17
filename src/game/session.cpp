#include <game/level.h>
#include <putki/pkgloader.h>
#include <putki/pkgmgr.h>

#include <claw/render.h>
#include <claw/log.h>

#include <ccg-ui/uicontext.h>
#include <claw/core/line.h>

#include <vector>

#include <math.h>

namespace claw
{
	namespace session
	{
		struct particle
		{
			float px, py;
			float vx, vy;
		};
	
		struct instance
		{
			putki::pkgmgr::loaded_package *level_pkg;
			claw::level::instance *level;
			claw::level::draw_info leveldraw;
			float time;
			
			std::vector<particle> particles;
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
		
		void update(instance *i, ccgui::uicontext *ctx, float deltatime)
		{
			i->time += deltatime;
			i->leveldraw.ofsx = 0; //floorf(sinf(0.4f * i->time) * 300);
			i->leveldraw.ofsy = 0; //floorf(cosf(0.4f * i->time) * 100);
			
			if (ctx->input.mouse->primary.wentDown)
			{
				printf("went down at %f %f\n", ctx->input.mouse->x, ctx->input.mouse->y);
				particle p;
				p.px = ctx->input.mouse->x;
				p.py = ctx->input.mouse->y;
				p.vx = 0;
				p.vy = 0;
				i->particles.push_back(p);
			}
		}
		
		void draw(instance *d, claw::render::data *renderer)
		{
			level::draw(d->level, &d->leveldraw, renderer);
			
			for (unsigned int i=0;i<d->particles.size();i++)
			{
				particle *p = &d->particles[i];
				
				for (float a=0;a<6.28;a += 0.01f)
				{
					float x1 = p->px + 500.0f * cosf(a + 0.01f * d->time);
					float y1 = p->py + 500.0f * sinf(a + 0.01f * d->time);
					
					
					claw::intersection perp;
					if (level::line_map_intersection(d->level, p->px, p->py, x1, y1, &perp))
					{
						x1 = perp.x;
						y1 = perp.y;
					}

					render::line(renderer, p->px, p->py, x1, y1, 0xff44ff);
				}
				
//				render::line(renderer, p->px - 4, p->py, p->px + 4, p->py, 0xffff00);
//				render::line(renderer, p->px, p->py - 4, p->px, p->py + 4, 0xffff00);
			}
		}
	}
}
