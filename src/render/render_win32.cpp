#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include <claw/log.h>
#include <claw/appwindow.h>

#include <outki/types/ccg-ui/Texture.h>

#include <cassert>
#include <map>
#include <string>
#include <vector>
#include <cassert>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "render.h"

namespace claw
{
	namespace render
	{
#pragma pack(push, 1)
		struct SolidVert
		{
			float x, y, z;
			DWORD color;
		};

		struct TexVert
		{
			float x, y, z;
			DWORD color;
			float u0, v0;
		};
#pragma pack(pop)

		struct data
		{
			IDirect3D9 *dx;
			IDirect3DDevice9 *device;
			D3DPRESENT_PARAMETERS d3dpresent;
			appwindow::data *window;
		};

		struct loaded_texture
		{
			IDirect3DTexture9 *texture;
		};

		data* create(appwindow::data *window)
		{
			data *d = new data();

			CLAW_INFO("Creating DirectX 9 Device");
			d->dx = Direct3DCreate9(D3D_SDK_VERSION);
			d->window = window;


			memset( &d->d3dpresent, 0, sizeof( D3DPRESENT_PARAMETERS ) );
			d->d3dpresent.Windowed = true;
			d->d3dpresent.SwapEffect = D3DSWAPEFFECT_DISCARD;
			d->d3dpresent.BackBufferCount = 1;
			d->d3dpresent.BackBufferFormat = D3DFMT_X8R8G8B8;
			d->d3dpresent.AutoDepthStencilFormat = D3DFMT_D24S8;
			d->d3dpresent.EnableAutoDepthStencil = true;

			if (FAILED(d->dx->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)appwindow::hwnd(window), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d->d3dpresent, &d->device )))
			{
				CLAW_ERROR("Could not create device!");
				return 0;
			}

			return d;
		}

		void destroy(data *d)
		{
			d->device->Release();
			d->dx->Release();
			delete d;
		}

		bool get_size(data *d, int *width, int *height)
		{
			IDirect3DSurface9 *surface;
			if (D3D_OK == d->device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surface))
			{
				D3DSURFACE_DESC desc;
				surface->GetDesc(&desc);
				surface->Release();

				*width = (int)desc.Width;
				*height = (int)desc.Height;
				return true;
			}

			return false;
		}

		void begin(data *d, bool clearcolor, bool cleardepth, unsigned int clear_color)
		{
			DWORD flags = 0;
			if (clearcolor)
			{
				flags |= D3DCLEAR_TARGET;
			}
			if (cleardepth)
			{
				flags |= D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
			}

			int w, h;
			if (get_size(d, &w, &h))
			{
				int x0, y0, x1, y1;
				appwindow::get_client_rect(d->window, &x0, &y0, &x1, &y1);
				if (x1 != w || y1 != h)
				{
					d->d3dpresent.BackBufferWidth = x1;
					d->d3dpresent.BackBufferHeight = y1;
					d->device->Reset(&d->d3dpresent);
				}
			}

			d->device->Clear(0, 0, flags, (DWORD) clear_color, 1.0f, 0x0);

			if (FAILED(d->device->BeginScene()))
			{
				CLAW_ERROR("Could not begin scene");
			}

			D3DXMATRIX mtx;
			D3DXMatrixOrthoLH(&mtx, (float)w, (float)h, 0.1f, 100.0f);

			D3DXMATRIX sc;
			D3DXMatrixScaling(&sc, 1, -1, 1);
			D3DXMATRIX ofs;
			D3DXMatrixTranslation(&ofs, (float)-(w - 0.5f)/2.0f, (int)-(h - 0.5f)/2.0f, 0);

			mtx = ofs * sc * mtx;
			d->device->SetTransform(D3DTS_PROJECTION, &mtx);

		}

		void end(data *d)
		{
			if (FAILED(d->device->EndScene()))
			{
				CLAW_ERROR("Could not end scene");
			}
		}

		void present(data *d)
		{
			if (FAILED(d->device->Present(0, 0, 0, 0)))
			{
				CLAW_ERROR("Could not end scene");
			}
		}

		void solid_rect(data *d, float x0, float y0, float x1, float y1, unsigned int color)
		{
			SolidVert v[4];
			v[0].x = x0;
			v[0].y = y0;
			v[0].z = 1.0f;
			v[0].color = color;

			v[1].x = x1;
			v[1].y = y0;
			v[1].z = 1.0f;
			v[1].color = color;

			v[2].x = x0;
			v[2].y = y1;
			v[2].z = 1.0f;
			v[2].color = color;

			v[3].x = x1;
			v[3].y = y1;
			v[3].z = 1.0f;
			v[3].color = color;

			d->device->SetRenderState(D3DRS_LIGHTING, false);
			d->device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);

			//d->device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			//d->device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			d->device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(SolidVert));
		}

		void tex_rect(data *d, loaded_texture *tex, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, unsigned int color)
		{
			TexVert v[4];
			v[0].x = x0;
			v[0].y = y0;
			v[0].z = 1.0f;
			v[0].u0 = u0;
			v[0].v0 = v0;
			v[0].color = color;

			v[1].x = x1;
			v[1].y = y0;
			v[1].z = 1.0f;
			v[1].u0 = u1;
			v[1].v0 = v0;
			v[1].color = color;

			v[2].x = x0;
			v[2].y = y1;
			v[2].z = 1.0f;
			v[2].u0 = u0;
			v[2].v0 = v1;
			v[2].color = color;

			v[3].x = x1;
			v[3].y = y1;
			v[3].z = 1.0f;
			v[3].u0 = u1;
			v[3].v0 = v1;
			v[3].color = color;

			d->device->SetTexture(0, tex->texture);
			d->device->SetRenderState(D3DRS_LIGHTING, false);
			d->device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
			d->device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(TexVert));
			d->device->SetTexture(0, 0);
		}

		loaded_texture * load_texture(data *d, outki::Texture *texture)
		{
			outki::TextureOutput *output = texture->Output;

			outki::TextureOutputPng *p = output->exact_cast<outki::TextureOutputPng>();
			if (p)
			{
				IDirect3DTexture9 *texture;
				std::string fullpath("out/win32/");
				fullpath.append(p->PngPath);
				if (D3D_OK == D3DXCreateTextureFromFileEx(d->device, fullpath.c_str(), D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0,
				                                          D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture))
				{
					loaded_texture *lt = new loaded_texture();
					lt->texture = texture;
					return lt;
				}
			}
			return 0;
		}

	}
}