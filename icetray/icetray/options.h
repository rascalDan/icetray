#ifndef ICETRAY_OPTIONS_H
#define ICETRAY_OPTIONS_H

#include <factory.h>
#include <visibility.h>
#include <boost/program_options.hpp>
#include <Ice/Properties.h>

namespace IceTray {
	typedef boost::shared_ptr<boost::program_options::options_description> OptionsDescPtr;
	class DLL_PUBLIC Options : public AdHoc::AbstractPluginImplementation {
		public:
			Options(const std::string & name);


		private:
			friend class OptionsCollation;
			virtual void options(boost::program_options::options_description_easy_init && optDesc) = 0;
			OptionsDescPtr getOptions();
			const std::string optionsName;
	};
	typedef AdHoc::Factory<Options> OptionsFactory;
	typedef AdHoc::PluginOf<Options> OptionsPlugin;

	template <typename T>
	class DLL_PUBLIC OptionsResolver {
		public:
			OptionsResolver() :
				opts(dynamic_cast<T&>(*AdHoc::PluginManager::getDefault()->get<Options>(typeid(T).name())->implementation()))
			{
			}

			const T * operator->() const { return &opts; }

		private:
			const T & opts;
	};

	class DLL_PUBLIC OptionsCollation {
		public:
			OptionsCollation();
			~OptionsCollation();

			void apply(Ice::PropertiesPtr p);
			std::ostream & write(std::ostream &) const;

		private:
			boost::program_options::options_description all;
	};
}
#define ICETRAY_OPTIONS_FUNC \
	options(boost::program_options::options_description_easy_init && optDesc)
#define ICETRAY_OPTIONS_BODY(OptsDef) \
	{ optDesc OptsDef ; }
#define ICETRAY_OPTIONS_DECLARE \
	void ICETRAY_OPTIONS_FUNC;
#define ICETRAY_OPTIONS_INLINE(OptsDef) \
	void ICETRAY_OPTIONS_FUNC ICETRAY_OPTIONS_BODY(OptsDef)
#define ICETRAY_OPTIONS(Class, OptsDef) \
	void Class::ICETRAY_OPTIONS_FUNC ICETRAY_OPTIONS_BODY(OptsDef)

#endif

