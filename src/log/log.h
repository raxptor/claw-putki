#ifndef __CLAW_LOG_H__
#define __CLAW_LOG_H__

namespace claw
{
	void log(const char *cstr);    // needed by macros.
	void error(const char *cstr);

	void syslog(const char *cstr); // output to console
}

#include <sstream>

#if _DEBUG || true

// Logging enabled.
	#define CLAW_INFO(stmt) {                          \
		std::ostringstream __DPRINT_LINE;                  \
		__DPRINT_LINE << __FILE__ << " (" << __LINE__ << "): " << stmt; \
		claw::log(__DPRINT_LINE.str().c_str()); \
}

// Logging enabled.
	#define CLAW_ERROR(stmt) {                         \
		std::ostringstream __DPRINT_LINE;                  \
		__DPRINT_LINE << __FILE__ << " (" << __LINE__ << "): " << stmt; \
		claw::error(__DPRINT_LINE.str().c_str()); \
}

#else

	#define CLAW_INFO(stmt) { }

// Logging enabled.
	#define CLAW_ERROR(stmt) {                         \
		std::ostringstream __DPRINT_LINE;                  \
		__DPRINT_LINE << __FILE__ << " (" << __LINE__ << "): " << stmt; \
		claw::error(__DPRINT_LINE.str().c_str()); \
}

#endif

#define CLAW_WARNING(x) CLAW_INFO("warn: " << x)
#define CLAW_DEBUG(x) CLAW_INFO("debug:" << x)

#endif
