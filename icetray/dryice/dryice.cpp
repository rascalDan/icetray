#include "dryice.h"
#include <boost/assert.hpp>
#include <dlfcn.h>
#include <factory.h>
#include <Ice/Initialize.h>

namespace IceTray {
	typedef IceBox::Service *(* SetupFunction)(Ice::CommunicatorPtr);

	DryIce::DryIce(const Ice::StringSeq & cmdline)
	{
		void * i = dlsym(NULL, "createIceTrayService");
		BOOST_VERIFY(i);
		auto sf = (SetupFunction)i;
		BOOST_VERIFY(sf);
		Ice::StringSeq args;
		Ice::InitializationData id;
		id.properties = Ice::createProperties();
		id.properties->setProperty("DryIce.Endpoints", "tcp -p 9002");
		id.properties->setProperty("DryIce.PoolProvider", "MockPool");
		id.properties->parseCommandLineOptions("", cmdline);
		ic = Ice::initialize(args, id);
		s = sf(nullptr);
		s->start("DryIce", ic, {});
	}

	DryIce::~DryIce()
	{
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
		ic = Ice::initialize();
	}

	DryIceClient::~DryIceClient()
	{
		ic->destroy();
	}
}

