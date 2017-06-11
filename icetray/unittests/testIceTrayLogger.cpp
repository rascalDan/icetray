#define BOOST_TEST_MODULE TestIceTrayLogger
#include <boost/test/unit_test.hpp>

#include <logger.h>
#include <icetrayService.h>
#include <Ice/Initialize.h>
#include <Ice/Communicator.h>
#include <Ice/ObjectAdapter.h>
#include <compileTimeFormatter.h>
#include <boost/format.hpp>

using namespace IceTray::Logging;

struct LogEntry {
	std::string domain;
	std::string message;
};

class TestLogWriter : public LogWriter {
	public:
		TestLogWriter(LogLevel ll) :
			calls(0),
			l(ll)
		{
		}

		IceUtil::Optional<LogLevel> level(const std::string &, const Ice::Current &) override
		{
			return l;
		}

		void message(LogLevel priority, const std::string & domain, const std::string & message, const Ice::Current &) override
		{
			calls += 1;
			BOOST_REQUIRE(l >= priority);
			msgs.push_back({domain, message});
		}

		std::vector<LogEntry> msgs;
		unsigned int calls;

	private:
		const LogLevel l;
};

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

	private:
		Ice::CommunicatorPtr ic;
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
	BOOST_REQUIRE_EQUAL(0, w->calls);
	BOOST_REQUIRE_EQUAL(0, e->calls);

	log->message(INFO, "into");
	BOOST_REQUIRE(w->msgs.empty());
	BOOST_REQUIRE(e->msgs.empty());
	BOOST_REQUIRE_EQUAL(0, w->calls);
	BOOST_REQUIRE_EQUAL(0, e->calls);

	log->message(NOTICE, "notice");
	BOOST_REQUIRE(w->msgs.empty());
	BOOST_REQUIRE(e->msgs.empty());
	BOOST_REQUIRE_EQUAL(0, w->calls);
	BOOST_REQUIRE_EQUAL(0, e->calls);

	log->message(WARNING, "warning");
	BOOST_REQUIRE_EQUAL(1, w->msgs.size());
	BOOST_REQUIRE(e->msgs.empty());
	BOOST_REQUIRE_EQUAL(1, w->calls);
	BOOST_REQUIRE_EQUAL(0, e->calls);

	log->message(ERR, "err");
	BOOST_REQUIRE_EQUAL(2, w->msgs.size());
	BOOST_REQUIRE_EQUAL(1, e->msgs.size());
	BOOST_REQUIRE_EQUAL(2, w->calls);
	BOOST_REQUIRE_EQUAL(1, e->calls);

	log->message(CRIT, "crit");
	BOOST_REQUIRE_EQUAL(3, w->msgs.size());
	BOOST_REQUIRE_EQUAL(2, e->msgs.size());
	BOOST_REQUIRE_EQUAL(3, w->calls);
	BOOST_REQUIRE_EQUAL(2, e->calls);

	log->message(ALERT, "alert");
	BOOST_REQUIRE_EQUAL(4, w->msgs.size());
	BOOST_REQUIRE_EQUAL(3, e->msgs.size());
	BOOST_REQUIRE_EQUAL(4, w->calls);
	BOOST_REQUIRE_EQUAL(3, e->calls);

	log->message(EMERG, "emerg");
	BOOST_REQUIRE_EQUAL(5, w->msgs.size());
	BOOST_REQUIRE_EQUAL(4, e->msgs.size());
	BOOST_REQUIRE_EQUAL(5, w->calls);
	BOOST_REQUIRE_EQUAL(4, e->calls);

	manager.removeWriter(wp);
	log->message(ERR, "err2");
	BOOST_REQUIRE_EQUAL(5, w->msgs.size());
	BOOST_REQUIRE_EQUAL(5, e->msgs.size());
	BOOST_REQUIRE_EQUAL(5, w->calls);
	BOOST_REQUIRE_EQUAL(5, e->calls);
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
	auto logger = LOGMANAGER()->getLogger("test.domain");
	BOOST_REQUIRE(logger);
}

BOOST_AUTO_TEST_SUITE_END();

