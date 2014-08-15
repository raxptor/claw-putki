#include <putki/data-dll/dllinterface.h>
#include <putki/builder/builder.h>

namespace inki
{
	void bind_ccg_ui();
	void bind_ccg_ui_dll();
	void bind_claw();
	void bind_claw_dll();
}

void ccg_ui_register_handlers(putki::builder::data *builder);
void claw_register_handlers(putki::builder::data *builder);

void setup_builder(putki::builder::data *builder)
{
	ccg_ui_register_handlers(builder);
	claw_register_handlers(builder);
}

extern "C"
{
	#if defined(_MSC_VER)
	__declspec(dllexport) putki::data_dll_i* __cdecl load_data_dll(const char *data_path)
	#else
	putki::data_dll_i* load_data_dll(const char *data_path)
	#endif
	{
		inki::bind_ccg_ui();
		inki::bind_ccg_ui_dll();
		inki::bind_claw();
		inki::bind_claw_dll();

		putki::builder::set_builder_configurator(&setup_builder);

		// bind at startup.
		return putki::create_dll_interface(data_path);
	}

}