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

class TestOptionsInline : public IceTray::Options {
	public:
		TestOptionsInline() : IceTray::Options("Test options inline")
		{
		}

		ICETRAY_OPTIONS_INLINE(
				("testIntInline", boost::program_options::value(&testInt), "testInt")
		);

		int testInt;
};
FACTORY(TestOptionsInline, IceTray::OptionsFactory);

