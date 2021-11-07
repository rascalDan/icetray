#include <Ice/BuiltinSequences.h>
#include <Ice/CommunicatorF.h>
#include <Ice/ObjectAdapterF.h>
#include <icetrayService.h>
#include <string>
#include <visibility.h>

class DLL_PUBLIC TestService : public IceTray::Service {
public:
	void addObjects(const std::string &, const Ice::CommunicatorPtr &, const Ice::StringSeq &,
			const Ice::ObjectAdapterPtr &) override;
};
