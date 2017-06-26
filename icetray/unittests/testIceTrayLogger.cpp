#define BOOST_TEST_MODULE TestIceTrayLogger
#include <boost/test/unit_test.hpp>

#include <logger.h>
#include <icetrayService.h>
#include <Ice/Initialize.h>
#include <Ice/Communicator.h>
#include <Ice/ObjectAdapter.h>
#include <compileTimeFormatter.h>
#include <boost/format.hpp>
#include <slicer/common.h>

using namespace IceTray::Logging;

struct LogEntry {
	LogLevel priority;
	std::string domain;
	std::string message;
};

class TestLogWriter : public AbstractLogWriter {
	public:
		template <typename ... T>
		TestLogWriter(const T & ... t) :
			AbstractLogWriter(t...)
		{
		}

		TestLogWriter(Ice::Properties * p) :
			AbstractLogWriter("TestLogWriter", p)
		{
		}

		void message(LogLevel priority, const std::string & domain, const std::string & message, const Ice::Current &) override
		{
			msgs.push_back({priority, domain, message});
		}

		std::vector<LogEntry> msgs;
};
FACTORY(TestLogWriter, LogWriterFactory);

class TestLogImpl {
	public:
		TestLogImpl() :
			ic(Ice::initialize()),
			adp(ic->createObjectAdapterWithEndpoints("test", "default"))
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

		LogWriterPrx add(LogWriter * w)
		{
			return LogWriterPrx::uncheckedCast(adp->addWithUUID(w));
		}

	protected:
		LoggerPtr log;
		LogManager manager;
		Ice::CommunicatorPtr ic;

	private:
		Ice::ObjectAdapterPtr adp;
};

BOOST_FIXTURE_TEST_SUITE(li, TestLogImpl);

BOOST_AUTO_TEST_CASE(no_writers) {
	log->message(EMERG, "");
	log->message(DEBUG, "");
}

BOOST_AUTO_TEST_CASE(priority_filtering) {
	auto w = new TestLogWriter(WARNING);
	auto e = new TestLogWriter(ERR);
	auto wp = add(w);
	auto ep = add(e);
	manager.addWriter(wp);
	manager.addWriter(ep);
	BOOST_REQUIRE(w->msgs.empty());
	BOOST_REQUIRE(e->msgs.empty());

	log->message(DEBUG, "debug");
	BOOST_REQUIRE(w->msgs.empty());
	BOOST_REQUIRE(e->msgs.empty());

	log->message(INFO, "into");
	BOOST_REQUIRE(w->msgs.empty());
	BOOST_REQUIRE(e->msgs.empty());

	log->message(NOTICE, "notice");
	BOOST_REQUIRE(w->msgs.empty());
	BOOST_REQUIRE(e->msgs.empty());

	log->message(WARNING, "warning");
	BOOST_REQUIRE_EQUAL(1, w->msgs.size());
	BOOST_REQUIRE(e->msgs.empty());

	log->message(ERR, "err");
	BOOST_REQUIRE_EQUAL(2, w->msgs.size());
	BOOST_REQUIRE_EQUAL(1, e->msgs.size());

	log->message(CRIT, "crit");
	BOOST_REQUIRE_EQUAL(3, w->msgs.size());
	BOOST_REQUIRE_EQUAL(2, e->msgs.size());

	log->message(ALERT, "alert");
	BOOST_REQUIRE_EQUAL(4, w->msgs.size());
	BOOST_REQUIRE_EQUAL(3, e->msgs.size());

	log->message(EMERG, "emerg");
	BOOST_REQUIRE_EQUAL(5, w->msgs.size());
	BOOST_REQUIRE_EQUAL(4, e->msgs.size());

	manager.removeWriter(wp);
	log->message(ERR, "err2");
	BOOST_REQUIRE_EQUAL(5, w->msgs.size());
	BOOST_REQUIRE_EQUAL(5, e->msgs.size());
}

BOOST_AUTO_TEST_CASE( no_domains )
{
	auto d = new TestLogWriter();
	manager.addWriter(add(d));
	log->message(DEBUG, "debug message.");
	log->message(EMERG, "emergency message.");
	BOOST_REQUIRE(d->msgs.empty());
}

BOOST_AUTO_TEST_CASE(formatter_plain)
{
	auto d = new TestLogWriter(DEBUG);
	manager.addWriter(add(d));
	log->message(DEBUG, "plain message.");
	BOOST_REQUIRE_EQUAL(1, d->msgs.size());
	BOOST_REQUIRE_EQUAL("plain message.", d->msgs.front().message);
	BOOST_REQUIRE_EQUAL("test.domain", d->msgs.front().domain);
}

BOOST_AUTO_TEST_CASE(formatter_libc)
{
	auto d = new TestLogWriter(DEBUG);
	manager.addWriter(add(d));
	log->messagef(DEBUG, "plain %s.", "message");
	BOOST_REQUIRE_EQUAL(1, d->msgs.size());
	BOOST_REQUIRE_EQUAL("plain message.", d->msgs.front().message);
	BOOST_REQUIRE_EQUAL("test.domain", d->msgs.front().domain);
}

BOOST_AUTO_TEST_CASE(formatter_boost_format)
{
	auto d = new TestLogWriter(DEBUG);
	manager.addWriter(add(d));
	log->messagebf(DEBUG, "plain %s", std::string("message"));
	BOOST_REQUIRE_EQUAL(1, d->msgs.size());
	BOOST_REQUIRE_EQUAL("plain message", d->msgs.front().message);
	BOOST_REQUIRE_EQUAL("test.domain", d->msgs.front().domain);
}

AdHocFormatter(Plain, "plain %?.");
BOOST_AUTO_TEST_CASE(formatter_adhoc_compiletime)
{
	auto d = new TestLogWriter(DEBUG);
	manager.addWriter(add(d));
	log->messagectf<Plain>(DEBUG, "message");
	BOOST_REQUIRE_EQUAL(1, d->msgs.size());
	BOOST_REQUIRE_EQUAL("plain message.", d->msgs.front().message);
	BOOST_REQUIRE_EQUAL("test.domain", d->msgs.front().domain);
}

BOOST_AUTO_TEST_CASE( domains_none )
{
	// No domains
	auto l = add(new TestLogWriter());
	BOOST_REQUIRE(!l->level("test"));
	BOOST_REQUIRE(!l->level("test.domain"));
	BOOST_REQUIRE(!l->lowestLevel());
}

BOOST_AUTO_TEST_CASE( domains_single )
{
	// A single catch-all domain at the given level
	auto l = add(new TestLogWriter(ERR));
	BOOST_REQUIRE_EQUAL(ERR, *l->level("test"));
	BOOST_REQUIRE_EQUAL(ERR, *l->level("test.domain"));
	BOOST_REQUIRE(l->lowestLevel());
	BOOST_REQUIRE_EQUAL(ERR, *l->lowestLevel());
}

BOOST_AUTO_TEST_CASE( domains_fromNullProperties )
{
	// A single catch-all domain at the default level (WARNING)
	auto l = add(new TestLogWriter("", Ice::PropertiesPtr()));
	BOOST_REQUIRE_EQUAL(WARNING, *l->level("test"));
	BOOST_REQUIRE_EQUAL(WARNING, *l->level("test.domain"));
	BOOST_REQUIRE(l->lowestLevel());
	BOOST_REQUIRE_EQUAL(WARNING, *l->lowestLevel());
}

BOOST_AUTO_TEST_CASE( domains_fromProperties )
{
	// Domains configured according to properties
	Ice::PropertiesPtr p = ic->getProperties();
	p->setProperty("TestLogWriter.domains.test.domain", "EMERG");
	p->setProperty("TestLogWriter.domains.test.debug", "DEBUG");
	p->setProperty("TestLogWriter.domains", "ignored");
	p->setProperty("TestLogWriter.default", "WARNING");
	auto l = add(new TestLogWriter("TestLogWriter", p));
	BOOST_REQUIRE_EQUAL(WARNING, *l->level("test"));
	BOOST_REQUIRE_EQUAL(WARNING, *l->level("other"));
	BOOST_REQUIRE_EQUAL(EMERG, *l->level("test.domain"));
	BOOST_REQUIRE_EQUAL(DEBUG, *l->level("test.debug"));
	BOOST_REQUIRE(l->lowestLevel());
	BOOST_REQUIRE_EQUAL(DEBUG, *l->lowestLevel());
}

BOOST_AUTO_TEST_CASE( domains_fromProperties_noDefault )
{
	// Domains configured according to properties
	Ice::PropertiesPtr p = ic->getProperties();
	p->setProperty("TestLogWriter.domains.test.domain", "EMERG");
	p->setProperty("TestLogWriter.domains.test.debug", "DEBUG");
	auto l = add(new TestLogWriter("TestLogWriter", p));
	BOOST_REQUIRE_EQUAL(EMERG, *l->level("test.domain"));
	BOOST_REQUIRE_EQUAL(DEBUG, *l->level("test.debug"));
	BOOST_REQUIRE(l->lowestLevel());
	BOOST_REQUIRE_EQUAL(DEBUG, *l->lowestLevel());
}

BOOST_AUTO_TEST_CASE( domains_fromProperties_onlyDefault )
{
	// Domains configured according to properties
	Ice::PropertiesPtr p = ic->getProperties();
	p->setProperty("TestLogWriter.default", "INFO");
	auto l = add(new TestLogWriter("TestLogWriter", p));
	BOOST_REQUIRE_EQUAL(INFO, *l->level("test"));
	BOOST_REQUIRE_EQUAL(INFO, *l->level("other"));
	BOOST_REQUIRE_EQUAL(INFO, *l->level("test.domain"));
	BOOST_REQUIRE_EQUAL(INFO, *l->level("test.debug"));
	BOOST_REQUIRE(l->lowestLevel());
	BOOST_REQUIRE_EQUAL(INFO, *l->lowestLevel());
}

BOOST_AUTO_TEST_CASE( domains_fromProperties_badLevel )
{
	Ice::PropertiesPtr p = ic->getProperties();
	p->setProperty("TestLogWriter.domains.test.domain", "BAD");
	BOOST_REQUIRE_THROW({
		TestLogWriter tlw("TestLogWriter", p);
	}, Slicer::InvalidEnumerationSymbol);
}

BOOST_AUTO_TEST_SUITE_END();

class TestService : public IceTray::Service {
	public:
		void addObjects(const std::string &, const Ice::CommunicatorPtr &, const Ice::StringSeq &, const Ice::ObjectAdapterPtr &)
		{
		}
};

BOOST_FIXTURE_TEST_SUITE( ts, TestService );

BOOST_AUTO_TEST_CASE( getLogger )
{
	auto ic = Ice::initialize();
	ic->getProperties()->setProperty("test.Endpoints", "default");
	ic->getProperties()->setProperty("TestLogWriter.default", "INFO");
	this->start("test", ic, {});
	auto logger = LOGMANAGER()->getLogger("test.domain");
	BOOST_REQUIRE(logger);
	BOOST_REQUIRE_EQUAL("test.domain", logger->getDomain());
	this->stop();
	ic->destroy();
}

BOOST_AUTO_TEST_CASE( getLoggerForType )
{
	auto logger = LOGMANAGER()->getLogger<IceTray::Service>();
	BOOST_REQUIRE(logger);
	BOOST_REQUIRE_EQUAL("IceTray::Service", logger->getDomain());
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_AUTO_TEST_CASE( console )
{
	IceTray::Logging::LogWriterPtr lwp =
		IceTray::Logging::LogWriterFactory::createNew("ConsoleLogWriter", NULL);
	lwp->message(DEBUG, "some.domain", "some message");
}

