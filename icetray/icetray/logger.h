#ifndef ICETRAY_LOGGING_H
#define ICETRAY_LOGGING_H

#include <Ice/Optional.h>
#include <Ice/Properties.h>
#include <array>
#include <boost/format/format_fwd.hpp>
#include <buffer.h>
#include <c++11Helpers.h>
#include <compileTimeFormatter.h>
#include <cstddef>
#include <factory.h> // IWYU pragma: keep
#include <logWriter.h>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <shared_mutex>
#include <string>
#include <sys/types.h>
#include <typeinfo>
#include <visibility.h>
// IWYU pragma: no_include "factory.impl.h"
// IWYU pragma: no_include <boost/test/unit_test.hpp>
namespace Ice {
	struct Current;
}

namespace IceTray {
	namespace Logging {
		class LoggerBase;

		using LogWriters = std::set<LogWriterPrxPtr>;
		using LogLevelWriters = std::array<LogWriters, 8>;
		using Loggers = std::set<LoggerBase *>;

		class DLL_PUBLIC LoggerBase {
		public:
			explicit LoggerBase(Domain domain);
			virtual ~LoggerBase() = 0;

			SPECIAL_MEMBERS_DELETE(LoggerBase);

			const Domain & getDomain() const;

		protected:
			friend class LogManager;
			mutable std::shared_mutex _lock;
			LogLevelWriters logs;
			const Domain domain;
		};

		class DLL_PUBLIC Logger : public LoggerBase {
		public:
			explicit Logger(const Domain & domain);

			void message(LogLevel priority, const std::string & msg) const;
			void messagef(LogLevel priority, const char * msgfmt, ...) const __attribute__((format(printf, 3, 4)));
			template<typename fmt, typename... Args>
			void
			messagectf(LogLevel priority, const Args &... args) const
			{
				const auto fl = firstFor(priority);
				if (fl != logs.end()) {
					message(fl, priority, fmt::get(args...));
				}
			}
			template<typename... Args>
			void
			messagebf(LogLevel priority, const std::string & msgfmt, const Args &... args) const
			{
				const auto fl = firstFor(priority);
				if (fl != logs.end()) {
					auto fmt = AdHoc::Buffer::getFormat(msgfmt);
					(fmt % ... % args);
					messagebf(fl, priority, fmt);
				}
			}

		private:
			void message(LogLevelWriters::const_iterator fl, LogLevel priority, const std::string & msg) const;
			void messagebf(LogLevelWriters::const_iterator fl, LogLevel priority, const boost::format & f) const;

			LogLevelWriters::const_iterator firstFor(LogLevel priority) const;
		};
		using LoggerPtr = std::shared_ptr<Logger>;

		class DLL_PUBLIC LogManager {
		public:
			template<typename T>
			LoggerPtr
			getLogger()
			{
				return getLogger(typeid(T));
			}
			LoggerPtr getLogger(const std::type_info &);
			LoggerPtr getLogger(const std::string &);
			LogLevelWriters getLogsForDomain(const Domain &) const;
			void addWriter(const LogWriterPrxPtr & writer);
			void removeWriter(const LogWriterPrxPtr & writer);

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
			explicit AbstractLogWriter() = default;
			explicit AbstractLogWriter(LogLevel level);
			explicit AbstractLogWriter(const std::string & prefix, const Ice::PropertiesPtr & p);

			using LogDomains = std::map<Domain, LogLevel>;
			LogDomains logDomains;

		public:
			static Domain splitDomain(const std::string &);
			static void writeDomain(std::ostream &, std::optional<size_t>, const Domain &);
		};

		using LogWriterFactory = AdHoc::Factory<LogWriter, const Ice::PropertiesPtr &>;
	}
}

namespace LOG = ::IceTray::Logging;

#define LOGMANAGER() (::IceTray::Logging::LogManager::getDefault())

namespace AdHoc {
	StreamWriterT('D') {
		template<typename... Pn>
		static void
		write(stream & s, ssize_t width, const IceTray::Logging::Domain & domain, const Pn &... pn)
		{
			IceTray::Logging::AbstractLogWriter::writeDomain(
					s, width >= 0 ? std::make_optional(width) : std::nullopt, domain);
			StreamWriter::next(s, pn...);
		}
	};
}

#endif
