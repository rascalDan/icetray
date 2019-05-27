#ifndef ICETRAY_MOCKMAILSERVER_H
#define ICETRAY_MOCKMAILSERVER_H

#include <mockMail.h>
#include <mailServer.h>

namespace IceTray {
	namespace Mail {
		class DLL_PUBLIC MockMailServerImpl : public MockMailServer, BasicMailServer {
			public:
				void sendEmail(const EmailPtr & msg) override;
				Emails getSentEmails() override;

			private:
				Emails sentEmails;
		};
	}
}

#endif

