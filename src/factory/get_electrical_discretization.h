#ifndef _GET_ELECTRICAL_DISCRETIZATION_
#define _GET_ELECTRICAL_DISCRETIZATION_

#include "../xmlparser/xmlparameter.h"



namespace factory
{

///Returns the number of cell elements in a cell/thermal block
size_t GetElectricalDiscretization(const boost::shared_ptr<xmlparser::XmlParameter> &thermalBlockNode);

}

#endif