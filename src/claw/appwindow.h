namespace claw
{
	namespace appwindow
	{
		struct data;
		
		typedef void (*updatefunc)(void);

		data* create(const char *title, int width, int height);

		void destroy(data *);
		void set_title(data *, const char *title);

		void get_client_rect(data *d, int *x0, int *y0, int *x1, int *y1);
		
		void run_loop(data *d, updatefunc f);

#if defined(WIN32)
		void* hwnd(data *);
#endif
	}
}
