#define BOOST_TEST_MODULE TestIceTray
#include <boost/test/unit_test.hpp>

#include <dryice.h>
#include <Ice/Communicator.h>
#include "testIceTrayServiceI.h"

#include <pq-mock.h>
#include <definedDirs.h>

class Service : public IceTray::DryIce, PQ::Mock {
	public:
		Service() :
			PQ::Mock("user=postgres dbname=postgres", "icetraydb", {
				rootDir / "testIceTrayService.sql"
			})
		{
		}
};

BOOST_GLOBAL_FIXTURE( Service );

class Client : public IceTray::DryIceClient {
	public:
		Client() :
			p(TestIceTray::TestIceTrayServicePrx::checkedCast(ic->stringToProxy("test:tcp -p 9002")))
		{
		}
		TestIceTray::TestIceTrayServicePrx p;
};

BOOST_FIXTURE_TEST_SUITE( client, Client );

BOOST_AUTO_TEST_CASE( startup )
{
	BOOST_REQUIRE(ic);
}

BOOST_AUTO_TEST_CASE( services )
{
	BOOST_REQUIRE(p);
	p->ice_ping();
	p->method1();
	p->method2(1, "test");
}

BOOST_AUTO_TEST_SUITE_END();

