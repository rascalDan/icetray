#define BOOST_TEST_MODULE TestIceTrayLogger
#include <boost/test/unit_test.hpp>

#include "testOptions.h"
#include <Ice/Communicator.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/Initialize.h>
#include "icetrayService.h"
#include "dryice.h"

//
// TestService is pulled in from libtestService.so
//

class DI : public IceTray::DryIce {
	public:
		DI(const Ice::StringSeq & opts) : IceTray::DryIce(opts) { }
};

BOOST_AUTO_TEST_CASE( testOptions )
{
	DI di({ "--testInt=3", "--vec=1,2,4,8" });
	IceTray::OptionsResolver<TestOptions> myOpts;
	BOOST_REQUIRE_EQUAL(3, myOpts->testInt);
	BOOST_REQUIRE_EQUAL("some string", myOpts->testString);
	BOOST_REQUIRE_EQUAL(4, myOpts->testVec.size());
	BOOST_REQUIRE_EQUAL(1, myOpts->testVec[0]);
	BOOST_REQUIRE_EQUAL(2, myOpts->testVec[1]);
	BOOST_REQUIRE_EQUAL(4, myOpts->testVec[2]);
	BOOST_REQUIRE_EQUAL(8, myOpts->testVec[3]);
}

