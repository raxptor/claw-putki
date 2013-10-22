
#include <ccg-ui/ccg-renderer.h>
#include <claw/render.h>

#include <string>
#include <hash_map>

namespace claw
{
	class claw_ui_renderer : public ccgui::render_api
	{
		public:

			typedef std::hash_map< outki::Texture* , render::loaded_texture* > ResolvedTexturesMap; 

			claw_ui_renderer(render::data *renderer) : m_renderer(renderer)
			{

			}

			~claw_ui_renderer()
			{
			
			}

			void tex_rect(outki::Texture *texture, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, unsigned int color)
			{
				again:
				ResolvedTexturesMap::iterator i = m_loaded.find(texture);
				if (i == m_loaded.end())
				{
					m_loaded[texture] = render::load_texture(m_renderer, texture);
					goto again;
				}

				if (!i->second)
					return;

				claw::render::tex_rect(m_renderer, i->second, x0, y0, x1, y1, u0, v0, u1, v1, color);
			}
				

		private:

			ResolvedTexturesMap m_loaded;
			render::data *m_renderer;

	};
}
