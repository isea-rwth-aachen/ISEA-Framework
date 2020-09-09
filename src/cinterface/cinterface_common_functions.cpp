/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : cinterface.cpp
* Creation Date : 23-05-2013
* Last Modified : Mi 14 Okt 2015 17:26:21 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "cinterface_common_functions.h"

// STD
#include <algorithm>

// ETC
//
#include "../convert/double2char.h"
#include "../cstring/strlcpy.h"
#include "../xmlparser/tinyxml2/xmlparserimpl.h"

#include "../exceptions/error_proto.h"

#ifndef _SYMBOLIC_

#ifdef __cplusplus
extern "C"
{
#endif

    void CheckVectorSizes( const size_t &smallInt, const size_t &bigInt, const char *myChars )
    {
        if ( smallInt > bigInt )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "VectorSizeError", myChars, smallInt, bigInt );
    }

    void CreateCharArrayFromDouble( double *arry, size_t buflen, char *target )
    {
        Double2Char d2c( arry, buflen );
        strlcpy( target, d2c.mReturnArray.get(), buflen );
    }

    size_t PickLargerSize( const size_t &size1, const size_t &size2 ) { return std::max( size1, size2 ); }

    size_t PickSmallerSize( const size_t &size1, const size_t &size2 ) { return std::min( size1, size2 ); }

    void GetDefinedSizes( const char *configStr, size_t *stateSize, size_t *observerSize, size_t *numberOfCells, size_t *numberOfProbes )
    {
        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromMem( configStr );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();

        if ( stateSize )
        {
            if ( parser.GetRoot()->HasElementDirectChild( "Options" ) &&
                 parser.GetRoot()->GetElementChild( "Options" )->HasElementDirectChild( "MinStateSize" ) )
                *stateSize = parser.GetRoot()->GetElementChild( "Options" )->GetElementIntValue( "MinStateSize", 1 );
            else
                *stateSize = 1;
        }

        if ( observerSize )
        {
            if ( parser.GetRoot()->HasElementDirectChild( "Options" ) &&
                 parser.GetRoot()->GetElementChild( "Options" )->HasElementDirectChild( "MinOutputSize" ) )
                *observerSize = parser.GetRoot()->GetElementChild( "Options" )->GetElementIntValue( "MinOutputSize", 1 );
            else
                *observerSize = 1;
        }

        if ( numberOfCells )
        {
            if ( parser.GetRoot()->HasElementDirectChild( "Options" ) &&
                 parser.GetRoot()->GetElementChild( "Options" )->HasElementDirectChild( "MinNumberOfCells" ) )
                *numberOfCells = parser.GetRoot()->GetElementChild( "Options" )->GetElementIntValue( "MinNumberOfCells", 1 );
            else
                *numberOfCells = 1;
        }
        if ( numberOfProbes )
        {
            if ( parser.GetRoot()->HasElementDirectChild( "Options" ) &&
                 parser.GetRoot()->GetElementChild( "Options" )->HasElementDirectChild( "MinNumberOfProbes" ) )
                *numberOfProbes =
                 parser.GetRoot()->GetElementChild( "Options" )->GetElementIntValue( "MinNumberOfProbes", 1 );
            else
                *numberOfProbes = 1;
        }
    }


#ifdef __cplusplus
}
#endif
#endif /* _SYMBOLIC_ */
