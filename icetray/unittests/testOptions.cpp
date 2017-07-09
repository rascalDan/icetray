#include "testOptions.h"

TestOptions::TestOptions() : IceTray::Options("Test options")
{
}

ICETRAY_OPTIONS(TestOptions,
		("testInt", boost::program_options::value(&testInt), "testInt")
		("testString", boost::program_options::value(&testString)->default_value("some string"), "testString")
		("vec", boost::program_options::value(&testVec), "vector")
);

FACTORY(TestOptions, IceTray::OptionsFactory);

