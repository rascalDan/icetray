#include "testIceTrayServiceI.h"
#include <factory.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/Communicator.h>
#include <boost/assert.hpp>
#include <testIceTrayServiceTestSql.sql.h>
#include <subdir/some.sql.h>
#include <subdir/a/more.sql.h>
#include <boost/test/test_tools.hpp>

class Foo {  };

namespace IceTray {
	template<>
	void
	AbstractDatabaseClient::bind1(int o, DB::Command * cmd, const Foo &) \
	{
		cmd->bindNull(o);
	}
}

namespace TestIceTray {
	TestIceTrayServiceI::TestIceTrayServiceI(IceTray::DatabasePoolPtr d) :
		IceTray::AbstractCachingDatabaseClient(d)
	{
	}
	template<typename Type>
	void
	TestIceTrayServiceI::fetchTest(const Type & value)
	{
		BOOST_REQUIRE_EQUAL(0, (fetch<int, int, Type>(sql::testIceTrayServiceTestSql, 1, value)));
		BOOST_REQUIRE_EQUAL(0, (fetch<int, boost::optional<Type>, boost::optional<Type>>(sql::testIceTrayServiceTestSql, boost::optional<Type>(), value)));
		BOOST_REQUIRE_EQUAL(0, (fetch<int, IceUtil::Optional<Type>, IceUtil::Optional<Type>>(sql::testIceTrayServiceTestSql, IceUtil::None, value)));
	}

	void TestIceTrayServiceI::method1(const Ice::Current &)
	{
		fetch<int>(sql::subdir::some);
		fetch<int>(sql::subdir::a::more);
		// Just check we can bind all the ICE types
		fetchTest<bool>(true);
		fetchTest<Ice::Byte>(1);
		fetchTest<Ice::Short>(1);
		fetchTest<Ice::Int>(100000);
		fetchTest<Ice::Long>(10000000);
		fetchTest<Ice::Float>(0.1f);
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

	void TestIceTrayServiceI::method2(Ice::Int id, const std::string & name, const Ice::Current &)
	{
		BOOST_VERIFY((fetchCache<int>(sql::testIceTrayServiceTestSql, 10, id, name)) == (fetchCache<int>(sql::testIceTrayServiceTestSql, 10, id, name)));
	}

	void TestService::addObjects(const std::string &, const Ice::CommunicatorPtr & ic, const Ice::StringSeq &, const Ice::ObjectAdapterPtr & adp)
	{
		adp->add(new TestIceTray::TestIceTrayServiceI(getConnectionPool(ic, "postgresql", "icetraydb")), ic->stringToIdentity("test"));
	}

	NAMEDFACTORY("default", TestService, IceTray::ServiceFactory);
}

