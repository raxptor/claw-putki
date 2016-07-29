#include <putki/builder/build.h>
#include <putki/builder/builder.h>
#include <putki/builder/package.h>
#include <putki/builder/build-db.h>
#include <putki/builder/log.h>
#include <kosmos-builder-utils/pngutil.h>
#include <iostream>

#include <inki/types/claw/tiles.h>

namespace
{ 
	bool build_tilemap(const putki::builder::build_info* info)
	{
		inki::tilemap *tilemap = (inki::tilemap *) info->object;
		if (!tilemap->texture.get())
		{
			RECORD_WARNING(info->record, "Tilemap has no texture set")
			return false;
		}

		kosmos::pngutil::loaded_png png;
		if (kosmos::pngutil::load_from_resource(info, tilemap->texture->source.c_str(), &png))
		{
			int tilesx = png.width / tilemap->tile_width;
			int tilesy = png.height / tilemap->tile_height;
			RECORD_INFO(info->record, "Tilemap is " << png.width << "x" << png.height << " and has " << tilesx << "x" << tilesy << " tiles ")
			for (int y=0;y<tilesy;y++)
			{
				for (int x=0;x<tilesx;x++)
				{
					inki::tileinfo ti;
					ti.u0 = float((x+0) * tilemap->tile_width) / float(png.width);
					ti.u1 = float((x+1) * tilemap->tile_width) / float(png.width);
					ti.v0 = float((y+0) * tilemap->tile_height) / float(png.height);
					ti.v1 = float((y+1) * tilemap->tile_height) / float(png.height);
					tilemap->tiles.push_back(ti);
				}
			}
			kosmos::pngutil::free(&png);
		}
		else
		{
			RECORD_WARNING(info->record, "Failed to load png!")
		}
		return true;
	}
};

void register_tilemap_builder(putki::builder::data *builder)
{
	putki::builder::handler_info info[1] = {
		{ inki::tilemap::type_id(), "tilemap-builder-1", build_tilemap, 0 }
	};
	putki::builder::add_handlers(builder, &info[0], &info[1]);
}
