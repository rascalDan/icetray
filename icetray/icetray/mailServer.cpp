#include "mailServer.h"
#include <libesmtp.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/Communicator.h>
#include <memstream.h>

namespace IceTray {
	namespace Mail {
		LibesmtpMailServer::LibesmtpMailServer(std::string s) :
			server(std::move(s))
		{
		}

		void
		LibesmtpMailServer::sendEmail(const EmailPtr & msg)
		{
			smtp_session_t session = smtp_create_session();
			smtp_message_t message = smtp_add_message(session);
			smtp_set_server(session, server.c_str());
			smtp_set_header(message, "To", msg->to.name.c_str(), msg->to.address.c_str());
			smtp_set_header(message, "From", msg->from.name.c_str(), msg->from.address.c_str());
			smtp_set_header(message, "Subject", msg->subject.c_str());
			smtp_add_recipient(message, msg->to.address.c_str());
			AdHoc::MemStream ms;
			writeEmailContent(msg, ms);
			smtp_set_message_fp(message, ms);
			if (!smtp_start_session(session)) {
				char buf[BUFSIZ];
				auto b = smtp_strerror(smtp_errno(), buf, sizeof(buf));
				assert(b);
				SendEmailFailed e(__FILE__, __LINE__, b);
				smtp_destroy_session(session);
				throw e;
			}
			smtp_destroy_session(session);
		}

		void
		BasicMailServer::writeEmailContent(EmailPtr msg, FILE * ms)
		{
			fputs("MIME-Version: 1.0\r\n", ms);
			msg->content->write({ ms }, 0);
		}


		void
		SendEmailFailed::ice_print(std::ostream & buf) const
		{
			buf << "Failed to send email: " << message;
		}
	}
}

