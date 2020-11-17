#include "mockMailServer.h"

namespace IceTray::Mail {
	void
	MockMailServerImpl::sendEmail(const EmailPtr & e)
	{
		sentEmails.push_back(e);
	}

	Emails
	MockMailServerImpl::getSentEmails()
	{
		return sentEmails;
	}
}
