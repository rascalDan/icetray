#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/detail/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/lexical_cast/bad_lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/type_index/type_index_facade.hpp>
#include <chrono>
#include <compileTimeFormatter.h>
#include <filesystem>
#include <fstream> // IWYU pragma: keep
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace po = boost::program_options;
namespace fs = std::filesystem;

AdHocFormatter(CPPHeader,
		R"C(#include "%?.h"
#include <staticSqlSource.h>
#include <string>

const IceTray::StaticSqlSource )C");
AdHocFormatter(CPPNS, "%?::");
AdHocFormatter(CPPOpen, R"C(%?
__attribute__((init_priority(209))) (R"SQL()C");
AdHocFormatter(CPPConnectorOpts, R"C(, "%?", {%?})C");
AdHocFormatter(CPPConnectorOpt, R"C({ "%?", "%?" },)C");
AdHocFormatter(CPPConnectorOptFlag, R"C({ "%?", "" },)C");
AdHocFormatter(CPPClose, R"C()SQL")C");
AdHocFormatter(CPPFooter, R"C();

)C");
AdHocFormatter(HHeader,
		R"H(#include <staticSqlSource.h>

)H");
AdHocFormatter(HDeclartion, "extern const IceTray::StaticSqlSource %?;\n");
AdHocFormatter(HFooter, "\n");

AdHocFormatter(OpenNamespace, "namespace %? {\n");
AdHocFormatter(CloseNamespace, "} // %?\n");

int
// NOLINTNEXTLINE(bugprone-exception-escape)
main(int argc, char ** argv)
{
	po::options_description opts("IceTray SQL-to-C++ precompiler");
	fs::path sql, cpp, h, base;
	std::string sqlns, connector;

	// clang-format off
	opts.add_options()
		("help,h", "Show this help message")
		("sql", po::value(&sql)->required(), "Path of SQL script")
		("cpp", po::value(&cpp)->required(), "Path of C++ file to write")
		("h", po::value(&h)->required(), "Path of header file to write")
		// NOLINTNEXTLINE(clang-analyzer-optin.cplusplus.VirtualCall)
		("basedir,d", po::value(&base)->default_value(fs::current_path()),
		 "Base directory of SQL scripts (namespaces are created relative to here)")
		("namespace", po::value(&sqlns), "Namespace to create SqlSource in")
		("connector,c", po::value(&connector), "Specifiy a default connector name")
		;
	// clang-format on

	po::positional_options_description p;
	p.add("sql", 1);
	p.add("cpp", 1);
	p.add("h", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(opts).positional(p).run(), vm);

	// NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
	if (vm.count("help")) {
		std::cout << opts << std::endl;
		return 1;
	}
	po::notify(vm);

	std::vector<std::string> sqlnsparts;
	// NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
	boost::algorithm::split(sqlnsparts, sqlns, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
	auto r = fs::canonical(sql).lexically_relative(fs::canonical(base)).parent_path();
	if (!r.empty()) {
		std::copy(r.begin(), r.end(), std::back_inserter(sqlnsparts));
	}

	std::ifstream sqlin(sql);
	std::ofstream cppout(cpp);
	std::ofstream hout(h);

	CPPHeader::write(cppout, sql.filename().string());
	const auto fullns {boost::algorithm::join(sqlnsparts, "::")};
	CPPNS::write(cppout, fullns);
	CPPOpen::write(cppout, sql.stem().string());
	std::string buf;
	std::stringstream map;
	while (!std::getline(sqlin, buf).eof()) {
		if (boost::algorithm::starts_with(buf, "-- libdbpp-")) {
			connector = buf.substr(11);
		}
		else if (boost::algorithm::starts_with(buf, "-- libdbpp:")) {
			auto opt = buf.substr(11);
			auto colon = opt.find(':');
			if (colon != std::string::npos) {
				CPPConnectorOpt::write(map, opt.substr(0, colon), opt.substr(colon + 1));
			}
			else {
				CPPConnectorOptFlag::write(map, opt);
			}
		}
		else {
			cppout << buf << std::endl;
		}
	}
	CPPClose::write(cppout);
	if (!connector.empty() && !map.str().empty()) {
		CPPConnectorOpts::write(cppout, connector, map.str());
	}
	CPPFooter::write(cppout);

	HHeader::write(hout);
	OpenNamespace::write(hout, fullns);
	HDeclartion::write(hout, sql.stem().string());
	CloseNamespace::write(hout, fullns);
	HFooter::write(hout);

	return 0;
}
