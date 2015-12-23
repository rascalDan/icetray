#include "testIceTrayServiceI.h"
#include <factory.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/Communicator.h>
#include <boost/assert.hpp>

namespace TestIceTray {
	struct TestSql {
		static const std::string sql;
		static const std::size_t hash;
	};
	const std::string TestSql::sql("SELECT COUNT(*) FROM testTable WHERE id = ? AND name = ?");
	const std::size_t TestSql::hash(std::hash<std::string>()(sql));
	TestIceTrayServiceI::TestIceTrayServiceI(boost::shared_ptr<AdHoc::ResourcePool<DB::Connection>> d) :
		IceTray::AbstractDatabaseClient(d)
	{
	}

	void TestIceTrayServiceI::method(const Ice::Current &)
	{
		Ice::Int id = 4;
		std::string name = "test";
		BOOST_VERIFY((fetch<int, TestSql>(id, name)) == (fetch<int, TestSql>(id, name)));
	}

	void TestService::addObjects(const std::string &, const Ice::CommunicatorPtr & ic, const Ice::StringSeq &, const Ice::ObjectAdapterPtr & adp)
	{
		adp->add(new TestIceTray::TestIceTrayServiceI(getConnectionPool(ic, "postgresql", "icetraydb")), ic->stringToIdentity("test"));
	}

	NAMEDFACTORY("default", TestService, IceTray::ServiceFactory);
}

