#ifndef ICETRAY_LOGWRITER
#define ICETRAY_LOGWRITER

[["ice-prefix"]]
[["cpp:include:string_view_support.h"]]
module IceTray {
	module Logging {
		enum LogLevel {
			/* These match those defined in syslog.h */
			EMERG	= 0,
			ALERT = 1,
			CRIT = 2,
			ERR =	3,
			WARNING = 4,
			NOTICE = 5,
			INFO = 6,
			DEBUG = 7
		};
		["slicer:ignore"]
		sequence<string> Domain;

		interface LogWriter {
			optional(0) LogLevel lowestLevel();
			optional(0) LogLevel level(Domain domain);
			void message(LogLevel level, Domain domain, ["cpp:view-type:std::string_view"] string message);
		};
	};
};

#endif

