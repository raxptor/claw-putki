#include "appwindow.h"

#include <outki/types/ccg-ui/Texture.h>
#include <OpenGL/gl.h>

#include <claw/log.h>
#include <datacontainer/datacontainer.h>

#include <stdio.h>
#include <map>
#include <string>

namespace claw
{
	namespace render
	{
		struct loaded_texture
		{
			outki::DataContainer *container;
			GLuint handle;
			std::string source;
			int refcount;
		};

		typedef std::map<std::string, loaded_texture*> LoadedTextures;

		struct data
		{
			appwindow::data *window;
			LoadedTextures textures;
		};


		data* create(appwindow::data *window)
		{
			data *d = new data();
			d->window = window;
			return d;
		}

		void unload_texture(data *d, loaded_texture *tex)
		{
			if (tex->refcount-- == 0)
			{
				glDeleteTextures(1, &tex->handle);
				d->textures.erase(d->textures.find(tex->source));
				delete tex;
			}
		}


		loaded_texture * load_texture(data *d, outki::Texture *texture)
		{
			LoadedTextures::iterator i = d->textures.find(texture->id);
			if (i != d->textures.end())
			{
				i->second->refcount++;
				return i->second;
			}

			if (!texture->Output)
			{
				CLAW_ERROR("Trying to load a texture which has no generated output! [" << texture->id << "]");
				return 0;
			}

			datacontainer::loaded_data *loaded = datacontainer::load(texture->Output->Data, true);
			if (!loaded)
			{
				CLAW_INFO("Could not load texture, no streaming yet")
				return 0;
			}

			if (!loaded->size)
			{
				CLAW_INFO("Empty texture?!")
				datacontainer::release(loaded);
				return 0;
			}

			if (texture->Output->rtti_type_ref() == outki::TextureOutputRaw::type_id())
			{
				if (loaded->size != 4 * texture->Width * texture->Height)
				{
					CLAW_ERROR("Texture is " << texture->Width << "x" << texture->Height << " but bytes are " << loaded->size)
				}

				loaded_texture *tex = new loaded_texture();
				tex->refcount = 1;
				tex->source = texture->id;
				tex->container = texture->Output->Data;

				d->textures.insert(std::make_pair<std::string, loaded_texture*>(texture->id, tex));

				glGenTextures(1, &tex->handle);
				glBindTexture(GL_TEXTURE_2D, tex->handle);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->Width, texture->Height,
				             0, GL_RGBA, GL_UNSIGNED_BYTE, loaded->data);

				CLAW_INFO("Bound texture [" << texture->id << "] to handle=" << tex->handle);
				return tex;
			}
			else
			{
				CLAW_ERROR("Unknown texture format encountered" << texture->id);
				return 0;
			}
		}

		void destroy(data *d)
		{
			delete d;
		}

		void begin(data *d, bool clearcolor, bool cleardepth, unsigned int clear_color)
		{
			int x0, y0, x1, y1;
			appwindow::get_client_rect(d->window, &x0, &y0, &x1, &y1);
			glViewport(0, 0, x1, y1);

			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT);

			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();

			glOrtho(0, (float)x1, (float)y1, 0, -1, 1);
		}

		void end(data *d)
		{
			glFlush();
		}

		void present(data *d)
		{

		}

		bool get_size(data *d, int * width, int * height)
		{
			int x0, y0, x1, y1;
			appwindow::get_client_rect(d->window, &x0, &y0, &x1, &y1);
			*width = x1 - x0;
			*height = y1 - y0;
			return true;
		}

		inline void intColor(unsigned int color)
		{
			glColor4ub((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff, (color >> 24)&0xff);
		}

		void gradient_rect(data *d, float x0, float y0, float x1, float y1, unsigned int tl, unsigned int tr, unsigned int bl, unsigned int br)
		{
			glBegin(GL_TRIANGLE_STRIP);
			intColor(tl);
			glVertex2f(x0,y0);
			intColor(tr);
			glVertex2f(x1,y0);
			intColor(bl);
			glVertex2f(x0,y1);
			intColor(br);
			glVertex2f(x1,y1);
			glEnd();
		}

		void solid_rect(data *d, float x0, float y0, float x1, float y1, unsigned int color)
		{
			intColor(color);
			glBegin(GL_TRIANGLE_STRIP);
			glVertex2f(x0,y0);
			glVertex2f(x1,y0);
			glVertex2f(x0,y1);
			glVertex2f(x1,y1);
			glEnd();
		}

		void line(data *d, float x0, float y0, float x1, float y1, unsigned int color)
		{
			glLineWidth(1);
			intColor(color);
			glBegin(GL_LINES);
			glVertex2f(x0,y0);
			glVertex2f(x1,y1);
			glEnd();
		}

		void tex_rect(data *d, loaded_texture *tex, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, unsigned int color)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, tex->handle);

			glColor4ub((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff, (color >> 24)&0xff);
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(u0, v0);
			glVertex2f(x0,y0);
			glTexCoord2f(u1, v0);
			glVertex2f(x1,y0);
			glTexCoord2f(u0, v1);
			glVertex2f(x0,y1);
			glTexCoord2f(u1, v1);
			glVertex2f(x1,y1);
			glEnd();

			glDisable(GL_TEXTURE_2D);
		}

	}
}