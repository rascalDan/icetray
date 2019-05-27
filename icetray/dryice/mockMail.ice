#ifndef ICETRAY_MOCKMAIL
#define ICETRAY_MOCKMAIL

#include <mail.ice>

[["ice-prefix"]]
module IceTray {
	module Mail {
		local sequence<Email> Emails;
		local interface MockMailServer extends MailServer {
			Emails getSentEmails();
		};
	};
};

#endif

