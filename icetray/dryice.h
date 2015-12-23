#ifndef ICETRAY_TESTSETUP_H
#define ICETRAY_TESTSETUP_H

#include <Ice/Communicator.h>
#include <IceBox/IceBox.h>
#include <visibility.h>

namespace IceTray {
	class DLL_PUBLIC DryIce {
		public:
			DryIce(const Ice::StringSeq & = Ice::StringSeq());
			DryIce(const DryIce &) = delete;
			virtual ~DryIce();

			void operator=(const DryIce &) = delete;

		protected:
			Ice::CommunicatorPtr ic;
			IceBox::ServicePtr s;
	};

	class DLL_PUBLIC DryIceClient {
		public:
			DryIceClient();
			virtual ~DryIceClient();

			Ice::CommunicatorPtr ic;
	};
}

#endif

