#define BOOST_TEST_MODULE TestIceBoxInterface
#include <boost/test/unit_test.hpp>

#include <Ice/CommunicatorF.h>
#include <dlfcn.h>
#include <icetrayService.h>

BOOST_AUTO_TEST_CASE(IceBoxInterface)
{
	using SetupFunction = IceTray::Service * (*)(Ice::CommunicatorPtr);

	auto sf = reinterpret_cast<SetupFunction>(dlsym(nullptr, "createIceTrayService"));
	BOOST_REQUIRE(sf);
	auto service = sf(nullptr);
	BOOST_REQUIRE(service);
	BOOST_REQUIRE_EQUAL(service, IceTray::Service::getCurrent());
	delete service;
}
