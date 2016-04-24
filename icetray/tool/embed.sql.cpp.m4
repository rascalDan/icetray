changecom(`@@')
#include "NAME.sql.h"

const std::string NAMESPACE::NAME::sql({
patsubst(esyscmd(`xxd -p -c 12 ' SQL), `\(..\)', `0x\1, ')0x0a});
const std::size_t NAMESPACE::NAME::hash(std::hash<std::string>()(sql));

