#include "staticdata.h"

#include <putki/pkgmgr.h>
#include <putki/pkgloader.h>

#include <kosmos/log/log.h>

namespace game
{
	namespace
	{
		putki::pkgmgr::loaded_package *s_pkg = 0;
		const char *s_static_package_name = "static.pkg";
	}

	void load_static_package()
	{
		s_pkg = putki::pkgloader::from_file(s_static_package_name);
        if (!s_pkg)
        {
            KOSMOS_ERROR("Failed to load required package[" << s_static_package_name << "]");
        }
		putki::pkgmgr::register_for_liveupdate(s_pkg);
	}

	outki::global_settings *get_global_settings()
	{
		return (outki::global_settings *)putki::pkgmgr::resolve(s_pkg, "globalsettings");
	}

}
