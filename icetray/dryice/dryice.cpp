#include "dryice.h"
#include <boost/assert.hpp>
#include <factory.h>
#include <Ice/Initialize.h>
#include <Ice/ObjectAdapter.h>

namespace IceTray {
	DryIce * DryIce::currentDryIce = nullptr;

	DryIce::DryIce(const Ice::StringSeq & cmdline)
	{
		BOOST_ASSERT(!currentDryIce);
		Ice::StringSeq args;
		Ice::InitializationData id;
		id.properties = Ice::createProperties();
		id.properties->setProperty("DryIceClient.Endpoints", "default");
		id.properties->setProperty("DryIce.Endpoints", "default");
		id.properties->setProperty("DryIce.PoolProvider", "MockPool");
		id.properties->parseCommandLineOptions("", cmdline);
		ic = Ice::initialize(args, id);
		s = Service::create(ic);
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

	void
	DryIce::replace(const std::string & name, Ice::ObjectPtr replacement)
	{
		auto id = ic->stringToIdentity(name);
		s->adp->remove(id);
		s->adp->add(replacement, id);
	}

	DryIceClient::DryIceClient() :
		adapter(DryIce::currentDryIce->ic->createObjectAdapter("DryIceClient"))
	{
		adapter->activate();
	}

	DryIceClient::~DryIceClient()
	{
		adapter->deactivate();
		adapter->destroy();
	}

	ServicePtr
	DryIceClient::getService() const
	{
		BOOST_ASSERT(DryIce::currentDryIce);
		BOOST_ASSERT(DryIce::currentDryIce->s);
		return DryIce::currentDryIce->s;
	}

	Ice::ObjectAdapterPtr
	DryIceClient::getAdapter() const
	{
		return adapter;
	}
}

