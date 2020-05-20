#include "check_positive_int.h"

//#include "../thermal/exceptions/parametrization_error.h"
#include "../exceptions/error_proto.h"
#include "../misc/macros.h"
#include <stdexcept>


namespace factory
{

size_t CheckPositiveInt( const int &val, const boost::shared_ptr< xmlparser::XmlParameter > &param, const char name[] )
{
    if ( val < 1 )
    {
        if ( param.get() )
        {
            const size_t MAXSIZE = 1024;
            char description[MAXSIZE] = {0};
            sprintf( description, "xml-file line %i", static_cast< int >( param->GetLineNumber() ) );
            char stringbuffer[MAXSIZE] = {0};
            real_snprintf( stringbuffer, MAXSIZE, "Integer %s must not be negative or zero", name );
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "error56", stringbuffer, description );
        }
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedXmlParameterCheckPositiveInt" );
    }
    return val;
}
}
