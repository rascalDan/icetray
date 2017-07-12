#include <Ice/Ice.h>
#include <visibility.h>
#include <factory.impl.h>
#include "icetrayService.h"
#include <boost/assert.hpp>

namespace IceTray {
	Service * Service::current = nullptr;

	Service::Service()
	{
		BOOST_ASSERT(!current);
		current = this;
	}

	Service::~Service()
	{
		BOOST_ASSERT(current);
		current = nullptr;
	}

	Service *
	Service::getCurrent()
	{
		BOOST_ASSERT(current);
		return current;
	}

	Service *
	Service::create(Ice::CommunicatorPtr)
	{
		return IceTray::ServiceFactory::createNew("default");
	}

	void Service::start(const std::string & name, const Ice::CommunicatorPtr & ic, const Ice::StringSeq & args)
	{
		adp = ic->createObjectAdapter(name);
		configureLoggers(adp, ic->getProperties());
		optionsCollation.apply(ic->getProperties());
		addObjects(name, ic, args, adp);
		adp->activate();
	}

	void
	Service::configureLoggers(const Ice::ObjectAdapterPtr & adp, const Ice::PropertiesPtr & p)
	{
		auto logManager = LOGMANAGER();
		for (auto logWriterFactory : AdHoc::PluginManager::getDefault()->getAll<Logging::LogWriterFactory>()) {
			auto logWriter = logWriterFactory->implementation()->create(p.get());
			if (logWriter->lowestLevel()) {
				auto prx = Logging::LogWriterPrx::uncheckedCast(adp->addWithUUID(logWriter));
				logWriters.insert(prx);
				logManager->addWriter(prx);
			}
			else {
				delete logWriter;
			}
		}
	}

	void
	Service::shutdownLoggers()
	{
		auto logManager = LOGMANAGER();
		for (auto prx : logWriters) {
			logManager->removeWriter(prx);
		}
	}

	void Service::stop()
	{
		shutdownLoggers();
		adp->deactivate();
		adp->destroy();
	}

	DatabasePoolPtr Service::getConnectionPool(const Ice::CommunicatorPtr & ic, const std::string & type, const std::string & name)
	{
		auto p = ic->getProperties();
		return DatabasePoolPtr(PoolProvider::createNew(
					p->getPropertyWithDefault("DryIce.PoolProvider", "DefaultPool"),
					name, type, p));
	}
}

extern "C" {
	DLL_PUBLIC
	IceBox::Service *
	createIceTrayService(Ice::CommunicatorPtr ic)
	{
		return IceTray::Service::create(ic);
	}
}

INSTANTIATEVOIDFACTORY(IceTray::Service);
INSTANTIATEFACTORY(IceTray::DatabasePool, const std::string &, const std::string &, Ice::PropertiesPtr);

