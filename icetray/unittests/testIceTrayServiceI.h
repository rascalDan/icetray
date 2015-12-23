#ifndef TEST_ICETRAY_SERVICE_IMPL_H
#define TEST_ICETRAY_SERVICE_IMPL_H

#include <testIceTrayService.h>
#include <icetrayService.h>
#include <abstractDatabaseClient.h>

namespace TestIceTray {
	class TestIceTrayServiceI : IceTray::AbstractDatabaseClient, public TestIceTrayService {
		public:
			TestIceTrayServiceI(boost::shared_ptr<AdHoc::ResourcePool<DB::Connection>> db);

			void method(const Ice::Current &) override;
	};

	class TestService : public IceTray::Service {
		public:
			void addObjects(const std::string &, const Ice::CommunicatorPtr &, const Ice::StringSeq &, const Ice::ObjectAdapterPtr &) override;
	};
}

#endif

