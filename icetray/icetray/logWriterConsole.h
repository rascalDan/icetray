#ifndef ICETRAY_LOGGING_CONSOLE_H
#define ICETRAY_LOGGING_CONSOLE_H

#include "logger.h"

namespace IceTray {
	namespace Logging {
		class ConsoleLogWriter : public AbstractLogWriter {
			public:
				ConsoleLogWriter(Ice::Properties * p);

				void message(LogLevel priority, const Domain & domain, const std::string & message, const Ice::Current &) override;

				static DLL_PUBLIC std::ostream & writeStream(std::ostream &, int width, LogLevel priority, const Domain & domain, const std::string & message);

				const int width;
		};
	}
}

#endif

