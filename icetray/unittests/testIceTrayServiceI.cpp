#include "testIceTrayServiceI.h"
#include "basicDataClient.h"
#include "icecube.h"
#include "testIceTrayService.h"
#include "transactionalDatabaseClient.h"
#include <Ice/Communicator.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/Optional.h>
#include <abstractCachingDatabaseClient.h>
#include <boost/assert.hpp>
#include <boost/test/unit_test.hpp>
#include <command.h>
#include <connectionPool.h>
#include <factory.h>
#include <icetrayService.h>
#include <iosfwd>
#include <memory>
#include <optional>
#include <staticSqlSource.h>
#include <subdir/a/more.sql.h>
#include <subdir/some.sql.h>
#include <testIceTrayServiceTestSql.sql.h>
#include <testIceTrayServiceTestSqlUpdate.sql.h>
// IWYU pragma: no_include "plugins.impl.h"
// IWYU pragma: no_include "resourcePool.impl.h"

namespace Ice {
	struct Current;
}
class Foo {
};

namespace DB {
	template<>
	void
	Command::bindParam(unsigned int o, const Foo &)
	{
		bindNull(o);
	}
}

namespace TestIceTray {
	TestIceTrayServiceI::TestIceTrayServiceI(const DB::ConnectionPoolPtr & d) :
		IceTray::AbstractCachingDatabaseClient(d)
	{
	}
	template<typename Type>
	void
	TestIceTrayServiceI::fetchTest(const Type & value)
	{
		BOOST_REQUIRE_EQUAL(0, (fetch<int, int, Type>(sql::testIceTrayServiceTestSql, 1, value)));
		BOOST_REQUIRE_EQUAL(
				0, (fetch<int, std::optional<Type>, std::optional<Type>>(sql::testIceTrayServiceTestSql, {}, value)));
		BOOST_REQUIRE_EQUAL(0,
				(fetch<int, Ice::optional<Type>, Ice::optional<Type>>(
						sql::testIceTrayServiceTestSql, IceUtil::None, value)));
	}

	void
	TestIceTrayServiceI::method1(const Ice::Current &)
	{
		fetch<int>(sql::subdir::some);
		fetch<int>(sql::subdir::a::more);
		// Just check we can bind all the ICE types
		fetchTest<bool>(true);
		fetchTest<Ice::Byte>(1);
		fetchTest<Ice::Short>(1);
		fetchTest<Ice::Int>(100000);
		fetchTest<Ice::Long>(10000000);
		fetchTest<Ice::Float>(0.1F);
		fetchTest<Ice::Double>(3.14);
		fetchTest<std::string>("some string");
		fetchTest<unsigned long>(20000000);
		// BLOB
		Ice::ByteSeq blob;
		fetchTest<Ice::ByteSeq>(blob);
		// Test we can bind nulls
		fetchTest<std::nullptr_t>(nullptr);
		// Test we can extend to bind other things
		Foo foo;
		fetchTest<Foo>(foo);
		// Test we can provide our own DB connection
		auto dbc = db->get();
		fetch<int>(dbc.get(), sql::testIceTrayServiceTestSql, 1, 1);
	}

	void
	TestIceTrayServiceI::method2(Ice::Int id, std::string name, const Ice::Current &)
	{
		BOOST_VERIFY((fetchCache<int>(sql::testIceTrayServiceTestSql, 10, id, name))
				== (fetchCache<int>(sql::testIceTrayServiceTestSql, 10, id, name)));
	}

	Ice::Int
	TestIceTrayServiceI::method3(const Ice::Current &)
	{
		return static_cast<Ice::Int>(transactional().modify(sql::testIceTrayServiceTestSqlUpdate, 5));
	}

	void
	TestCubeI::method1()
	{
	}

	void
	TestCubeI::method2(Ice::Int, const std::string &)
	{
	}

	void
	TestService::addObjects(const std::string &, const Ice::CommunicatorPtr & ic, const Ice::StringSeq &,
			const Ice::ObjectAdapterPtr & adp)
	{
		IceTray::Cube::addObject<TestIceTray::TestIceTrayService, TestIceTray::TestIceTrayServiceI>(
				adp, "test", getConnectionPool(ic, "postgresql", "icetraydb"));
		IceTray::Cube::add<TestIceTray::TestCube, TestIceTray::TestCubeI>();
	}

	NAMEDFACTORY("default", TestService, IceTray::ServiceFactory)
}
