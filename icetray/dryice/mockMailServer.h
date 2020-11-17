#ifndef ICETRAY_MOCKMAILSERVER_H
#define ICETRAY_MOCKMAILSERVER_H

#include <mailServer.h>
#include <mockMail.h>

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
