changecom(`@@')
#include "NAME.sql.h"

const IceTray::StaticSqlSource NAMESPACE::NAME ({
patsubst(esyscmd(`xxd -p -c 12 ' SQL), `\(..\)', `0x\1, ')0x0a});

