#ifndef ICETRAY_LOGGING_CONSOLE_H
#define ICETRAY_LOGGING_CONSOLE_H

#include "logWriter.h"
#include "logger.h"
#include <Ice/Properties.h>
#include <iosfwd>
#include <string_view>
#include <visibility.h>
namespace Ice {
	struct Current;
}

namespace IceTray {
	namespace Logging {
		class ConsoleLogWriter : public AbstractLogWriter {
		public:
			explicit ConsoleLogWriter(const Ice::PropertiesPtr & p);

			void message(LogLevel priority, Domain domain, std::string_view message, const Ice::Current &) override;

			static DLL_PUBLIC std::ostream & writeStream(std::ostream &, int width, LogLevel priority,
					const Domain & domain, const std::string_view & message);

			const int width;
		};
	}
}

#endif
