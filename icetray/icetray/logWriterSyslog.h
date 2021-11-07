#ifndef ICETRAY_LOGGING_SYSLOG_H
#define ICETRAY_LOGGING_SYSLOG_H

#include "logWriter.h"
#include "logger.h"
#include <Ice/Properties.h>
#include <c++11Helpers.h>
#include <string_view>
namespace Ice {
	struct Current;
}

namespace IceTray {
	namespace Logging {
		class SyslogLogWriter : public AbstractLogWriter {
		public:
			explicit SyslogLogWriter(const Ice::PropertiesPtr & p);
			~SyslogLogWriter() override;

			SPECIAL_MEMBERS_DELETE(SyslogLogWriter);

			void message(LogLevel priority, Domain domain, std::string_view message, const Ice::Current &) override;

			const int width;
		};
	}
}

#endif
