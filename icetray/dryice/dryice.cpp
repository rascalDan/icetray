#include "dryice.h"
#include <boost/assert.hpp>
#include <dlfcn.h>
#include <factory.h>
#include <Ice/Initialize.h>

namespace IceTray {
	typedef IceBox::Service *(* SetupFunction)(Ice::CommunicatorPtr);

	DryIce * DryIce::currentDryIce = nullptr;

	DryIce::DryIce(const Ice::StringSeq & cmdline)
	{
		BOOST_ASSERT(!currentDryIce);
		void * i = dlsym(NULL, "createIceTrayService");
		BOOST_VERIFY(i);
		auto sf = (SetupFunction)i;
		BOOST_VERIFY(sf);
		Ice::StringSeq args;
		Ice::InitializationData id;
		id.properties = Ice::createProperties();
		id.properties->setProperty("DryIce.Endpoints", "default");
		id.properties->setProperty("DryIce.PoolProvider", "MockPool");
		id.properties->parseCommandLineOptions("", cmdline);
		ic = Ice::initialize(args, id);
		s = sf(nullptr);
		s->start("DryIce", ic, {});
		currentDryIce = this;
	}

	DryIce::~DryIce()
	{
		currentDryIce = nullptr;
		if (s) {
			s->stop();
			s = NULL;
		}
		if (ic) {
			ic->destroy();
			ic = NULL;
		}
	}

	DryIceClient::DryIceClient()
	{
	}

	DryIceClient::~DryIceClient()
	{
	}
}

