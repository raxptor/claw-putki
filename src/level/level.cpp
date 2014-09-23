#include "level.h"

#include <claw/log.h>
#include <claw/render.h>

#include <putki/liveupdate/liveupdate.h>
#include <memory.h>

#include <iostream>

namespace claw
{
	namespace level
	{
		enum
		{
			MAX_LAYERS = 512
		};

		struct instance
		{
			outki::map *level;
			render::loaded_texture *layer_textures[MAX_LAYERS];
		};

		instance *create(outki::map *level)
		{
			instance *i = new instance();
			i->level = level;

			memset(i->layer_textures, 0x00, sizeof(render::loaded_texture*) * MAX_LAYERS);
			return i;
		}

		void free(instance *i)
		{
			delete i;
		}

		void draw_layer(instance *i, unsigned int index, outki::maplayer_graphics *graphics, draw_info *di, render::data *renderer)
		{
			outki::tilemap *tiles = graphics->tiles;

			if (i->layer_textures[index] && (!tiles || LIVE_UPDATE(&tiles->texture)))
			{
				render::unload_texture(renderer, i->layer_textures[index]);
				i->layer_textures[index] = 0;
			}

			if (!i->layer_textures[index])
			{
				if (!tiles || !tiles->texture)
				{
					return;
				}

				i->layer_textures[index] = render::load_texture(renderer, tiles->texture);
			}

			render::loaded_texture *tex = i->layer_textures[index];
			if (!tex)
			{
				CLAW_ERROR("Texture load failed on layer " << index);
				return;
			}

			for (int y=0;y<graphics->height;y++)
			{
				for (int x=0;x<graphics->width;x++)
				{
					int tile = graphics->data[y * graphics->width + x];
					if (tile == -1)
					{
						continue;
					}

					outki::tileinfo *info = &tiles->tiles[tile];

					float x0 = x * tiles->tile_width + di->ofsx;
					float x1 = x0 + tiles->tile_width;
					float y0 = y * tiles->tile_height + di->ofsy;
					float y1 = y0 + tiles->tile_height;
					render::tex_rect(renderer, tex, x0, y0, x1, y1, info->u0, info->v0, info->u1, info->v1, 0xffffffff);
				}
			}
		}


		bool line_map_intersection(instance *d, float _x0, float _y0, float _x1, float _y1, intersection *res)
		{
			const float dummy = 99999999;
			res->t = dummy;

			for (unsigned int i=0;i<d->level->collision_lines_size;i++)
			{
				outki::tile_collision_line *line = &d->level->collision_lines[i];

				intersection tmp;
				if (claw::line_intersection(_x0, _y0, _x1, _y1, line->x0, line->y0, line->x1, line->y1, &tmp))
				{
					if (tmp.t < res->t)
					{
						*res = tmp;
					}
				}
			}
			return res->t != dummy;
		}

		void draw(instance *d, draw_info *di, render::data *renderer)
		{
			if (LIVE_UPDATE(&d->level))
				std::cout << "level updated" << std::endl;

			for (unsigned int i=0;i<d->level->layers_size;i++)
			{
				if (LIVE_UPDATE(&d->level->layers[i]))
					std::cout << "layer updated " << std::endl;

				if (LIVEUPDATE_ISNULL(d->level->layers[i]))
				{
					continue;
				}

				if (outki::maplayer_graphics *graphics = d->level->layers[i]->exact_cast<outki::maplayer_graphics>())
				{
					draw_layer(d, i, graphics, di, renderer);
				}
			}

			for (unsigned int i=0;i<d->level->collision_lines_size;i++)
			{
				outki::tile_collision_line *line = &d->level->collision_lines[i];

				const float x0 = line->x0 + di->ofsx;
				const float y0 = line->y0 + di->ofsy;
				const float x1 = line->x1 + di->ofsx;
				const float y1 = line->y1 + di->ofsy;
				render::line(renderer, x0, y0, x1, y1, 0xffff00ff);

				const float cx = 0.5f * (x0+x1);
				const float cy = 0.5f * (y0+y1);

				const float dx = (y1 - y0);
				const float dy = (x0 - x1);
				render::line(renderer, cx, cy, cx + 0.25f * dx, cy + 0.25f * dy, 0xffff00);
			}
		}

	}
}
