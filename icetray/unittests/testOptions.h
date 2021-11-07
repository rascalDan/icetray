#ifndef ICETRAY_TEST_TESTOPTIONS_H
#define ICETRAY_TEST_TESTOPTIONS_H

#include <options.h>
#include <string>
#include <vector>
#include <visibility.h>

class DLL_PUBLIC TestOptions : public IceTray::Options {
public:
	TestOptions();

	ICETRAY_OPTIONS_DECLARE

	int testInt;
	std::string testString;
	std::vector<int> testVec;
};

#endif
