#include <logWriter.h>
#include "logger.h"
#include <compileTimeFormatter.h>
#include <slicer/modelPartsTypes.h>

namespace IceTray {
	namespace Logging {
		AdHocFormatter(LogMsg, "%?: %?: %?\n");
		class ConsoleLogWriter : public AbstractLogWriter {
			public:
				ConsoleLogWriter(Ice::Properties * p) :
					AbstractLogWriter("logging.console", p)
				{
				}

				void message(LogLevel priority, const std::string & domain, const std::string & message, const Ice::Current &) override
				{
					LogMsg::write(priority < WARNING ? std::cerr : std::cout,
							Slicer::ModelPartForEnum<LogLevel>::lookup(priority), domain, message);
				}
		};
		FACTORY(ConsoleLogWriter, LogWriterFactory);
	}
}

