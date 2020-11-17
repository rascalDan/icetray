#ifndef ICETRAY_LOGGING_CONSOLE_H
#define ICETRAY_LOGGING_CONSOLE_H

#include "logger.h"

namespace IceTray {
	namespace Logging {
		class ConsoleLogWriter : public AbstractLogWriter {
		public:
			ConsoleLogWriter(const Ice::PropertiesPtr & p);

			void message(LogLevel priority, Domain domain, std::string_view message, const Ice::Current &) override;

			static DLL_PUBLIC std::ostream & writeStream(std::ostream &, int width, LogLevel priority,
					const Domain & domain, const std::string_view & message);

			const int width;
		};
	}
}

#endif
