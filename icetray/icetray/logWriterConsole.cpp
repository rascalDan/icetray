#include <logWriter.h>
#include "logWriterConsole.h"
#include <compileTimeFormatter.h>
#include <slicer/modelPartsTypes.h>

namespace AdHoc {
	AdHocFormatter(DomainFmt, ".%?");
	StreamWriterT('d') {
		template<typename ... Pn>
		static void write(stream & s, int width, const IceTray::Logging::Domain & domain, const Pn & ... pn)
		{
			auto di = domain.begin();
			if (di == domain.end()) {
				return StreamWriter::next(s, pn...);
			}

			if (width == -1) {
				s << *di++;
				while (di != domain.end()) {
					DomainFmt::write(s, *di++);
				}
			}
			else {
				int target = width;
				while (di != domain.end()) {
					int total = di == domain.begin() ? -1 : 0;
					for (auto dic = di; dic != domain.end(); dic++) {
						total += 1 + dic->length();
					}
					if (di == domain.begin()) {
						if (total > target) {
							s << di->front();
							target -= 1;
						}
						else {
							s << *di;
							target -= di->length();
						}
					}
					else {
						if (total > target) {
							DomainFmt::write(s, di->front());
							target -= 2;
						}
						else {
							DomainFmt::write(s, *di);
							target -= 1 + di->length();
						}
					}
					di++;
				}
			}
			StreamWriter::next(s, pn...);
		}	
	};
}

namespace IceTray {
	namespace Logging {
		AdHocFormatter(LogMsg, "%?: %d: %?\n");
		ConsoleLogWriter::ConsoleLogWriter(const Ice::PropertiesPtr & p) :
			AbstractLogWriter("logging.console", p),
			width(p ? p->getPropertyAsIntWithDefault("logging.console.width", -1) : -1)
		{
		}

		void
		ConsoleLogWriter::message(LogLevel priority, Domain domain, std::string message, const Ice::Current &)
		{
			writeStream(priority < LogLevel::WARNING ? std::cerr : std::cout,
					width, priority, domain, message);
			(priority < LogLevel::WARNING ? std::cerr : std::cout).flush();
		}

		std::ostream &
		ConsoleLogWriter::writeStream(std::ostream & s, int width, LogLevel priority, const Domain & domain, const std::string & message)
		{
			return LogMsg::write(s,
					Slicer::ModelPartForEnum<LogLevel>::lookup(priority), width, domain, message);
		}
		FACTORY(ConsoleLogWriter, LogWriterFactory);
	}
}

