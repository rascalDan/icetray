#include <logWriter.h>
#include "logWriterConsole.h"
#include <slicer/modelPartsTypes.h>

// NOLINTNEXTLINE(modernize-concat-nested-namespaces)
namespace IceTray {
	namespace Logging {
		AdHocFormatter(LogMsg, "%?: %D: %?\n");
		ConsoleLogWriter::ConsoleLogWriter(const Ice::PropertiesPtr & p) :
			AbstractLogWriter("logging.console", p),
			width(p ? p->getPropertyAsIntWithDefault("logging.console.width", -1) : -1)
		{
		}

		void
		ConsoleLogWriter::message(LogLevel priority, Domain domain, const std::string_view message, const Ice::Current &)
		{
			writeStream(priority < LogLevel::WARNING ? std::cerr : std::cout,
					width, priority, domain, message);
			(priority < LogLevel::WARNING ? std::cerr : std::cout).flush();
		}

		std::ostream &
		ConsoleLogWriter::writeStream(std::ostream & s, int width, LogLevel priority, const Domain & domain, const std::string_view & message)
		{
			return LogMsg::write(s,
					Slicer::ModelPartForEnum<LogLevel>::lookup(priority), width, domain, message);
		}
		FACTORY(ConsoleLogWriter, LogWriterFactory);
	}
}

