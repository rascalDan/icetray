#define BOOST_TEST_MODULE TestIceTray
#include <boost/test/unit_test.hpp>

#include "testIceTrayService.h"
#include <Ice/Proxy.h>
#include <definedDirs.h>
#include <dryice.h>
#include <filesystem>
#include <icecube.h>
#include <icetrayService.h>
#include <memory>
#include <mockDatabase.h>
#include <pq-mock.h>
#include <staticSqlSource.h>
#include <testIceTrayServiceTestSql.sql.h>

class Service : public IceTray::DryIce, DB::PluginMock<PQ::Mock> {
public:
	Service() :
		DB::PluginMock<PQ::Mock>("icetraydb", {rootDir / "testIceTrayService.sql"}, "user=postgres dbname=postgres")
	{
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
	BOOST_REQUIRE(IceTray::Service::getCurrent());
	p->ice_ping();
	p->method1();
	p->method2(1, "test");
}

BOOST_AUTO_TEST_CASE(cube_services_proxy)
{
	auto prx = IceTray::Cube::get<TestIceTray::TestIceTrayService>();
	BOOST_REQUIRE(prx);
	prx->ice_ping();
	prx->method1();
	prx->method2(1, "test");
}

BOOST_AUTO_TEST_CASE(cube_services_local)
{
	auto ptr = IceTray::Cube::get<TestIceTray::TestCube>();
	BOOST_REQUIRE(ptr);
	ptr->method1();
	ptr->method2(1, "test");
}

BOOST_AUTO_TEST_CASE(getIceComponents)
{
	BOOST_REQUIRE(getService());
	BOOST_REQUIRE(getAdapter());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(sqlModify)
{
	auto db = DB::MockDatabase::openConnectionTo("icetraydb");
	BOOST_REQUIRE(db);
	BOOST_REQUIRE(TestIceTray::sql::testIceTrayServiceTestSql.modify(db.get()));
}
