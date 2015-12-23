#include <Ice/Ice.h>
#include <visibility.h>
#include <factory.impl.h>
#include "icetrayService.h"

namespace IceTray {
	void Service::start(const std::string & name, const Ice::CommunicatorPtr & ic, const Ice::StringSeq & args)
	{
		adp = ic->createObjectAdapter(name);
		addObjects(name, ic, args, adp);
		adp->activate();
	}

	void Service::stop()
	{
		adp->deactivate();
		adp->destroy();
	}

	boost::shared_ptr<AdHoc::ResourcePool<DB::Connection>> Service::getConnectionPool(const Ice::CommunicatorPtr & ic, const std::string & type, const std::string & name)
	{
		auto p = ic->getProperties();
		return boost::shared_ptr<AdHoc::ResourcePool<DB::Connection>>(PoolProvider::createNew(
					p->getPropertyWithDefault("DryIce.PoolProvider", "DefaultPool"),
					name, type, p));
	}
}

extern "C" {
	DLL_PUBLIC
	IceBox::Service *
	createIceTrayService(Ice::CommunicatorPtr)
	{
		return IceTray::ServiceFactory::createNew("default");
	}
}

INSTANTIATEVOIDFACTORY(IceTray::Service);
INSTANTIATEFACTORY(IceTray::PoolType, const std::string &, const std::string &, Ice::PropertiesPtr);

