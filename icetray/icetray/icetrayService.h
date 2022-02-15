#ifndef ICETRAY_SERVICE_H
#define ICETRAY_SERVICE_H

#include "logWriter.h"
#include "options.h"
#include <Ice/BuiltinSequences.h>
#include <Ice/CommunicatorF.h>
#include <Ice/ObjectAdapterF.h>
#include <Ice/PropertiesF.h>
#include <IceBox/IceBox.h>
#include <c++11Helpers.h>
#include <connectionPool.h>
#include <factory.h> // IWYU pragma: keep
#include <memory>
#include <set>
#include <string>
#include <visibility.h>
// IWYU pragma: no_include "factory.impl.h"

namespace IceTray {
	class DLL_PUBLIC Service : public IceBox::Service, public AdHoc::AbstractPluginImplementation {
	public:
		Service();
		~Service() override;

		SPECIAL_MEMBERS_DELETE(Service);

		virtual void addObjects(const std::string & name, const Ice::CommunicatorPtr & ic, const Ice::StringSeq &,
				const Ice::ObjectAdapterPtr &)
				= 0;

		void start(const std::string & name, const Ice::CommunicatorPtr & ic, const Ice::StringSeq & args) override;

		void stop() override;

		DB::ConnectionPoolPtr getConnectionPool(
				const Ice::CommunicatorPtr & ic, const std::string & type, const std::string & prefix);

		[[nodiscard]] static Service * getCurrent();
		[[nodiscard]] static Service * create(const Ice::CommunicatorPtr &);

	private:
		void configureLoggers(const Ice::ObjectAdapterPtr &, const Ice::PropertiesPtr &);
		void shutdownLoggers();

		friend class DryIce;
		friend class Cube;
		Ice::ObjectAdapterPtr adp;
		static Service * current;
		std::set<Logging::LogWriterPrxPtr> logWriters;
		OptionsCollation optionsCollation;
		AdHoc::PluginManager servicePlugins;
	};

	using ServicePtr = std::shared_ptr<Service>;

	// Custom factory required because IceBox interface requires a naked pointer.
	// See: https://doc.zeroc.com/display/Ice37/Developing+IceBox+Services#DevelopingIceBoxServices-C++ServiceEntryPoint
	class ServiceFactory : public AdHoc::AbstractPluginImplementation {
	public:
		[[nodiscard]] virtual Service * create() const = 0;

		template<typename Impl> class For;
	};

	template<typename Impl> class ServiceFactory::For : public ServiceFactory {
	public:
		[[nodiscard]] Service *
		create() const override
		{
			return new Impl();
		}
	};

	using PoolProvider = AdHoc::Factory<DB::BasicConnectionPool, const std::string &, const std::string &,
			const Ice::PropertiesPtr &>;
}

#endif
