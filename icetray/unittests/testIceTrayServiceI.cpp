#include "testIceTrayServiceI.h"
#include <factory.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/Communicator.h>
#include <boost/assert.hpp>
#include <testIceTrayServiceTestSql.sql.h>
#include <subdir/some.sql.h>
#include <subdir/a/more.sql.h>

namespace TestIceTray {
	TestIceTrayServiceI::TestIceTrayServiceI(IceTray::DatabasePoolPtr d) :
		IceTray::AbstractCachingDatabaseClient(d)
	{
	}

	void TestIceTrayServiceI::method1(const Ice::Current &)
	{
		fetch<int, sql::subdir::some>();
		fetch<int, sql::subdir::a::more>();
	}

	void TestIceTrayServiceI::method2(Ice::Int id, const std::string & name, const Ice::Current &)
	{
		BOOST_VERIFY((fetchCache<int, sql::testIceTrayServiceTestSql>(10, id, name)) == (fetchCache<int, sql::testIceTrayServiceTestSql>(10, id, name)));
	}

	void TestService::addObjects(const std::string &, const Ice::CommunicatorPtr & ic, const Ice::StringSeq &, const Ice::ObjectAdapterPtr & adp)
	{
		adp->add(new TestIceTray::TestIceTrayServiceI(getConnectionPool(ic, "postgresql", "icetraydb")), ic->stringToIdentity("test"));
	}

	NAMEDFACTORY("default", TestService, IceTray::ServiceFactory);
}

