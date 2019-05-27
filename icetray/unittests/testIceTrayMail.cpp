#define BOOST_TEST_MODULE TestIceTrayMail
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <memstream.h>
#include <mailServer.h>
#include <mimeImpl.h>
#include <fileUtils.h>
#include <definedDirs.h>

using namespace std::string_literals;
using namespace IceTray::Mime;
using namespace IceTray::Mail;
using QPTD = std::tuple<std::string_view, std::string_view>;
using B64TD = std::tuple<size_t, std::string_view>;

BOOST_DATA_TEST_CASE(quotedPrintable, boost::unit_test::data::make<QPTD>({
	{ "", "" },
	{ "Simple string", "Simple string" },
	{ " \t Leading whitespace", " \t Leading whitespace" },
	{ "Trailing whitespace \t \n", "Trailing whitespace \t=20\r\n" },
	{ "High byte values £ © ±", "High byte values =C2=A3 =\r\n=C2=A9 =C2=B1" },
	{ "<html lang=\"en\">", "<html lang=3D\"en\">" }
}), input, expected)
{
	AdHoc::MemStream ms;
	TextPart::quotedPrintable(input, ms, 25);
	BOOST_CHECK_EQUAL(expected, ms.sv());
}

BOOST_DATA_TEST_CASE(base64, boost::unit_test::data::make<B64TD>({
	{ 0, "\r\n" },
	{ 1, "iQ==\r\n" },
	{ 2, "iVA=\r\n" },
	{ 3, "iVBO\r\n" },
	{ 4, "iVBORw==\r\n" },
	{ 5, "iVBORw0=\r\n" },
	{ 90,
		"iVBORw0KGgoAAAANSUhEUgAA\r\n"
		"AAsAAAALCAYAAACprHcmAAAA\r\n"
		"BmJLR0QA/wD/AP+gvaeTAAAA\r\n"
		"J0lEQVQYlWP8////fwbiwGom\r\n"
		"IhUyMDAwMIwqpo9iFgYGhtVE\r\n" },
	{ 113,
		"iVBORw0KGgoAAAANSUhEUgAA\r\n"
		"AAsAAAALCAYAAACprHcmAAAA\r\n"
		"BmJLR0QA/wD/AP+gvaeTAAAA\r\n"
		"J0lEQVQYlWP8////fwbiwGom\r\n"
		"IhUyMDAwMIwqpo9iFgYGhtVE\r\n"
		"qj0BAAvPBjJ63HJVAAAAAElF\r\n"
		"TkSuQmA=\r\n" },
}), input, expected)
{
	AdHoc::MemStream ms;
	AdHoc::FileUtils::MemMap png(rootDir / "fixtures" / "mail" / "blank.png");
	BinaryViewPart::base64(png.sv<uint8_t>().substr(0, input), ms, 24);
	BOOST_CHECK_EQUAL(expected, ms.sv());
}

struct TestBase {
	TestBase() :
		e(std::make_shared<Email>()),
#ifdef DUMP
		dump(fopen("/tmp/dump.eml", "w")),
#endif
		fixtures(rootDir / "fixtures" / "mail")
	{
		e->from = {"from", "from@test.com"};
		e->to = {"to", "to@test.com"};
		e->subject = "subject";

		auto commonHeaders = [](FILE * s) {
			fputs("From: from <from@test.com>\r\n", s);
			fputs("To: to <to@test.com>\r\n", s);
			fputs("Date: Fri, 24 May 2019 22:19:54 +0000 (UTC)\r\n", s);
			fputs("Return-path: <bounces@test.com>\r\n", s);
		};
		commonHeaders(ms);
#ifdef DUMP
		commonHeaders(dump);
#endif
	}
#ifdef DUMP
	~TestBase()
	{
		fclose(dump);
	}
#endif
	EmailPtr e;
	AdHoc::MemStream ms;
#ifdef DUMP
	FILE * dump;
#endif
	const std::filesystem::path fixtures;
};

const std::string text_content = "Simple text £\r\n";
const std::string html_content = "<html lang=\"en\">\r\n"
"<head>\r\n"
"<title>£</title>\r\n"
"</head>\r\n"
"<html>\r\n";

BOOST_FIXTURE_TEST_SUITE(base, TestBase);

BOOST_AUTO_TEST_CASE(single_part)
{
	e->content = std::make_shared<TextPart>(Headers {
		{ "X-Source", "single_part" }
	}, "text/plain", text_content);
	BasicMailServer::writeEmailContent(e, ms);
	BOOST_CHECK_EQUAL(ms, AdHoc::FileUtils::MemMap(fixtures / "simple.eml").sv());
}

BOOST_AUTO_TEST_CASE(multipart_alt)
{
	auto text = std::make_shared<TextPart>(Headers {
		{ "X-Source", "multipart_plain" }
	}, "text/plain", text_content);
	auto html = std::make_shared<TextPart>(Headers {
		{ "X-Source", "multipart_html" }
	}, "text/html", html_content);
	e->content = std::make_shared<MultiPart>(Headers {
		{ "X-Source", "multipart_top" }
	}, "alternative", Parts { text, html });
	BasicMailServer::writeEmailContent(e, ms);
	BOOST_CHECK_EQUAL(ms, AdHoc::FileUtils::MemMap(fixtures / "multipart-alt.eml").sv());
}

BOOST_AUTO_TEST_CASE(multipart_alt_imgs)
{
	AdHoc::FileUtils::MemMap png(fixtures / "blank.png");
	auto text = std::make_shared<TextPart>(Headers {
		{ "X-Source", "multipart_plain" }
	}, "text/plain", text_content);
	auto img1 = std::make_shared<BinaryViewPart>(Headers {
		{ "X-Source", "multipart_html_img1" }
	}, "image/png", png.sv<uint8_t>());
	auto img2 = std::make_shared<BinaryCopyPart>(Headers {
		{ "X-Source", "multipart_html_img2" }
	}, "image/png", std::vector<uint8_t>{ png.sv<uint8_t>().begin(), png.sv<uint8_t>().end() });
	auto html = std::make_shared<TextPart>(Headers {
		{ "X-Source", "multipart_html_main" }
	}, "text/html", html_content);
	auto htmlrel = std::make_shared<MultiPart>(Headers {
		{ "X-Source", "multipart_html" }
	}, "related", Parts { html, img1, img2 });
	e->content = std::make_shared<MultiPart>(Headers {
		{ "X-Source", "multipart_top" }
	}, "alternative", Parts { text, htmlrel });
	BasicMailServer::writeEmailContent(e, ms);
	BOOST_CHECK_EQUAL(ms, AdHoc::FileUtils::MemMap(fixtures / "multipart-alt-imgs.eml").sv());
}

BOOST_AUTO_TEST_SUITE_END();

