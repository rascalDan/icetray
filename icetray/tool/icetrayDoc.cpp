#include <boost/lexical_cast/bad_lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include <compileTimeFormatter.h>
#include <dlfcn.h>
#include <iostream>
#include <options.h>
#include <string>
#include <vector>

namespace po = boost::program_options;

AdHocFormatter(dlopenFail, "Failed to dlopen(%?): %?\n");

void
write()
{
	IceTray::OptionsCollation oc;
	oc.write(std::cout);
}

int
main(int argc, char ** argv)
{
	po::options_description opts("IceTray library documentation");
	std::vector<std::string> libs;

	// clang-format off
	opts.add_options()
		("help,h", "Show this help message")
		("lib", po::value(&libs)->required(), "Library filename (see dlopen(3))")
		;
	// clang-format on
	po::positional_options_description p;
	p.add("lib", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(opts).positional(p).run(), vm);

	if (vm.count("help")) {
		std::cout << opts << std::endl;
		return 1;
	}
	po::notify(vm);

	std::vector<void *> dls;
	for (const auto & lib : libs) {
		auto dl = dlopen(lib.c_str(), RTLD_NOW);
		if (!dl) {
			dlopenFail::write(std::cerr, lib, dlerror());
			return 1;
		}
		dls.push_back(dl);
	}

	write();

	for (const auto & dl : dls) {
		dlclose(dl);
	}

	return 0;
}
