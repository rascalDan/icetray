#define BOOST_TEST_MODULE TestIceTrayLogger
#include <boost/test/unit_test.hpp>

#include "dryice.h"
#include "icetrayService.h"
#include "testOptions.h"
#include <Ice/Communicator.h>
#include <Ice/Initialize.h>
#include <Ice/ObjectAdapter.h>

//
// TestService is pulled in from libtestService.so
//

class DI : public IceTray::DryIce {
public:
	explicit DI(const Ice::StringSeq & opts) : IceTray::DryIce(opts) { }
};

BOOST_AUTO_TEST_CASE(testOptions)
{
	DI di({"--testInt=3", "--vec=1,2,4,8"});
	IceTray::OptionsResolver<TestOptions> myOpts;
	BOOST_REQUIRE_EQUAL(3, myOpts->testInt);
	BOOST_REQUIRE_EQUAL("some string", myOpts->testString);
	BOOST_REQUIRE_EQUAL(4, myOpts->testVec.size());
	BOOST_REQUIRE_EQUAL(1, myOpts->testVec[0]);
	BOOST_REQUIRE_EQUAL(2, myOpts->testVec[1]);
	BOOST_REQUIRE_EQUAL(4, myOpts->testVec[2]);
	BOOST_REQUIRE_EQUAL(8, myOpts->testVec[3]);
}

BOOST_AUTO_TEST_CASE(overrideDefaultWithQuotesString)
{
	DI di({R"C(--testString="some \"Quotes\" string")C"});
	IceTray::OptionsResolver<TestOptions> myOpts;
	BOOST_REQUIRE_EQUAL("some \"Quotes\" string", myOpts->testString);
}

BOOST_AUTO_TEST_CASE(overrideDefaultWithEmptyString)
{
	DI di({R"C(--testString="")C"});
	IceTray::OptionsResolver<TestOptions> myOpts;
	BOOST_REQUIRE(myOpts->testString.empty());
}
