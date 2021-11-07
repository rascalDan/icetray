#ifndef ICETRAY_ICECUBE_H
#define ICETRAY_ICECUBE_H

#include "icetrayService.h"
#include <Ice/Initialize.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/Proxy.h>
#include <Ice/ProxyF.h>
#include <memory>
#include <plugins.h> // IWYU pragma: keep
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
// IWYU pragma: no_include "plugins.impl.h"
namespace Ice {
	class Object;
}

namespace IceTray {
	class CubePlugIn : public AdHoc::AbstractPluginImplementation {
	};

	template<typename I> class CubePlugInOf : public CubePlugIn, public I {
		using I::I;
	};

	class CubeObject : public CubePlugIn {
	public:
		explicit inline CubeObject(Ice::ObjectPrxPtr o) : obj(std::move(o)) { }
		Ice::ObjectPrxPtr obj;
	};

	class Cube {
	protected:
		static inline auto
		pm()
		{
			return &Service::getCurrent()->servicePlugins;
		}

	public:
		template<typename T, typename I, typename... Args>
		static auto
		addObject(const Ice::ObjectAdapterPtr & adp, const std::string & name, Args &&... args)
		{
			static_assert(std::is_convertible<I *, T *>::value);
			static_assert(std::is_convertible<I *, Ice::Object *>::value);
			auto prx = adp->add(std::make_shared<I>(std::forward<Args>(args)...), Ice::stringToIdentity(name));
			return pm()->add<CubePlugIn>(
					std::make_shared<CubeObject>(prx), typeid(typename T::ProxyType).name(), __FILE__, __LINE__);
		}

		template<typename T, typename I, typename... Args>
		static auto
		add(Args &&... args)
		{
			static_assert(std::is_convertible<CubePlugInOf<I> *, T *>::value);
			return pm()->add<CubePlugIn>(std::make_shared<CubePlugInOf<I>>(std::forward<Args>(args)...),
					typeid(T).name(), __FILE__, __LINE__);
		}

		template<typename T>
		static auto
		get()
		{
			if constexpr (std::is_convertible<T *, Ice::Object *>::value) {
				using Prx = typename T::ProxyType;
				auto c = pm()->getImplementation<CubePlugIn>(typeid(Prx).name());
				return Ice::uncheckedCast<Prx>(std::dynamic_pointer_cast<CubeObject>(c)->obj);
			}
			else {
				auto c = pm()->getImplementation<CubePlugIn>(typeid(T).name());
				return std::dynamic_pointer_cast<T>(c);
			}
		}
	};
}

#endif
