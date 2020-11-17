#include "icetrayService.h"
#include <Ice/Ice.h>
#include <boost/assert.hpp>
#include <factory.impl.h>
#include <visibility.h>

namespace IceTray {
	Service * Service::current = nullptr;

	Service::Service()
	{
		// NOLINTNEXTLINE(hicpp-no-array-decay)
		BOOST_ASSERT(!current);
		current = this;
	}

	Service::~Service()
	{
		// NOLINTNEXTLINE(hicpp-no-array-decay)
		BOOST_ASSERT(current);
		current = nullptr;
	}

	Service *
	Service::getCurrent()
	{
		// NOLINTNEXTLINE(hicpp-no-array-decay)
		BOOST_ASSERT(current);
		return current;
	}

	Service *
	Service::create(const Ice::CommunicatorPtr &)
	{
		auto serviceFactories = AdHoc::PluginManager::getDefault()->getAll<IceTray::ServiceFactory>();
		// NOLINTNEXTLINE(hicpp-no-array-decay)
		BOOST_ASSERT(serviceFactories.size() == 1);
		return serviceFactories.begin()->get()->implementation()->create();
	}

	void
	Service::start(const std::string & name, const Ice::CommunicatorPtr & ic, const Ice::StringSeq & args)
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
		for (const auto & logWriterFactory : AdHoc::PluginManager::getDefault()->getAll<Logging::LogWriterFactory>()) {
			auto logWriter = logWriterFactory->implementation()->create(p);
			if (logWriter->lowestLevel({})) {
				auto prx = Ice::uncheckedCast<Logging::LogWriterPrx>(adp->addWithUUID(logWriter));
				logWriters.insert(prx);
				logManager->addWriter(prx);
			}
		}
	}

	void
	Service::shutdownLoggers()
	{
		auto logManager = LOGMANAGER();
		for (const auto & prx : logWriters) {
			logManager->removeWriter(prx);
		}
	}

	void
	Service::stop()
	{
		shutdownLoggers();
		adp->deactivate();
		adp->destroy();
	}

	DB::ConnectionPoolPtr
	Service::getConnectionPool(const Ice::CommunicatorPtr & ic, const std::string & type, const std::string & name)
	{
		auto p = ic->getProperties();
		return PoolProvider::createNew(p->getPropertyWithDefault("DryIce.PoolProvider", "DefaultPool"), name, type, p);
	}
}

extern "C" {
DLL_PUBLIC
IceBox::Service *
// Public ICE Box API signature
// NOLINTNEXTLINE(performance-unnecessary-value-param)
createIceTrayService(Ice::CommunicatorPtr ic)
{
	return IceTray::Service::create(ic);
}
}

INSTANTIATEPLUGINOF(IceTray::ServiceFactory);
INSTANTIATEFACTORY(DB::BasicConnectionPool, const std::string &, const std::string &, const Ice::PropertiesPtr &);
