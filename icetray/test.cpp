#include <factory.impl.h>
#include <boost/any.hpp>

template class std::shared_ptr<boost::any>;
INSTANTIATEFACTORY(int, std::shared_ptr<boost::any>);

