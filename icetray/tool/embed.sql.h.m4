define(`foreach',`ifelse(eval($#>2),1,
	`pushdef(`$1',`$3')$2`'popdef(`$1')
  `'ifelse(eval($#>3),1,`$0(`$1',`$2',shift(shift(shift($@))))')')')
define(NAMESPACEC,`patsubst(NAMESPACE,`::', `,')')
#include <staticSqlSource.h>

foreach(`ns', `namespace ns {	', NAMESPACEC)
	extern const IceTray::StaticSqlSource NAME;
foreach(`ns', `}', NAMESPACEC)

