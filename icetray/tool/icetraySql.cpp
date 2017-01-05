#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <compileTimeFormatter.h>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

AdHocFormatter(CPPHeader,
		R"C(#include "%?.h"

const IceTray::StaticSqlSource )C");
AdHocFormatter(CPPNS, "%?::");
AdHocFormatter(CPPOpen, R"C(%? (
R"SQL()C");
AdHocFormatter(CPPFooter, R"C()SQL");

)C");
AdHocFormatter(HHeader,
		R"H(#include <staticSqlSource.h>

)H");
AdHocFormatter(HDeclartion, "extern const IceTray::StaticSqlSource %?;\n");
AdHocFormatter(HFooter, "\n");

AdHocFormatter(OpenNamespace, "namespace %? {\n");
AdHocFormatter(CloseNamespace, "} // %?\n");

int
main(int argc, char ** argv)
{
	po::options_description opts("IceTray SQL-to-C++ precompiler");
	fs::path sql, cpp, h, base;
	std::string sqlns;

	opts.add_options()
		("help,h", "Show this help message")
		("sql", po::value(&sql)->required(), "Path of SQL script")
		("cpp", po::value(&cpp)->required(), "Path of C++ file to write")
		("h", po::value(&h)->required(), "Path of header file to write")
		("basedir,d", po::value(&base)->default_value(fs::current_path()), "Base directory of SQL scripts (namespaces are created relative to here)")
		("namespace", po::value(&sqlns), "Namespace to create SqlSource in")
		;

	po::positional_options_description p;
	p.add("sql", 1);
	p.add("cpp", 1);
	p.add("h", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(opts).positional(p).run(), vm);

	if (vm.count("help")) {
		std::cout << opts << std::endl;
		return 1;
	}
	po::notify(vm);

	std::vector<std::string> sqlnsparts;
	boost::algorithm::split(sqlnsparts, sqlns, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
	auto r = fs::canonical(sql).lexically_relative(fs::canonical(base)).parent_path();
	if (!r.empty()) {
		std::transform(r.begin(), r.end(), std::back_inserter(sqlnsparts), [](const auto & p) { return p.string(); });
	}

	std::ifstream sqlin(sql.string());
	std::ofstream cppout(cpp.string());
	std::ofstream hout(h.string());

	CPPHeader::write(cppout, sql.leaf().string());
	std::for_each(sqlnsparts.begin(), sqlnsparts.end(), [&cppout](const auto & nsp) {
		CPPNS::write(cppout, nsp);
	});
	CPPOpen::write(cppout, sql.stem().string());
	std::copy(std::istreambuf_iterator<char>(sqlin),
			std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(cppout));
	CPPFooter::write(cppout);

	HHeader::write(hout);
	std::for_each(sqlnsparts.begin(), sqlnsparts.end(), [&hout](const auto & nsp) {
		OpenNamespace::write(hout, nsp);
	});
	HDeclartion::write(hout, sql.stem().string());
	std::for_each(sqlnsparts.begin(), sqlnsparts.end(), [&hout](const auto & nsp) {
		CloseNamespace::write(hout, nsp);
	});
	HFooter::write(hout);

	return 0;
}

