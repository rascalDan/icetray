#ifndef TEST_ICETRAY_SERVICE_IMPL_H
#define TEST_ICETRAY_SERVICE_IMPL_H

#include <Ice/BuiltinSequences.h>
#include <Ice/CommunicatorF.h>
#include <Ice/Config.h>
#include <Ice/ObjectAdapterF.h>
#include <abstractCachingDatabaseClient.h>
#include <connectionPool.h>
#include <icetrayService.h>
#include <string>
#include <testIceTrayService.h>
namespace Ice {
	struct Current;
}

namespace TestIceTray {
	class TestIceTrayServiceI : IceTray::AbstractCachingDatabaseClient, public TestIceTrayService {
	public:
		TestIceTrayServiceI(const DB::ConnectionPoolPtr & db);

		void method1(const Ice::Current &) override;
		void method2(Ice::Int id, std::string name, const Ice::Current &) override;
		Ice::Int method3(const Ice::Current &) override;

	private:
		template<typename Type> void fetchTest(const Type & value);
	};

	class TestService : public IceTray::Service {
	public:
		void addObjects(const std::string &, const Ice::CommunicatorPtr &, const Ice::StringSeq &,
				const Ice::ObjectAdapterPtr &) override;
	};

	class TestCubeI : public TestCube {
	public:
		void method1() override;
		void method2(Ice::Int id, const std::string & name) override;
	};
}

#endif
