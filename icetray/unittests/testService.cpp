#include "testService.h"
#include "testOptions.h"

TestService::~TestService()
{
}

void
TestService::addObjects(const std::string &, const Ice::CommunicatorPtr &, const Ice::StringSeq &, const Ice::ObjectAdapterPtr &)
{
	// Verifies option resolution is available for addObjects.
	IceTray::OptionsResolver<TestOptions> myOpts;
}

FACTORY(TestService, IceTray::ServiceFactory);

