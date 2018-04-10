#ifndef ICETRAY_LOGGING_H
#define ICETRAY_LOGGING_H

#include <factory.h>
#include <visibility.h>
#include <logWriter.h>
#include <boost/format/format_fwd.hpp>
#include <map>
#include <buffer.h>
#include <shared_mutex>
#include <Ice/Properties.h>
#include <globalStatic.h>

namespace IceTray {
	namespace Logging {
		class LogManager;
		class LoggerBase;

		typedef std::set<LogWriterPrxPtr> LogWriters;
		typedef std::array<LogWriters, 8> LogLevelWriters;
		typedef std::set<LoggerBase *> Loggers;

		class DLL_PUBLIC LoggerBase {
			public:
				LoggerBase(const Domain & domain);
				~LoggerBase();

				const Domain & getDomain() const;

			protected:
				friend class LogManager;
				mutable std::shared_mutex _lock;
				LogLevelWriters logs;
				const Domain domain;
		};

		class DLL_PUBLIC Logger : public LoggerBase {
			public:
				Logger(const Domain & domain);

				void message(LogLevel priority, const std::string & msg) const;
				void messagef(LogLevel priority, const char * msgfmt, ...) const __attribute__ ((format (printf, 3, 4)));
				template <typename fmt, typename... Args>
				void messagectf(LogLevel priority, const Args & ... args) const {
					const auto fl = firstFor(priority);
					if (fl != logs.end()) {
						message(fl, priority, fmt::get(args...));
					}
				}
				template <typename... Args>
				void messagebf(LogLevel priority, const std::string & msgfmt, const Args & ... args) const {
					const auto fl = firstFor(priority);
					if (fl != logs.end()) {
						auto fmt = AdHoc::Buffer::getFormat(msgfmt);
						messagebf(fl, priority, fmt, args...);
					}
				}

			private:
				void message(LogLevelWriters::const_iterator fl, LogLevel priority, const std::string & msg) const;
				void vmessagef(LogLevelWriters::const_iterator fl, LogLevel priority, const char * msgfmt, va_list) const;
				template <typename Arg, typename... OtherArgs>
				void messagebf(LogLevelWriters::const_iterator fl, LogLevel priority, boost::format & f, const Arg & arg, const OtherArgs & ... otherargs) const {
					f % arg;
					messagebf(fl, priority, f, otherargs...);
				}
				void messagebf(LogLevelWriters::const_iterator fl, LogLevel priority, const boost::format & f) const;

				LogLevelWriters::const_iterator firstFor(LogLevel priority) const;
		};
		typedef std::shared_ptr<Logger> LoggerPtr;

		class DLL_PUBLIC LogManager {
			public:
				template<typename T>
				LoggerPtr getLogger() {
					return getLogger(typeid(T));
				}
				LoggerPtr getLogger(const std::type_info &);
				LoggerPtr getLogger(const std::string &);
				LogLevelWriters getLogsForDomain(const Domain &) const;
				void addWriter(LogWriterPrxPtr writer);
				void removeWriter(LogWriterPrxPtr writer);

				static LogManager * getDefault();

			private:
				void updateLoggerWriters() const;
				mutable std::shared_mutex _lock;
				Loggers loggers;
				LogWriters logWriters;
		};

		class DLL_PUBLIC AbstractLogWriter : public LogWriter {
			public:
				IceUtil::Optional<LogLevel> lowestLevel(const Ice::Current &) override;
				IceUtil::Optional<LogLevel> level(Domain, const Ice::Current &) override;

			protected:
				AbstractLogWriter();
				AbstractLogWriter(LogLevel level);
				AbstractLogWriter(const std::string & prefix, Ice::PropertiesPtr p);

				typedef std::map<Domain, LogLevel> LogDomains;
				LogDomains logDomains;

			public:
				static Domain splitDomain(const std::string &);
		};

		typedef AdHoc::Factory<LogWriter, const Ice::PropertiesPtr &> LogWriterFactory;
	}
}

namespace LOG = ::IceTray::Logging;

#define LOGMANAGER() (::IceTray::Logging::LogManager::getDefault())

#endif

