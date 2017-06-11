#include "logger.h"
#include <factory.impl.h>
#include <buffer.h>
#include <lockHelpers.h>

INSTANTIATEFACTORY(IceTray::Logging::LogWriter, Ice::CommunicatorPtr);

namespace IceTray {
	namespace Logging {
		LoggerBase::LoggerBase(LogManager * manager, const std::string & domain) :
			logs(manager->getLogsForDomain(domain)),
			domain(domain)
		{
		}

		LoggerBase::~LoggerBase()
		{
		}

		Logger::Logger(LogManager * manager, const std::string & domain) : LoggerBase(manager, domain) { }

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
		LogManager::getLogger(const std::string & domain)
		{
			auto logger = LoggerPtr(new Logger(this, domain));
			loggers.insert(logger.get());
			return logger;
		}

		LogLevelWriters
		LogManager::getLogsForDomain(const std::string & domain) const
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
		LogManager::addWriter(LogWriterPrx writer)
		{
			UpgradableLock(_lock, l);
			UpgradeScopeLock(l) {
				logWriters.insert(writer);
			}
			for (auto logger : loggers) {
				Lock(logger->_lock);
				logger->logs = getLogsForDomain(logger->domain);
			}
		}

		void
		LogManager::removeWriter(LogWriterPrx writer)
		{
			UpgradableLock(_lock, l);
			UpgradeScopeLock(l) {
				logWriters.erase(writer);
			}
			for (auto logger : loggers) {
				Lock(logger->_lock);
				logger->logs = getLogsForDomain(logger->domain);
			}
		}
	}
}

