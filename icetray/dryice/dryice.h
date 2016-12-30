#ifndef ICETRAY_TESTSETUP_H
#define ICETRAY_TESTSETUP_H

#include <Ice/Communicator.h>
#include <IceBox/IceBox.h>
#include <visibility.h>
#include "icetrayService.h"

namespace IceTray {
	class DLL_PUBLIC DryIce {
		public:
			DryIce(const Ice::StringSeq & = Ice::StringSeq());
			DryIce(const DryIce &) = delete;
			virtual ~DryIce();

			void operator=(const DryIce &) = delete;
			std::string getEndpoint() const;

		protected:
			friend class DryIceClient;
			static DryIce * currentDryIce;

			void replace(const std::string &, Ice::ObjectPtr);

			Ice::CommunicatorPtr ic;
			IceTray::ServicePtr s;
	};

	class DLL_PUBLIC DryIceClient {
		public:
			DryIceClient();
			virtual ~DryIceClient();

			template<typename Prx>
			Prx getProxy(const std::string & objectName) const
			{
				return Prx::checkedCast(DryIce::currentDryIce->ic->stringToProxy(objectName));
			}
	};
}

#endif

