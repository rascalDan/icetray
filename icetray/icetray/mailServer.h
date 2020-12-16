
#ifndef ICETRAY_MAILSERVER_H
#define ICETRAY_MAILSERVER_H

#include <mail.h>
#include <visibility.h>

namespace IceTray {
	namespace Mail {
		class DLL_PUBLIC BasicMailServer {
		public:
			static void writeEmailContent(const EmailPtr & msg, FILE * ms);
		};

		class DLL_PUBLIC LibesmtpMailServer : public MailServer, BasicMailServer {
		public:
			explicit LibesmtpMailServer(std::string server);

			void sendEmail(const EmailPtr & msg) override;

		private:
			const std::string server;
		};
	}
}

#endif
