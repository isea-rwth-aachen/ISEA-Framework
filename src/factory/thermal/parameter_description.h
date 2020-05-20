#ifndef _PARAMETER_DESCRIPTION_
#define _PARAMETER_DESCRIPTION_

#include "../../xmlparser/xmlparameter.h"
#include "../../misc/StrCont.h"
#include "../../geometry/cartesian.h"



namespace factory
{

/**
 * Gets the xml-parameter and the class name and returns this information as a misc::StrCont
 */
misc::StrCont ParameterDescription(const xmlparser::XmlParameter* param, const char *className);

/**
 * Gets the xml-parameter and the class name and returns this information as a misc::StrCont
 * @param[in] x,y,z Global coordinates of emplacement point
 */
template<typename T>
misc::StrCont ParameterDescription(const xmlparser::XmlParameter* param, const char *className, geometry::Cartesian<T> emplacementPoint)
{
    return misc::StrCont(param->GetElementName())
         + misc::StrCont(" of class ") + misc::StrCont(className)
         + misc::StrCont(" at (") + misc::StrCont(emplacementPoint.GetX()) + misc::StrCont(",") + misc::StrCont(emplacementPoint.GetY()) + misc::StrCont(",")+ misc::StrCont(emplacementPoint.GetZ()) + misc::StrCont(")")
         + misc::StrCont(" in xml-file line ") + misc::StrCont(param->GetLineNumber());
}

}

#endif