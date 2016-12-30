#define BOOST_TEST_MODULE TestIceTray
#include <boost/test/unit_test.hpp>

#include <dryice.h>
#include <Ice/Communicator.h>
#include "testIceTrayServiceI.h"

#include <pq-mock.h>
#include <definedDirs.h>

namespace TestIceTray {
	class DummyTestIceTrayServiceI : public TestIceTrayService {
		public:
			DummyTestIceTrayServiceI() { }

			void method1(const Ice::Current &) override { }
			void method2(Ice::Int, const std::string &, const Ice::Current &) override { }
	};
}

class Service : public IceTray::DryIce {
	public:
		Service()
		{
			replace("test", new TestIceTray::DummyTestIceTrayServiceI());
		}
};

BOOST_GLOBAL_FIXTURE( Service );

class Client : public IceTray::DryIceClient {
	public:
		Client() :
			p(getProxy<TestIceTray::TestIceTrayServicePrx>("test"))
		{
		}
		TestIceTray::TestIceTrayServicePrx p;
};

BOOST_FIXTURE_TEST_SUITE( client, Client );

BOOST_AUTO_TEST_CASE( services )
{
	BOOST_REQUIRE(p);
	p->ice_ping();
	p->method1();
	p->method2(1, "test");
}

BOOST_AUTO_TEST_SUITE_END();

