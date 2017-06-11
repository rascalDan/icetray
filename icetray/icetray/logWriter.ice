#ifndef ICETRAY_LOGWRITER
#define ICETRAY_LOGWRITER

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

		interface LogWriter {
			optional(0) LogLevel level(string domain);
			void message(LogLevel level, string domain, string message);
		};
	};
};

#endif

