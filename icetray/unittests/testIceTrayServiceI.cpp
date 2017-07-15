#include "testIceTrayServiceI.h"
#include <factory.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/Communicator.h>
#include <boost/assert.hpp>
#include <testIceTrayServiceTestSql.sql.h>
#include <subdir/some.sql.h>
#include <subdir/a/more.sql.h>

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

	void TestIceTrayServiceI::method1(const Ice::Current &)
	{
		fetch<int>(sql::subdir::some);
		fetch<int>(sql::subdir::a::more);
		// Just check we can bind all the ICE types
		fetch<int, Ice::Byte, bool>(sql::testIceTrayServiceTestSql, 1, true);
		fetch<int, Ice::Short, Ice::Float>(sql::testIceTrayServiceTestSql, 1, 0.1f);
		fetch<int, Ice::Long, Ice::Double>(sql::testIceTrayServiceTestSql, 100000, 3.14);
		// BLOB
		Ice::ByteSeq blob;
		fetch<int>(sql::testIceTrayServiceTestSql, blob, blob.size());
		// Test we can bind optionals and nulls
		fetch<int, boost::optional<Ice::Byte>, IceUtil::Optional<bool>>
			(sql::testIceTrayServiceTestSql, boost::optional<Ice::Byte>(10), IceUtil::Optional<bool>());
		fetch<int>(sql::testIceTrayServiceTestSql, 10, nullptr);
		// Test we can extend to bind other things
		Foo foo;
		fetch<int>(sql::testIceTrayServiceTestSql, Foo(), foo);
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

