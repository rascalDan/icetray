#ifndef ICETRAY_MAIL
#define ICETRAY_MAIL

#include <mime.ice>

[["ice-prefix"]]
module IceTray {
	module Mail {
		local struct Address {
			string name;
			string address;
		};

		local class Email {
			Address to;
			Address from;
			string subject;
			Mime::BasicPart content;
		};

		["cpp:ice_print"]
		local exception SendEmailFailed {
			string message;
		};

		local interface MailServer {
			idempotent void sendEmail(Email msg) throws SendEmailFailed;
		};
	};
};

#endif

