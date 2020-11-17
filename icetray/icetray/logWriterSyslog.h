#ifndef ICETRAY_LOGGING_SYSLOG_H
#define ICETRAY_LOGGING_SYSLOG_H

#include "logger.h"

namespace IceTray {
	namespace Logging {
		class SyslogLogWriter : public AbstractLogWriter {
		public:
			SyslogLogWriter(const Ice::PropertiesPtr & p);
			~SyslogLogWriter();

			void message(LogLevel priority, Domain domain, std::string_view message, const Ice::Current &) override;

			const int width;
		};
	}
}

#endif
