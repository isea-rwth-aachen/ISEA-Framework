#include "parameter_description.h"



namespace factory
{

misc::StrCont ParameterDescription(const xmlparser::XmlParameter* param, const char *className)
{
    return misc::StrCont(param->GetElementName())
         + misc::StrCont(" of class ") + misc::StrCont(className)
         + misc::StrCont(" in xml-file line ") + misc::StrCont(param->GetLineNumber());
}

template misc::StrCont ParameterDescription(const xmlparser::XmlParameter* param, const char *className, geometry::Cartesian<double> emplacementPoint);

}
