#include "mailServer.h"
#include "mail.h"
#include "mime.h"
#include <array>
#include <libesmtp.h>
#include <memory>
#include <memstream.h>
#include <ostream>
#include <utility>

namespace IceTray::Mail {
	LibesmtpMailServer::LibesmtpMailServer(std::string s) : server(std::move(s)) { }

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
			std::array<char, BUFSIZ> buf {};
			smtp_strerror(smtp_errno(), buf.data(), buf.size());
			smtp_destroy_session(session);
			throw SendEmailFailed(__FILE__, __LINE__, buf.data());
		}
		smtp_destroy_session(session);
	}

	void
	BasicMailServer::writeEmailContent(const EmailPtr & msg, FILE * ms)
	{
		fputs("MIME-Version: 1.0\r\n", ms);
		msg->content->write({ms}, 0);
	}

	void
	SendEmailFailed::ice_print(std::ostream & buf) const
	{
		buf << "Failed to send email: " << message;
	}
}
