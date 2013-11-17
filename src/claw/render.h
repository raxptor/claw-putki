#ifndef __CLAW_RENDER_H___
#define __CLAW_RENDER_H___

#include <claw/appwindow.h>

namespace outki
{
	struct Texture;
}

namespace claw
{
	namespace render
	{
		struct data;
		struct loaded_texture;

		loaded_texture * load_texture(data *d, outki::Texture *texture);

		data* create(appwindow::data *window);
		void destroy(data *);
		
		void begin(data *d, bool clearcolor, bool cleardepth, unsigned int clear_color);
		void end(data *d);
		void present(data *d);

		bool get_size(data *d, int * width, int * height);
		void solid_rect(data *d, float x0, float y0, float x1, float y1, unsigned int color);
		void gradient_rect(data *d, float x0, float y0, float x1, float y1, unsigned int tl, unsigned int tr, unsigned int bl, unsigned int br);
		void tex_rect(data *d, loaded_texture *tex, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, unsigned int color);		
		void line(data *d, float x0, float y0, float x1, float y1, unsigned int color);
	}
}

#endif