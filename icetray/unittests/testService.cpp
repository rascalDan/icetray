#include "testService.h"
#include "testOptions.h"
#include <factory.h>
#include <icetrayService.h>
#include <options.h>

void
TestService::addObjects(
		const std::string &, const Ice::CommunicatorPtr &, const Ice::StringSeq &, const Ice::ObjectAdapterPtr &)
{
	// Verifies option resolution is available for addObjects.
	IceTray::OptionsResolver<TestOptions> myOpts;
}

FACTORY(TestService, IceTray::ServiceFactory)
