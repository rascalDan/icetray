#define BOOST_TEST_MODULE TestIceTrayLogger
#include <boost/test/unit_test.hpp>

#include "testOptions.h"
#include <Ice/Communicator.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/Initialize.h>
#include "icetrayService.h"

class TestService : public IceTray::Service {
	public:
		void addObjects(const std::string &, const Ice::CommunicatorPtr &, const Ice::StringSeq &, const Ice::ObjectAdapterPtr &)
		{
		}
};

BOOST_AUTO_TEST_CASE( testOptions )
{
	auto ic = Ice::initialize();
	auto p = ic->getProperties();
	p->setProperty("test.Endpoints", "default");
	p->setProperty("testInt", "3");
	p->setProperty("vec", "1,2,4,8");
	BOOST_REQUIRE_EQUAL(4, p->getPropertyAsList("vec").size());

	TestService ts;
	ts.start("", ic, {});

	IceTray::OptionsResolver<TestOptions> myOpts;
	BOOST_REQUIRE_EQUAL(3, myOpts->testInt);
	BOOST_REQUIRE_EQUAL("some string", myOpts->testString);
	BOOST_REQUIRE_EQUAL(4, myOpts->testVec.size());
	BOOST_REQUIRE_EQUAL(1, myOpts->testVec[0]);
	BOOST_REQUIRE_EQUAL(2, myOpts->testVec[1]);
	BOOST_REQUIRE_EQUAL(4, myOpts->testVec[2]);
	BOOST_REQUIRE_EQUAL(8, myOpts->testVec[3]);

	ts.stop();
	ic->destroy();
}

