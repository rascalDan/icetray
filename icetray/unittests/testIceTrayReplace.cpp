#define BOOST_TEST_MODULE TestIceTray
#include <boost/test/unit_test.hpp>

#include "testIceTrayService.h"
#include "testIceTrayServiceI.h"
#include <Ice/Config.h>
#include <dryice.h>
#include <memory>
#include <string>
// IWYU pragma: no_include "plugins.impl.h"
namespace Ice {
	struct Current;
}

namespace TestIceTray {
	class DummyTestIceTrayServiceI : public TestIceTrayService {
	public:
		void
		method1(const Ice::Current &) override
		{
		}
		void
		method2(Ice::Int, std::string, const Ice::Current &) override
		{
		}
		Ice::Int
		method3(const Ice::Current &) override
		{
			return 0;
		}
	};
}

class Service : public IceTray::DryIce {
public:
	Service()
	{
		replace("test", std::make_shared<TestIceTray::DummyTestIceTrayServiceI>());
		replace<TestIceTray::TestCube, TestIceTray::TestCubeI>();
	}
};

BOOST_GLOBAL_FIXTURE(Service);

class Client : public IceTray::DryIceClient {
public:
	Client() : p(getProxy<TestIceTray::TestIceTrayServicePrx>("test")) { }
	TestIceTray::TestIceTrayServicePrxPtr p;
};

BOOST_FIXTURE_TEST_SUITE(client, Client)

BOOST_AUTO_TEST_CASE(services)
{
	BOOST_REQUIRE(p);
	p->ice_ping();
	p->method1();
	p->method2(1, "test");
}

BOOST_AUTO_TEST_SUITE_END()
