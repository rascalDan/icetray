#define BOOST_TEST_MODULE TestIceBoxInterface
#include <boost/test/unit_test.hpp>

#include <dlfcn.h>
#include <icetrayService.h>

BOOST_AUTO_TEST_CASE( IceBoxInterface )
{
	typedef IceTray::Service *(* SetupFunction)(Ice::CommunicatorPtr);

	void * i = dlsym(NULL, "createIceTrayService");
	BOOST_REQUIRE(i);
	auto sf = (SetupFunction)i;
	BOOST_REQUIRE(sf);
	auto service = sf(nullptr);
	BOOST_REQUIRE(service);
	BOOST_REQUIRE_EQUAL(service, IceTray::Service::getCurrent());
	delete service;
}

