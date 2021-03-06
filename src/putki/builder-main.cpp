#include <putki/builder/build.h>
#include <putki/builder/builder.h>
#include <putki/builder/package.h>

// generated.
namespace inki
{
	void bind_kosmos();
	void bind_claw();
	void bind_ccg_ui();
}

void ccg_ui_register_handlers(putki::builder::data *builder);
void claw_register_handlers(putki::builder::data *builder);
void kosmos_register_handlers(putki::builder::data *builder);
void kosmos_register_postbuild();

void app_register_handlers(putki::builder::data *builder)
{
	kosmos_register_handlers(builder);
	ccg_ui_register_handlers(builder);
	claw_register_handlers(builder);
}

void app_build_packages(putki::objstore::data *out, putki::build::packaging_config *pconf)
{
	{
		putki::package::data *pkg = putki::build::create_package(pconf);
		putki::package::add(pkg, "globalsettings", true, true);
		putki::build::commit_package(pkg, pconf, "static.pkg");
	}
	{
		putki::package::data *pkg = putki::build::create_package(pconf);
		putki::package::add(pkg, "ui/mainmenu/screen", true, true);
		putki::build::commit_package(pkg, pconf, "mainmenu.pkg");
	}
	{
		putki::package::data *pkg = putki::build::create_package(pconf);
		putki::package::add(pkg, "maps/testmap/testmap", true, true);
		putki::build::commit_package(pkg, pconf, "testmap.pkg");
	}

}

int run_putki_builder(int argc, char **argv);

int main(int argc, char **argv)
{
	inki::bind_claw();
	inki::bind_ccg_ui();
	inki::bind_kosmos();
	
	putki::build::set_builder_configurator(&app_register_handlers);
	putki::build::set_packager(&app_build_packages);
    
	kosmos_register_postbuild();
	
	return run_putki_builder(argc, argv);
}
