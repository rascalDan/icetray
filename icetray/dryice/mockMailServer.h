#ifndef ICETRAY_MOCKMAILSERVER_H
#define ICETRAY_MOCKMAILSERVER_H

#include <mail.h>
#include <mailServer.h>
#include <mockMail.h>
#include <visibility.h>

namespace IceTray {
	namespace Mail {
		class DLL_PUBLIC MockMailServerImpl : public MockMailServer, BasicMailServer {
		public:
			void sendEmail(const EmailPtr & msg) override;
			Emails getSentEmails() override;
			void clear() override;

		private:
			Emails sentEmails;
		};
	}
}

#endif
