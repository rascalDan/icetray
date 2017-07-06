#include "logger.h"
#include <factory.impl.h>
#include <buffer.h>
#include <lockHelpers.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <slicer/modelPartsTypes.h>
#include <globalStatic.impl.h>

INSTANTIATEFACTORY(IceTray::Logging::LogWriter, Ice::Properties *);

template class ::AdHoc::GlobalStatic<::IceTray::Logging::LogManager>;

namespace IceTray {
	namespace Logging {
		LoggerBase::LoggerBase(const Domain & domain) :
			domain(domain)
		{
		}

		LoggerBase::~LoggerBase()
		{
		}

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
				for (auto log : *fl) {
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
				va_start(v, msgfmt);
				vmessagef(fl, priority, msgfmt, v);
				va_end(v);
			}
		}

		void
		Logger::vmessagef(LogLevelWriters::const_iterator fl, LogLevel priority, const char * msgfmt, va_list va) const
		{
			char * msg;
			int len = vasprintf(&msg, msgfmt, va);
			if (len > 0) {
				message(fl, priority, msg);
			}
			free(msg);
		}

		LogLevelWriters::const_iterator
		Logger::firstFor(LogLevel priority) const
		{
			SharedLock(_lock);
			auto i = logs.begin();
			i += priority;
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
			std::unique_ptr<char, void(*)(void*)> res {
				abi::__cxa_demangle(type.name(), NULL, NULL, NULL), std::free
			};
			return getLogger(res.get());
		}

		LoggerPtr
		LogManager::getLogger(const std::string & domain)
		{
			auto domainTokens = AbstractLogWriter::splitDomain(domain);
			auto logger = LoggerPtr(new Logger(domainTokens));
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
					logs[*level].insert(log);
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
		LogManager::addWriter(LogWriterPrx writer)
		{
			UpgradableLock(_lock, l);
			UpgradeScopeLock(l) {
				logWriters.insert(writer);
			}
			updateLoggerWriters();
		}

		void
		LogManager::removeWriter(LogWriterPrx writer)
		{
			UpgradableLock(_lock, l);
			UpgradeScopeLock(l) {
				logWriters.erase(writer);
			}
			updateLoggerWriters();
		}

		LogManager *
		LogManager::getDefault()
		{
			return ::AdHoc::GlobalStatic<::IceTray::Logging::LogManager>::get();
		}

		AbstractLogWriter::AbstractLogWriter()
		{
		}

		AbstractLogWriter::AbstractLogWriter(LogLevel level)
		{
			logDomains.insert({ { }, level });
		}

		AbstractLogWriter::AbstractLogWriter(const std::string & prefix, Ice::PropertiesPtr p)
		{
			if (!p || prefix.empty()) {
				logDomains.insert({ { }, WARNING });
				return;
			}
			auto domainsPrefix = prefix + ".domains.";
			auto map = p->getPropertiesForPrefix(domainsPrefix);
			for (const auto & d : map) {
				auto level = Slicer::ModelPartForEnum<LogLevel>::lookup(d.second);
				auto domain = d.first.substr(domainsPrefix.length());
				logDomains.insert({ splitDomain(domain), level });
			}
			auto defaultLevel = p->getProperty(prefix + ".default");
			if (!defaultLevel.empty()) {
				auto level = Slicer::ModelPartForEnum<LogLevel>::lookup(defaultLevel);
				logDomains.insert({ {}, level });
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
		AbstractLogWriter::level(const Domain & domain, const Ice::Current &)
		{
			for (auto d = logDomains.rbegin(); d != logDomains.rend(); d++) {
				if (boost::algorithm::starts_with(domain, d->first)) {
					return d->second;
				}
			}
			return IceUtil::None;
		}

		Ice::StringSeq
		AbstractLogWriter::splitDomain(const std::string & domain)
		{
			if (domain.empty()) return Ice::StringSeq();

			Ice::StringSeq domainTokens;
			boost::algorithm::split(domainTokens, domain, boost::algorithm::is_any_of(".:"), boost::algorithm::token_compress_on);
			return domainTokens;
		}
	}
}

