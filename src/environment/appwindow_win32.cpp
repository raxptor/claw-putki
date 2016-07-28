#include "appwindow.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <kosmos/glwrap/gl.h>

namespace claw
{
	namespace appwindow
	{
		struct data
		{
			HWND window;
		};

		LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
		{
			switch (msg)
			{
				case WM_DESTROY:
				{
					PostQuitMessage(0);
					return 0;
				}
				case WM_SIZE:
				{
					RECT r;
					GetClientRect(hWnd, &r);
					return 0;
				}
				case WM_CREATE:
				{
					PIXELFORMATDESCRIPTOR pfd =
					{
						sizeof(PIXELFORMATDESCRIPTOR),
						1,
						PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
						PFD_TYPE_RGBA,
						32,
						0, 0, 0, 0, 0, 0,
						0,
						0,
						0,
						0, 0, 0, 0,
						24,
						8,
						0,
						PFD_MAIN_PLANE,
						0,
						0, 0, 0
					};

					HDC hdc = GetDC(hWnd);
					int  cpf = ChoosePixelFormat(hdc, &pfd);
					SetPixelFormat(hdc, cpf, &pfd);

					HGLRC rc = wglCreateContext(hdc);
					wglMakeCurrent(hdc, rc);

					glewInit();

					// MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
				}
				break;
			}

			return DefWindowProc( hWnd, msg, wParam, lParam );
		}

		void* hwnd(data *d)
		{
			return d->window;
		}

		data* create(const char *title, int width, int height, const char *icon)
		{
			data *d = new data;

			WNDCLASSEX wc =
			{
				sizeof( WNDCLASSEX ),    CS_CLASSDC, MsgProc, 0L,   0L,
				GetModuleHandle( NULL ), NULL,       NULL,    NULL, NULL,
				"CLAW-NG",               NULL
			};

			wc.hIcon = 0; // LoadIcon(wc.hInstance, MAKEINTRESOURCE(desc.icon));
			wc.hCursor = LoadCursor( NULL, IDC_ARROW );

			::RegisterClassEx( &wc );

			RECT rc = {
				0,
				0,
				width,
				height
			};

			AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

			// Create the application's window
			d->window = ::CreateWindowA("CLAW-NG", title,
			                            WS_OVERLAPPEDWINDOW, 100, 100, rc.right - rc.left, rc.bottom - rc.top,
			                            NULL, NULL, wc.hInstance, NULL );

			ShowWindow(d->window, true);
			
			return d;
		}

		void set_title(data *d, const char *title)
		{
			SetWindowTextA(d->window, title);
		}

		void get_client_rect(data *d, int *x0, int *y0, int *x1, int *y1)
		{
			RECT r;
			GetClientRect(d->window, &r);
			*x0 = 0;
			*y0 = 0;
			*x1 = r.right;
			*y1 = r.bottom;
		}

		void run_loop(data *d, updatefunc f)
		{
			HDC dc = GetDC(d->window);
			do
			{
				MSG m;
				while (PeekMessage(&m, 0, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&m);
					DispatchMessage(&m);

					if (m.message == WM_QUIT)
					{
						return;
					}
				}

				struct input_batch ib;
				ib.mouse.primary.isDown = 0;
				ib.mouse.primary.wentDown = 0;
				ib.mouse.primary.wentUp = 0;
				ib.mouse.x = 0;
				ib.mouse.y = 0;
				f(&ib, 0.01f);

				SwapBuffers(dc);
				
			} while (true);

		}

		void destroy(data *d)
		{
			delete d;
		}
	}
}
