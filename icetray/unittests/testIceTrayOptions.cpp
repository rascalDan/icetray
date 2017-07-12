#define BOOST_TEST_MODULE TestIceTrayLogger
#include <boost/test/unit_test.hpp>

#include "testOptions.h"
#include <Ice/Communicator.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/Initialize.h>
#include "icetrayService.h"
#include "dryice.h"

class TestService : public IceTray::Service {
	public:
		void addObjects(const std::string &, const Ice::CommunicatorPtr &, const Ice::StringSeq &, const Ice::ObjectAdapterPtr &)
		{
			// Verifies option resolution is available for addObjects.
			IceTray::OptionsResolver<TestOptions> myOpts;
		}
};
NAMEDFACTORY("default", TestService, IceTray::ServiceFactory);

class DI : public IceTray::DryIce {
	public:
		DI() : IceTray::DryIce({
			"--testInt=3",
			"--vec=1,2,4,8"
		})
		{
		}
};
BOOST_GLOBAL_FIXTURE(DI);

BOOST_AUTO_TEST_CASE( testOptions )
{
	IceTray::OptionsResolver<TestOptions> myOpts;
	BOOST_REQUIRE_EQUAL(3, myOpts->testInt);
	BOOST_REQUIRE_EQUAL("some string", myOpts->testString);
	BOOST_REQUIRE_EQUAL(4, myOpts->testVec.size());
	BOOST_REQUIRE_EQUAL(1, myOpts->testVec[0]);
	BOOST_REQUIRE_EQUAL(2, myOpts->testVec[1]);
	BOOST_REQUIRE_EQUAL(4, myOpts->testVec[2]);
	BOOST_REQUIRE_EQUAL(8, myOpts->testVec[3]);
}

