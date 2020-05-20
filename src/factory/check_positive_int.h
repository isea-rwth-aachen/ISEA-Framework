#ifndef _CHECK_POSITIVE_INT_
#define _CHECK_POSITIVE_INT_

#include "../xmlparser/xmlparameter.h"



namespace factory
{

size_t CheckPositiveInt(const int &val, const boost::shared_ptr<xmlparser::XmlParameter> &param, const char name[]);

}

#endif