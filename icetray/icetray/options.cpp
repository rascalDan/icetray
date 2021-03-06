#include "options.h"
#include <boost/program_options/errors.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <factory.impl.h>
#include <set>
#include <utility>
#include <vector>
// IWYU pragma: no_include <boost/smart_ptr/shared_ptr.hpp>

INSTANTIATEVOIDFACTORY(IceTray::Options)
INSTANTIATEPLUGINOF(IceTray::Options)

namespace po = boost::program_options;

namespace IceTray {
	Options::Options(std::string name) : optionsName(std::move(name)) { }

	OptionsDescPtr
	Options::getOptions()
	{
		auto opts = std::make_shared<boost::program_options::options_description>(optionsName);
		options(opts->add_options());
		return opts;
	}

	OptionsCollation::OptionsCollation()
	{
		auto defManager = AdHoc::PluginManager::getDefault();
		for (const auto & f : defManager->getAll<OptionsFactory>()) {
			auto o = f->implementation()->create();
			auto & inst = *o;
			defManager->add<Options>(o, typeid(inst).name(), __FILE__, __LINE__);
			all.add(*o->getOptions());
		}
	}

	OptionsCollation::~OptionsCollation()
	{
		auto defManager = AdHoc::PluginManager::getDefault();
		for (const auto & o : defManager->getAll<Options>()) {
			defManager->remove<Options>(o->name);
		}
	}

	void
	OptionsCollation::apply(const Ice::PropertiesPtr & p)
	{
		po::parsed_options result(&all);

		for (const auto & optDesc : all.options()) {
			po::option opt;
			opt.value = p->getPropertyAsList(optDesc->long_name());
			if (opt.value.empty()) {
				// If it was processed as an empty list and was empty, it wasn't there.
				if (p->getProperty(optDesc->long_name()).empty()) {
					continue;
				}
				// If it was process as an empty list but wasn't empty, treat as the empty string.
				opt.value = {""};
			}
			opt.string_key = optDesc->long_name();
			opt.unregistered = false;
			result.options.push_back(opt);
		}

		po::variables_map vm;
		po::store(result, vm);
		po::notify(vm);
	}

	std::ostream &
	OptionsCollation::write(std::ostream & s) const
	{
		s << all;
		return s;
	}
}
