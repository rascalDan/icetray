#ifndef ICETRAY_SERVICE_H
#define ICETRAY_SERVICE_H

#include <IceBox/IceBox.h>
#include <factory.h>
#include <visibility.h>
#include <connectionPool.h>
#include "logger.h"
#include "options.h"

namespace IceTray {
	class DLL_PUBLIC Service : public IceBox::Service, public AdHoc::AbstractPluginImplementation {
		public:
			Service();
			virtual ~Service();

			virtual void addObjects(const std::string & name, const Ice::CommunicatorPtr & ic, const Ice::StringSeq &, const Ice::ObjectAdapterPtr &) = 0;

			void start(const std::string & name, const Ice::CommunicatorPtr & ic, const Ice::StringSeq & args) override;

			void stop() override;

			DB::ConnectionPoolPtr getConnectionPool(const Ice::CommunicatorPtr & ic, const std::string & type, const std::string & prefix);

			static Service * getCurrent();
			static Service * create(const Ice::CommunicatorPtr &);

		private:
			void configureLoggers(const Ice::ObjectAdapterPtr &, const Ice::PropertiesPtr &);
			void shutdownLoggers();

			friend class DryIce;
			Ice::ObjectAdapterPtr adp;
			static Service * current;
			std::set<Logging::LogWriterPrxPtr> logWriters;
			OptionsCollation optionsCollation;
	};

	typedef std::shared_ptr<Service> ServicePtr;

	// Custom factory required because IceBox interface requires a naked pointer.
	// See: https://doc.zeroc.com/display/Ice37/Developing+IceBox+Services#DevelopingIceBoxServices-C++ServiceEntryPoint
	class ServiceFactory : public AdHoc::AbstractPluginImplementation {
		public:
			virtual Service * create() const = 0;

			template<typename Impl>
			class For;
	};

	template<typename Impl>
	class ServiceFactory::For : public ServiceFactory {
		public:
			Service * create() const
			{
				return new Impl();
			}
	};

	typedef AdHoc::Factory<DB::BasicConnectionPool, const std::string &, const std::string &, const Ice::PropertiesPtr &> PoolProvider;
}

#endif

