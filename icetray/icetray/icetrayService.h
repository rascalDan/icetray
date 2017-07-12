#ifndef ICETRAY_SERVICE_H
#define ICETRAY_SERVICE_H

#include <IceBox/IceBox.h>
#include <factory.h>
#include <visibility.h>
#include "database.h"
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

			DatabasePoolPtr getConnectionPool(const Ice::CommunicatorPtr & ic, const std::string & type, const std::string & prefix);

			static Service * getCurrent();
			static Service * create(Ice::CommunicatorPtr);

		private:
			void configureLoggers(const Ice::ObjectAdapterPtr &, const Ice::PropertiesPtr &);
			void shutdownLoggers();

			friend class DryIce;
			Ice::ObjectAdapterPtr adp;
			static Service * current;
			std::set<Logging::LogWriterPrx> logWriters;
			OptionsCollation optionsCollation;
	};

	typedef IceInternal::Handle<Service> ServicePtr;
	typedef AdHoc::Factory<Service> ServiceFactory;
	typedef AdHoc::Factory<DatabasePool, const std::string &, const std::string &, Ice::PropertiesPtr> PoolProvider;
}

#endif

