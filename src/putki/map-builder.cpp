#include <putki/builder/app.h>
#include <putki/builder/build.h>
#include <putki/builder/builder.h>
#include <putki/builder/package.h>
#include <putki/builder/resource.h>
#include <putki/builder/build-db.h>
#include <putki/builder/db.h>
#include <builder/pngutil.h>

#include <inki/types/claw/tiles.h>
#include <iostream>

struct map_layer_builder : putki::builder::handler_i
{
	virtual bool handle(putki::builder::data *builder, putki::build_db::record *record, putki::db::data *input, const char *path, putki::instance_t obj, putki::db::data *output, int obj_phase)
	{
		inki::maplayer *maplayer = (inki::maplayer *) obj;
		if (!maplayer)
			return false;

		std::cout << "Building maplayer [" << path << "]" << std::endl;
		return false;
	}
};

struct mapbuilder : putki::builder::handler_i
{
	virtual bool handle(putki::builder::data *builder, putki::build_db::record *record, putki::db::data *input, const char *path, putki::instance_t obj, putki::db::data *output, int obj_phase)
	{
		inki::map *map = (inki::map *) obj;
		if (!map)
			return false;

		std::cout << "Building map [" << path << "]" << std::endl;

		for (unsigned int i=0;i<map->layers.size();i++)
		{
			if (!map->layers[i])
				continue;

			if (map->layers[i]->rtti_type_ref() == inki::maplayer_graphics::type_id())
			{
				inki::maplayer_graphics *gr = (inki::maplayer_graphics*) map->layers[i];

				if (!gr->tiles || !gr->tiles->texture)
					continue;

				ccgui::pngutil::loaded_png png;
				if (!ccgui::pngutil::load_info(putki::resource::real_path(builder, gr->tiles->texture->Source.c_str()).c_str(), &png))
					continue;

				char tmp[1024];
				sprintf(tmp, "%s/layer_%d", path, i);

				const int tiles_x = png.width / gr->tiles->tile_width;
				const int tiles_y = png.height / gr->tiles->tile_height;
				
				// create output layer 
				inki::layer_output *lo = inki::layer_output::alloc();
				putki::db::insert(output, tmp, lo->th(), lo);
				putki::build_db::add_output(record, tmp);

				for (int y=0;y<gr->height;y++)
				{
					for (int x=0;x<gr->width;x++)
					{
						const unsigned int index = y * gr->width + x;						
						int tile = index < gr->data.size() ? gr->data[i] : -1;
						if (tile != -1)
						{
							int tileu = tile % tiles_x;
							int tilev = tile / tiles_y;

							inki::map_vertex vUL;
							vUL.x = float(gr->tiles->tile_width * x);
							vUL.y = float(gr->tiles->tile_height * y);
							vUL.u = float(tileu * gr->tiles->tile_width) / float(png.width);
							vUL.v = float(tilev * gr->tiles->tile_height) / float(png.height);

							inki::map_vertex vBR;
							vBR.x = float(gr->tiles->tile_width * (x + 1));
							vBR.y = float(gr->tiles->tile_height * (y + 1));
							vBR.u = float((tileu + 1) * gr->tiles->tile_width) / float(png.width);
							vBR.v = float((tilev + 1) * gr->tiles->tile_height) / float(png.height);

							inki::map_vertex vBL = vUL;
							vBL.y = vBR.y;
							vBL.v = vBR.v;

							lo->trilist.push_back(vUL);
							lo->trilist.push_back(vBR);
							lo->trilist.push_back(vBL);
						}
					}
				}
			}
		}


		return false;
	}
};

void register_map_builder(putki::builder::data *builder)
{
	static mapbuilder mb;
	static map_layer_builder mlb;
	putki::builder::add_data_builder(builder, "map", putki::builder::PHASE_INDIVIDUAL, &mb);
	putki::builder::add_data_builder(builder, "maplayer", putki::builder::PHASE_INDIVIDUAL, &mlb);
}
