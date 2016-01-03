define(`foreach',`ifelse(eval($#>2),1,
	`pushdef(`$1',`$3')$2`'popdef(`$1')
  `'ifelse(eval($#>3),1,`$0(`$1',`$2',shift(shift(shift($@))))')')')
define(NAMESPACEC,`patsubst(NAMESPACE,`::', `,')')
#include <functional>
#include <string>

foreach(`ns', `namespace ns {	', NAMESPACEC)
	class NAME {
		public:
			static const std::string sql;
			static const std::size_t hash;
	};
foreach(`ns', `}', NAMESPACEC)

