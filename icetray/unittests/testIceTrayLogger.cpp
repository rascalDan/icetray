#define BOOST_TEST_MODULE TestIceTrayLogger
#include <boost/test/unit_test.hpp>

#include "testService.h"
#include <Ice/Communicator.h>
#include <Ice/Initialize.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/Optional.h>
#include <Ice/Properties.h>
#include <Ice/PropertiesF.h>
#include <Ice/Proxy.h>
#include <boost/format.hpp>
#include <compileTimeFormatter.h>
#include <factory.h>
#include <logWriter.h>
#include <logWriterConsole.h>
#include <logger.h>
#include <memory>
#include <slicer/common.h> // IWYU pragma: keep
#include <slicer/modelPartsTypes.h>
#include <string>
#include <string_view>
#include <vector>
namespace Ice {
	struct Current;
}
namespace IceTray {
	class Service;
}

using namespace IceTray::Logging;

struct LogEntry {
	LogLevel priority;
	Domain domain;
	std::string message;
};

class TestLogWriter : public AbstractLogWriter {
public:
	template<typename... T> explicit TestLogWriter(const T &... t) : AbstractLogWriter(t...) { }

	explicit TestLogWriter(const Ice::PropertiesPtr & p) : AbstractLogWriter("TestLogWriter", p) { }

	void
	message(LogLevel priority, Domain domain, const std::string_view message, const Ice::Current &) override
	{
		msgs.push_back({priority, domain, std::string(message)});
	}

	std::vector<LogEntry> msgs;
};
FACTORY(TestLogWriter, LogWriterFactory)

namespace std {
	ostream &
	operator<<(ostream & s, const Domain & domain)
	{
		for (const auto & d : domain) {
			s << "::" << d;
		}
		return s;
	}
	ostream &
	operator<<(ostream & s, const LogLevel & ll)
	{
		s << Slicer::ModelPartForEnum<LogLevel>::lookup(ll);
		return s;
	}
}

class StaticLogTest {
public:
	static IceTray::Logging::LoggerPtr staticLog;
};
IceTray::Logging::LoggerPtr staticLog = LOGMANAGER()->getLogger<IceTray::Service>();

Domain other = {"other"};
Domain test = {"test"};
Domain testDomain = {"test", "domain"};
Domain testDebug = {"test", "debug"};

BOOST_AUTO_TEST_CASE(staticLogInit)
{
	BOOST_REQUIRE(staticLog);
	Domain expected = {"IceTray", "Service"};
	BOOST_REQUIRE_EQUAL(expected, staticLog->getDomain());
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class TestLogImpl {
public:
	TestLogImpl() : ic(Ice::initialize()), adp(ic->createObjectAdapterWithEndpoints("test", "default"))
	{
		adp->activate();
		log = manager.getLogger("test.domain");
	}

	~TestLogImpl()
	{
		adp->deactivate();
		adp->destroy();
		ic->destroy();
	}

	LogWriterPrxPtr
	add(const LogWriterPtr & w)
	{
		return Ice::uncheckedCast<LogWriterPrx>(adp->addWithUUID(w));
	}

protected:
	LoggerPtr log;
	LogManager manager;
	Ice::CommunicatorPtr ic;

private:
	Ice::ObjectAdapterPtr adp;
};

BOOST_FIXTURE_TEST_SUITE(li, TestLogImpl)

BOOST_AUTO_TEST_CASE(no_writers)
{
	log->message(LogLevel::EMERG, "");
	log->message(LogLevel::DEBUG, "");
}

BOOST_AUTO_TEST_CASE(ostreamDomain)
{
	std::stringstream str;
	str << testDomain;
	BOOST_REQUIRE_EQUAL("::test::domain", str.str());
}

BOOST_AUTO_TEST_CASE(priority_filtering)
{
	auto w = std::make_shared<TestLogWriter>(LogLevel::WARNING);
	auto e = std::make_shared<TestLogWriter>(LogLevel::ERR);
	auto wp = add(w);
	auto ep = add(e);
	manager.addWriter(wp);
	manager.addWriter(ep);
	BOOST_REQUIRE(w->msgs.empty());
	BOOST_REQUIRE(e->msgs.empty());

	log->message(LogLevel::DEBUG, "debug");
	BOOST_REQUIRE(w->msgs.empty());
	BOOST_REQUIRE(e->msgs.empty());

	log->message(LogLevel::INFO, "info");
	BOOST_REQUIRE(w->msgs.empty());
	BOOST_REQUIRE(e->msgs.empty());

	log->message(LogLevel::NOTICE, "notice");
	BOOST_REQUIRE(w->msgs.empty());
	BOOST_REQUIRE(e->msgs.empty());

	log->message(LogLevel::WARNING, "warning");
	BOOST_REQUIRE_EQUAL(1, w->msgs.size());
	BOOST_REQUIRE(e->msgs.empty());

	log->message(LogLevel::ERR, "err");
	BOOST_REQUIRE_EQUAL(2, w->msgs.size());
	BOOST_REQUIRE_EQUAL(1, e->msgs.size());

	log->message(LogLevel::CRIT, "crit");
	BOOST_REQUIRE_EQUAL(3, w->msgs.size());
	BOOST_REQUIRE_EQUAL(2, e->msgs.size());

	log->message(LogLevel::ALERT, "alert");
	BOOST_REQUIRE_EQUAL(4, w->msgs.size());
	BOOST_REQUIRE_EQUAL(3, e->msgs.size());

	log->message(LogLevel::EMERG, "emerg");
	BOOST_REQUIRE_EQUAL(5, w->msgs.size());
	BOOST_REQUIRE_EQUAL(4, e->msgs.size());

	manager.removeWriter(wp);
	log->message(LogLevel::ERR, "err2");
	BOOST_REQUIRE_EQUAL(5, w->msgs.size());
	BOOST_REQUIRE_EQUAL(5, e->msgs.size());
}

BOOST_AUTO_TEST_CASE(no_domains)
{
	auto d = std::make_shared<TestLogWriter>();
	manager.addWriter(add(d));
	log->message(LogLevel::DEBUG, "debug message.");
	log->message(LogLevel::EMERG, "emergency message.");
	BOOST_REQUIRE(d->msgs.empty());
}

BOOST_AUTO_TEST_CASE(formatter_plain)
{
	auto d = std::make_shared<TestLogWriter>(LogLevel::DEBUG);
	manager.addWriter(add(d));
	log->message(LogLevel::DEBUG, "plain message.");
	BOOST_REQUIRE_EQUAL(1, d->msgs.size());
	BOOST_REQUIRE_EQUAL("plain message.", d->msgs.front().message);
	BOOST_REQUIRE_EQUAL(testDomain, d->msgs.front().domain);
}

BOOST_AUTO_TEST_CASE(formatter_libc)
{
	auto d = std::make_shared<TestLogWriter>(LogLevel::DEBUG);
	manager.addWriter(add(d));
	log->messagef(LogLevel::DEBUG, "plain %s.", "message");
	BOOST_REQUIRE_EQUAL(1, d->msgs.size());
	BOOST_REQUIRE_EQUAL("plain message.", d->msgs.front().message);
	BOOST_REQUIRE_EQUAL(testDomain, d->msgs.front().domain);
}

BOOST_AUTO_TEST_CASE(formatter_boost_format)
{
	auto d = std::make_shared<TestLogWriter>(LogLevel::DEBUG);
	manager.addWriter(add(d));
	log->messagebf(LogLevel::DEBUG, "plain %s", std::string("message"));
	BOOST_REQUIRE_EQUAL(1, d->msgs.size());
	BOOST_REQUIRE_EQUAL("plain message", d->msgs.front().message);
	BOOST_REQUIRE_EQUAL(testDomain, d->msgs.front().domain);
}

AdHocFormatter(Plain, "plain %?.");
BOOST_AUTO_TEST_CASE(formatter_adhoc_compiletime)
{
	auto d = std::make_shared<TestLogWriter>(LogLevel::DEBUG);
	manager.addWriter(add(d));
	log->messagectf<Plain>(LogLevel::DEBUG, "message");
	BOOST_REQUIRE_EQUAL(1, d->msgs.size());
	BOOST_REQUIRE_EQUAL("plain message.", d->msgs.front().message);
	BOOST_REQUIRE_EQUAL(testDomain, d->msgs.front().domain);
}

BOOST_AUTO_TEST_CASE(domains_none)
{
	// No domains
	auto l = add(std::make_shared<TestLogWriter>());
	BOOST_REQUIRE(!l->level(test));
	BOOST_REQUIRE(!l->level(testDomain));
	BOOST_REQUIRE(!l->lowestLevel());
}

BOOST_AUTO_TEST_CASE(domains_single)
{
	// A single catch-all domain at the given level
	auto l = add(std::make_shared<TestLogWriter>(LogLevel::ERR));
	BOOST_REQUIRE_EQUAL(LogLevel::ERR, *l->level(test));
	BOOST_REQUIRE_EQUAL(LogLevel::ERR, *l->level(testDomain));
	BOOST_REQUIRE(l->lowestLevel());
	BOOST_REQUIRE_EQUAL(LogLevel::ERR, *l->lowestLevel());
}

BOOST_AUTO_TEST_CASE(domains_fromNullProperties)
{
	// A single catch-all domain at the default level (WARNING)
	auto l = add(std::make_shared<TestLogWriter>("", Ice::PropertiesPtr()));
	BOOST_REQUIRE_EQUAL(LogLevel::WARNING, *l->level(test));
	BOOST_REQUIRE_EQUAL(LogLevel::WARNING, *l->level(testDomain));
	BOOST_REQUIRE(l->lowestLevel());
	BOOST_REQUIRE_EQUAL(LogLevel::WARNING, *l->lowestLevel());
}

BOOST_AUTO_TEST_CASE(domains_fromProperties)
{
	// Domains configured according to properties
	Ice::PropertiesPtr p = ic->getProperties();
	p->setProperty("TestLogWriter.domains.test.domain", "EMERG");
	p->setProperty("TestLogWriter.domains.test.debug", "DEBUG");
	p->setProperty("TestLogWriter.domains", "ignored");
	p->setProperty("TestLogWriter.default", "WARNING");
	auto l = add(std::make_shared<TestLogWriter>("TestLogWriter", p));
	BOOST_REQUIRE_EQUAL(LogLevel::WARNING, *l->level(test));
	BOOST_REQUIRE_EQUAL(LogLevel::WARNING, *l->level(other));
	BOOST_REQUIRE_EQUAL(LogLevel::EMERG, *l->level(testDomain));
	BOOST_REQUIRE_EQUAL(LogLevel::DEBUG, *l->level(testDebug));
	BOOST_REQUIRE(l->lowestLevel());
	BOOST_REQUIRE_EQUAL(LogLevel::DEBUG, *l->lowestLevel());
}

BOOST_AUTO_TEST_CASE(domains_fromProperties_noDefault)
{
	// Domains configured according to properties
	Ice::PropertiesPtr p = ic->getProperties();
	p->setProperty("TestLogWriter.domains.test.domain", "EMERG");
	p->setProperty("TestLogWriter.domains.test.debug", "DEBUG");
	auto l = add(std::make_shared<TestLogWriter>("TestLogWriter", p));
	BOOST_REQUIRE_EQUAL(LogLevel::EMERG, *l->level(testDomain));
	BOOST_REQUIRE_EQUAL(LogLevel::DEBUG, *l->level(testDebug));
	BOOST_REQUIRE(l->lowestLevel());
	BOOST_REQUIRE_EQUAL(LogLevel::DEBUG, *l->lowestLevel());
}

BOOST_AUTO_TEST_CASE(domains_fromProperties_onlyDefault)
{
	// Domains configured according to properties
	Ice::PropertiesPtr p = ic->getProperties();
	p->setProperty("TestLogWriter.default", "INFO");
	auto l = add(std::make_shared<TestLogWriter>("TestLogWriter", p));
	BOOST_REQUIRE_EQUAL(LogLevel::INFO, *l->level(test));
	BOOST_REQUIRE_EQUAL(LogLevel::INFO, *l->level(other));
	BOOST_REQUIRE_EQUAL(LogLevel::INFO, *l->level(testDomain));
	BOOST_REQUIRE_EQUAL(LogLevel::INFO, *l->level(testDebug));
	BOOST_REQUIRE(l->lowestLevel());
	BOOST_REQUIRE_EQUAL(LogLevel::INFO, *l->lowestLevel());
}

BOOST_AUTO_TEST_CASE(domains_fromProperties_badLevel)
{
	Ice::PropertiesPtr p = ic->getProperties();
	p->setProperty("TestLogWriter.domains.test.domain", "BAD");
	BOOST_REQUIRE_THROW({ TestLogWriter tlw("TestLogWriter", p); }, Slicer::InvalidEnumerationSymbol);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ts, TestService)

BOOST_AUTO_TEST_CASE(getLogger)
{
	auto ic = Ice::initialize();
	ic->getProperties()->setProperty("test.Endpoints", "default");
	ic->getProperties()->setProperty("TestLogWriter.default", "INFO");
	this->start("test", ic, {});
	auto logger = LOGMANAGER()->getLogger("test.domain");
	BOOST_REQUIRE(logger);
	BOOST_REQUIRE_EQUAL(testDomain, logger->getDomain());
	this->stop();
	ic->destroy();
}

BOOST_AUTO_TEST_CASE(getLoggerForType)
{
	auto logger = LOGMANAGER()->getLogger<IceTray::Service>();
	BOOST_REQUIRE(logger);
	Domain expected = {"IceTray", "Service"};
	BOOST_REQUIRE_EQUAL(expected, logger->getDomain());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(syslog)
{
	IceTray::Logging::LogWriterPtr lwp = IceTray::Logging::LogWriterFactory::createNew("SyslogLogWriter", nullptr);
	lwp->message(LogLevel::DEBUG, testDomain, "some message", {});
}

BOOST_AUTO_TEST_CASE(console)
{
	IceTray::Logging::LogWriterPtr lwp = IceTray::Logging::LogWriterFactory::createNew("ConsoleLogWriter", nullptr);
	lwp->message(LogLevel::DEBUG, testDomain, "some message", {});
}

BOOST_AUTO_TEST_CASE(consoleNoWidth)
{
	std::stringstream str;
	ConsoleLogWriter::writeStream(str, -1, LogLevel::DEBUG, testDomain, "message");
	BOOST_REQUIRE_EQUAL("DEBUG: test.domain: message\n", str.str());
}

BOOST_AUTO_TEST_CASE(consoleWidthJustRight)
{
	std::stringstream str;
	ConsoleLogWriter::writeStream(str, 11, LogLevel::DEBUG, testDomain, "message");
	BOOST_REQUIRE_EQUAL("DEBUG: test.domain: message\n", str.str());
}

BOOST_AUTO_TEST_CASE(consoleWidthSmall)
{
	std::stringstream str;
	ConsoleLogWriter::writeStream(str, 10, LogLevel::DEBUG, testDomain, "message");
	BOOST_REQUIRE_EQUAL("DEBUG: t.domain: message\n", str.str());
}

BOOST_AUTO_TEST_CASE(consoleWidthTiny)
{
	std::stringstream str;
	ConsoleLogWriter::writeStream(str, 8, LogLevel::DEBUG, testDomain, "message");
	BOOST_REQUIRE_EQUAL("DEBUG: t.domain: message\n", str.str());
}

BOOST_AUTO_TEST_CASE(consoleWidthTooTiny)
{
	std::stringstream str;
	ConsoleLogWriter::writeStream(str, 7, LogLevel::DEBUG, testDomain, "message");
	BOOST_REQUIRE_EQUAL("DEBUG: t.d: message\n", str.str());
}

BOOST_AUTO_TEST_CASE(consoleWidthOverflow)
{
	std::stringstream str;
	ConsoleLogWriter::writeStream(str, 1, LogLevel::DEBUG, testDomain, "message");
	BOOST_REQUIRE_EQUAL("DEBUG: t.d: message\n", str.str());
}

BOOST_AUTO_TEST_CASE(consoleNoDomain)
{
	std::stringstream str;
	ConsoleLogWriter::writeStream(str, 0, LogLevel::DEBUG, {}, "message");
	BOOST_REQUIRE_EQUAL("DEBUG: : message\n", str.str());
}
