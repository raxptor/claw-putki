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
	}

	outki::globalsettings *get_global_settings()
	{
		return (outki::globalsettings *)putki::pkgmgr::resolve(s_pkg, "globalsettings");
	}

}
