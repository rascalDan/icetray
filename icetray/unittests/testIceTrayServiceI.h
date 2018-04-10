#ifndef TEST_ICETRAY_SERVICE_IMPL_H
#define TEST_ICETRAY_SERVICE_IMPL_H

#include <testIceTrayService.h>
#include <icetrayService.h>
#include <abstractCachingDatabaseClient.h>

namespace TestIceTray {
	class TestIceTrayServiceI : IceTray::AbstractCachingDatabaseClient, public TestIceTrayService {
		public:
			TestIceTrayServiceI(const IceTray::DatabasePoolPtr & db);

			void method1(const Ice::Current &) override;
			void method2(Ice::Int id, std::string name, const Ice::Current &) override;

		private:
			template<typename Type> void fetchTest(const Type & value);
	};

	class TestService : public IceTray::Service {
		public:
			void addObjects(const std::string &, const Ice::CommunicatorPtr &, const Ice::StringSeq &, const Ice::ObjectAdapterPtr &) override;
	};
}

#endif

