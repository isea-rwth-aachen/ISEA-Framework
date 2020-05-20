#include "get_electrical_discretization.h"

#include <cstdio>
#include <stdexcept>

//#include "../thermal/exceptions/parametrization_error.h"
#include "../exceptions/error_proto.h"
#include "../misc/cmp_str.h"
#include "../misc/StrCont.h"


namespace factory
{

size_t GetElectricalDiscretization( const boost::shared_ptr< xmlparser::XmlParameter > &thermalBlockParam )
{
    if ( !thermalBlockParam->HasElementDirectChild( "ElectricalDiscretization" ) )
        return 1;

    boost::shared_ptr< xmlparser::XmlParameter > discretizationParam =
     thermalBlockParam->GetElementChild( "ElectricalDiscretization" );
    misc::StrCont description = misc::StrCont( "xml-file line " ) + misc::StrCont( discretizationParam->GetLineNumber() );
    if ( discretizationParam->HasElementAttribute( "rho" ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DiscretizationRhoNonImplemented", "xml-file line",
                                             discretizationParam->GetLineNumber() );

    int totalDiscretization = 1;
    misc::equal_str comp;
    const char *classname = thermalBlockParam->GetElementAttributeCharPtrValue( "class" );

    if ( comp( classname, "RectangularBlock" ) )
        totalDiscretization = discretizationParam->GetElementAttributeIntValue( "x", 1 ) *
                              discretizationParam->GetElementAttributeIntValue( "y", 1 ) *
                              discretizationParam->GetElementAttributeIntValue( "z", 1 );
    else if ( comp( classname, "QuadraticCellBlock" ) || comp( classname, "Supercap" ) ||
              comp( classname, "HexagonalCellBlock" ) )
        totalDiscretization = discretizationParam->GetElementAttributeIntValue( "phi", 1 ) *
                              discretizationParam->GetElementAttributeIntValue( "z", 1 );
    else if ( comp( classname, "TriangularPrismBlock" ) )
        totalDiscretization = discretizationParam->GetElementAttributeIntValue( "z", 1 );
    else
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ThermalBlockClassNotValid", "xml-file line",
                                             discretizationParam->GetLineNumber() );

    if ( totalDiscretization <= 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ElectricalDiscretizationNegative", "xml-file line",
                                             discretizationParam->GetLineNumber() );

    return totalDiscretization;
}
}
