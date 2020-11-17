#define BOOST_TEST_MODULE TestIceTray
#include <boost/test/unit_test.hpp>

#include <Ice/Initialize.h>
#include <Ice/Properties.h>
#include <connectionPool.h>
#include <error.h>
#include <factory.h>
#include <factory.impl.h>
#include <icetrayService.h>

BOOST_AUTO_TEST_CASE(defaultPool)
{
	auto p = Ice::createProperties();
	p->setProperty("testcase.Database.ConnectionString", "user=template1 user=postgres");
	auto pool = IceTray::PoolProvider::createNew("DefaultPool", "testcase", "postgresql", p);
	BOOST_REQUIRE(pool);
	BOOST_REQUIRE_EQUAL(0, pool->inUseCount());
	{
		auto c = pool->get();
		c->ping();
	}
}
