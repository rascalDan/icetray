#include <icetrayService.h>

class DLL_PUBLIC TestService : public IceTray::Service {
	public:
		virtual ~TestService();

		void addObjects(const std::string &, const Ice::CommunicatorPtr &, const Ice::StringSeq &, const Ice::ObjectAdapterPtr &) override;
};

