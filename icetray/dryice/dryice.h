#ifndef ICETRAY_TESTSETUP_H
#define ICETRAY_TESTSETUP_H

#include <Ice/Communicator.h>
#include <IceBox/IceBox.h>
#include <visibility.h>
#include "icetrayService.h"
#include "icecube.h"

namespace IceTray {
	class DLL_PUBLIC DryIce : private Cube {
		public:
			DryIce(const Ice::StringSeq & = Ice::StringSeq());
			DryIce(const DryIce &) = delete;
			virtual ~DryIce();

			void operator=(const DryIce &) = delete;
			std::string getEndpoint() const;

		protected:
			friend class DryIceClient;
			static DryIce * currentDryIce;

			void replace(const std::string &, const Ice::ObjectPtr &);

			template<typename T, typename I, typename ... Args>
			static auto replace(Args && ... args)
			{
				pm()->remove<CubePlugIn>(typeid(T).name());
				return add<T, I>(std::forward<Args>(args)...);
			}


			Ice::CommunicatorPtr ic;
			IceTray::ServicePtr s;
	};

	class DLL_PUBLIC DryIceClient {
		public:
			DryIceClient();
			virtual ~DryIceClient();

			template<typename Prx>
			std::shared_ptr<Prx> getProxy(const std::string & objectName) const
			{
				return Ice::checkedCast<Prx>(DryIce::currentDryIce->ic->stringToProxy(objectName));
			}

			ServicePtr getService() const;
			Ice::ObjectAdapterPtr getAdapter() const;

		private:
			Ice::ObjectAdapterPtr adapter;
	};
}

#endif

