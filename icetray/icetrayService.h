#ifndef ICETRAY_SERVICE_H
#define ICETRAY_SERVICE_H

#include <IceBox/IceBox.h>
#include <factory.h>
#include <visibility.h>
#include <connectionPool.h>

namespace IceTray {
	typedef AdHoc::ResourcePool<DB::Connection> PoolType;
	typedef boost::shared_ptr<PoolType> PoolTypePtr;

	class DLL_PUBLIC Service : public IceBox::Service, public AdHoc::AbstractPluginImplementation {
		public:
			typedef boost::shared_ptr<DB::ConnectionPool> DBCPoolPtr;

			virtual void addObjects(const std::string & name, const Ice::CommunicatorPtr & ic, const Ice::StringSeq &, const Ice::ObjectAdapterPtr &) = 0;

			void start(const std::string & name, const Ice::CommunicatorPtr & ic, const Ice::StringSeq & args) override;

			void stop() override;

			PoolTypePtr getConnectionPool(const Ice::CommunicatorPtr & ic, const std::string & type, const std::string & prefix);

			Ice::ObjectAdapterPtr adp;
	};

	typedef AdHoc::Factory<Service> ServiceFactory;
	typedef AdHoc::Factory<PoolType, const std::string &, const std::string &, Ice::PropertiesPtr> PoolProvider;
}

#endif

