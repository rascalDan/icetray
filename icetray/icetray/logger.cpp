#include "logger.h"
#include "logWriter.h"
#include <Ice/BuiltinSequences.h>
#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/detail/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/format.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/type_index/type_index_facade.hpp>
#include <compileTimeFormatter.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cxxabi.h>
#include <factory.impl.h>
#include <globalStatic.impl.h>
#include <iterator>
#include <lockHelpers.h>
#include <mutex>
#include <slicer/modelPartsTypes.h>
#include <utility>
#include <vector>

INSTANTIATEFACTORY(IceTray::Logging::LogWriter, const Ice::PropertiesPtr &)

template class ::AdHoc::GlobalStatic<::IceTray::Logging::LogManager>;

// NOLINTNEXTLINE(modernize-concat-nested-namespaces)
namespace IceTray {
	namespace Logging {
		LoggerBase::LoggerBase(Domain domain) : domain(std::move(domain)) { }

		LoggerBase::~LoggerBase() = default;

		const Domain &
		LoggerBase::getDomain() const
		{
			return domain;
		}

		Logger::Logger(const Domain & domain) : LoggerBase(domain) { }

		void
		Logger::message(LogLevel priority, const std::string & msg) const
		{
			const auto fl = firstFor(priority);
			if (fl != logs.end()) {
				message(fl, priority, msg);
			}
		}

		void
		Logger::message(LogLevelWriters::const_iterator fl, LogLevel priority, const std::string & msg) const
		{
			SharedLock(_lock);
			while (fl != logs.end()) {
				for (const auto & log : *fl) {
					log->message(priority, domain, msg);
				}
				fl++;
			}
		}

		void
		Logger::messagebf(const LogLevelWriters::const_iterator fl, LogLevel priority, const boost::format & f) const
		{
			message(fl, priority, f.str());
		}

		void
		Logger::messagef(LogLevel priority, const char * msgfmt, ...) const
		{
			const auto fl = firstFor(priority);
			if (fl != logs.end()) {
				va_list v;
				// NOLINTNEXTLINE(hicpp-vararg,hicpp-no-array-decay)
				va_start(v, msgfmt);
				char * msg;
				// NOLINTNEXTLINE(hicpp-no-array-decay)
				int len = vasprintf(&msg, msgfmt, v);
				if (len > 0) {
					message(fl, priority, msg);
				}
				// NOLINTNEXTLINE(hicpp-no-malloc)
				free(msg);
				// NOLINTNEXTLINE(hicpp-no-array-decay)
				va_end(v);
			}
		}

		LogLevelWriters::const_iterator
		Logger::firstFor(LogLevel priority) const
		{
			SharedLock(_lock);
			auto i = logs.begin();
			i += static_cast<int>(priority);
			while (i != logs.end()) {
				if (!i->empty()) {
					return i;
				}
				i++;
			}
			return logs.end();
		}

		LoggerPtr
		LogManager::getLogger(const std::type_info & type)
		{
			std::unique_ptr<char, void (*)(void *)> res {
					abi::__cxa_demangle(type.name(), nullptr, nullptr, nullptr), std::free};
			// NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
			return getLogger(res.get());
		}

		LoggerPtr
		LogManager::getLogger(const std::string & domain)
		{
			auto domainTokens = AbstractLogWriter::splitDomain(domain);
			auto logger = std::make_shared<Logger>(domainTokens);
			logger->logs = getLogsForDomain(domainTokens);
			loggers.insert(logger.get());
			return logger;
		}

		LogLevelWriters
		LogManager::getLogsForDomain(const Domain & domain) const
		{
			SharedLock(_lock);
			LogLevelWriters logs;
			for (const auto & log : logWriters) {
				auto level = log->level(domain);
				if (level) {
					logs[static_cast<size_t>(*level)].insert(log);
				}
			}
			return logs;
		}

		void
		LogManager::updateLoggerWriters() const
		{
			for (auto logger : loggers) {
				Lock(logger->_lock);
				logger->logs = getLogsForDomain(logger->domain);
			}
		}

		void
		LogManager::addWriter(const LogWriterPrxPtr & writer)
		{
			ScopeLock(_lock) {
				logWriters.insert(writer);
			}
			SharedLock(_lock);
			updateLoggerWriters();
		}

		void
		LogManager::removeWriter(const LogWriterPrxPtr & writer)
		{
			ScopeLock(_lock) {
				logWriters.erase(writer);
			}
			SharedLock(_lock);
			updateLoggerWriters();
		}

		LogManager *
		LogManager::getDefault()
		{
			return ::AdHoc::GlobalStatic<::IceTray::Logging::LogManager>::get();
		}

		AbstractLogWriter::AbstractLogWriter(LogLevel level)
		{
			logDomains.insert({{}, level});
		}

		AbstractLogWriter::AbstractLogWriter(const std::string & prefix, const Ice::PropertiesPtr & p)
		{
			if (!p || prefix.empty()) {
				logDomains.insert({{}, LogLevel::WARNING});
				return;
			}
			auto domainsPrefix = prefix + ".domains.";
			auto map = p->getPropertiesForPrefix(domainsPrefix);
			for (const auto & d : map) {
				auto level = Slicer::ModelPartForEnum<LogLevel>::lookup(d.second);
				auto domain = d.first.substr(domainsPrefix.length());
				logDomains.insert({splitDomain(domain), level});
			}
			auto defaultLevel = p->getProperty(prefix + ".default");
			if (!defaultLevel.empty()) {
				auto level = Slicer::ModelPartForEnum<LogLevel>::lookup(defaultLevel);
				logDomains.insert({{}, level});
			}
		}

		// This is "low-level" e.g. DEBUG, as opposed to lowest enumeration value.
		IceUtil::Optional<LogLevel>
		AbstractLogWriter::lowestLevel(const Ice::Current &)
		{
			IceUtil::Optional<LogLevel> lowest;
			for (const auto & d : logDomains) {
				if (!lowest || d.second > *lowest) {
					lowest = d.second;
				}
			}
			return lowest;
		}

		IceUtil::Optional<LogLevel>
		AbstractLogWriter::level(Domain domain, const Ice::Current &)
		{
			if (const auto d = std::find_if(logDomains.rbegin(), logDomains.rend(),
						[&domain](const auto & d) {
							return boost::algorithm::starts_with(domain, d.first);
						});
					d != logDomains.rend()) {
				return d->second;
			}
			return IceUtil::None;
		}

		Ice::StringSeq
		AbstractLogWriter::splitDomain(const std::string & domain)
		{
			if (domain.empty()) {
				return {};
			}

			Ice::StringSeq domainTokens;
			boost::algorithm::split(
					domainTokens, domain, boost::algorithm::is_any_of(".:"), boost::algorithm::token_compress_on);
			return domainTokens;
		}

		AdHocFormatter(DomainFmt, ".%?");
		void
		AbstractLogWriter::writeDomain(
				std::ostream & s, std::optional<size_t> width, const IceTray::Logging::Domain & domain)
		{
			if (auto di = domain.begin(); di != domain.end()) {
				if (!width) {
					s << *di++;
					while (di != domain.end()) {
						DomainFmt::write(s, *di++);
					}
				}
				else {
					auto target = *width;
					while (di != domain.end()) {
						auto total = di == domain.begin() ? std::string::npos : 0;
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
			}
		}
	}
}
