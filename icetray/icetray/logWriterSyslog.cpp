#include "logWriterSyslog.h"
#include <logWriter.h>
#include <syslog.h>

// NOLINTNEXTLINE(modernize-concat-nested-namespaces)
namespace IceTray {
	namespace Logging {
		AdHocFormatter(LogMsg, "%D: %?");
		SyslogLogWriter::SyslogLogWriter(const Ice::PropertiesPtr & p) :
			AbstractLogWriter("logging.syslog", p),
			width(p ? p->getPropertyAsIntWithDefault("logging.syslog.width", -1) : -1)
		{
			if (p) {
				openlog(p->getPropertyWithDefault("logging.syslog.ident", "icetray").c_str(),
						p->getPropertyAsIntWithDefault("logging.syslog.option", 0),
						p->getPropertyAsIntWithDefault("logging.syslog.facility", LOG_DAEMON));
			}
		}

		SyslogLogWriter::~SyslogLogWriter()
		{
			closelog();
		}

		void
		SyslogLogWriter::message(LogLevel priority, Domain domain, const std::string_view message, const Ice::Current &)
		{
			syslog((int)priority, "%s", LogMsg::get(width, domain, message).c_str());
		}

		FACTORY(SyslogLogWriter, LogWriterFactory);
	}
}
