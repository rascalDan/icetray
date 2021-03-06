#include "testOptions.h"
#include <boost/lexical_cast/bad_lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <factory.h>
#include <options.h>

TestOptions::TestOptions() : IceTray::Options("Test options"), testInt(0) { }

ICETRAY_OPTIONS(TestOptions,
		("testInt", boost::program_options::value(&testInt), "testInt")("testString",
				boost::program_options::value(&testString)->default_value("some string"),
				"testString")("vec", boost::program_options::value(&testVec), "vector"))

class TestOptionsInline : public IceTray::Options {
public:
	TestOptionsInline() : IceTray::Options("Test options inline") { }

	ICETRAY_OPTIONS_INLINE(("testIntInline", boost::program_options::value(&testInt), "testInt"));

	int testInt {0};
};
FACTORY(TestOptionsInline, IceTray::OptionsFactory)
