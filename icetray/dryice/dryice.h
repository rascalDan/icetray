#ifndef ICETRAY_TESTSETUP_H
#define ICETRAY_TESTSETUP_H

#include "icecube.h"
#include "icetrayService.h"
#include <Ice/Communicator.h>
#include <IceBox/IceBox.h>
#include <c++11Helpers.h>
#include <visibility.h>

namespace IceTray {
	class DLL_PUBLIC DryIce : private Cube {
	public:
		explicit DryIce(const Ice::StringSeq & = Ice::StringSeq());
		virtual ~DryIce();

		SPECIAL_MEMBERS_DEFAULT_MOVE_NO_COPY(DryIce);

		[[nodiscard]] std::string getEndpoint() const;

	protected:
		friend class DryIceClient;
		static DryIce * currentDryIce;

		void replace(const std::string &, const Ice::ObjectPtr &);

		template<typename T, typename I, typename... Args>
		static auto
		replace(Args &&... args)
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

		SPECIAL_MEMBERS_DEFAULT(DryIceClient);

		template<typename Prx>
		[[nodiscard]] std::shared_ptr<Prx>
		getProxy(const std::string & objectName) const
		{
			return Ice::checkedCast<Prx>(DryIce::currentDryIce->ic->stringToProxy(objectName));
		}

		[[nodiscard]] ServicePtr getService() const;
		[[nodiscard]] Ice::ObjectAdapterPtr getAdapter() const;

	private:
		Ice::ObjectAdapterPtr adapter;
	};
}

#endif
