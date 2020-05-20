#ifndef _DEFAULT_CONVECTION_
#define _DEFAULT_CONVECTION_

#include <boost/shared_ptr.hpp>
#include "../thermal_structs.h"
#include "../../object/lookup_obj2d.h"
#include "../../exceptions/error_proto.h"


namespace thermal
{
using boost::shared_ptr;

/// DefaultConvection is interface class for the default convective cooling for external areas
template < typename T = double >
class DefaultConvection
{
    public:
    DefaultConvection(){};
    virtual ~DefaultConvection(){};
    /// Returns offset and slope for first order Taylor approximation of power per area cooled
    virtual Linearization< T > GetOffsetSlope( T surfaceTemperature, T characteristicLength, T airTemperature ) = 0;
    virtual bool isForced() const { return false; };
};


/// ConvectionByFormula returns the cooling of an area by convection with Heiko's proposed formula
template < typename T = double >
class ConvectionByFormula : public DefaultConvection< T >
{
    public:
    explicit ConvectionByFormula( T preFactor );
    virtual ~ConvectionByFormula(){};
    Linearization< T > GetOffsetSlope( T surfaceTemperature, T characteristicLength, T airTemperature );

    private:
    T mHelp_SameForAll;
    const T mAbsoluteZeroInC;
};


template < typename T >
ConvectionByFormula< T >::ConvectionByFormula( T preFactor )
    : mAbsoluteZeroInC( -273.15 )
{
    if ( preFactor <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "preFactorNegative" );

    const T pr_Air = 0.7075;              // VDI D2.2 Tablle 16 25°C 1 bar
    const T lambda_Air = 26.25 / 1000;    // VDI D2.2 Tablle 12 25°C 1 bar
    const T ny_Air = 0.00001579;          // VDI D2.2 Tabelle 14 157.9 * 10^-7 m²/s  25°C 1 bar
    const T g = 9.81;
    mHelp_SameForAll = preFactor * pow( pr_Air * g / ( ny_Air * ny_Air ), 0.25 ) * lambda_Air;
}

template < typename T >
Linearization< T > ConvectionByFormula< T >::GetOffsetSlope( T surfaceTemperature, T characteristicLength, T airTemperature )
{
    const T deltaTemperature = surfaceTemperature - airTemperature;
    if ( deltaTemperature < 0.0 )
        return Linearization< T >( 0.0, 0.0 );

    const T help_SameForOffsetSlope =
     mHelp_SameForAll * pow( ( airTemperature - mAbsoluteZeroInC ) * characteristicLength, -0.25 );
    return Linearization< T >( help_SameForOffsetSlope * pow( deltaTemperature, 1.25 ),
                               help_SameForOffsetSlope * 1.25 * pow( deltaTemperature, 0.25 ) );
}

template < typename T = double >
class ForcedConvectionByFormula : public DefaultConvection< T >
{
    public:
    explicit ForcedConvectionByFormula( T preFactor );
    virtual ~ForcedConvectionByFormula(){};
    Linearization< T > GetOffsetSlope( T surfaceTemperature, T characteristicLength, T airTemperature );

    virtual bool isForced() const { return true; };

    private:
    const T mPreFactor;
};

template < typename T >
ForcedConvectionByFormula< T >::ForcedConvectionByFormula( T preFactor )
    : mPreFactor( preFactor )
{
    if ( preFactor <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "preFactorNegative" );
}

template < typename T >
Linearization< T > ForcedConvectionByFormula< T >::GetOffsetSlope( T, T, T )
{
    return Linearization< T >( mPreFactor, mPreFactor );
}


/// ConvectionByLookUp returns the convective cooling of an area defined by a look up table
template < typename T = double >
class ConvectionByLookUp : public DefaultConvection< T >
{
    public:
    ConvectionByLookUp( shared_ptr< object::Object< T > > offsetLookUp, shared_ptr< object::Object< T > > slopeLookUp );
    virtual ~ConvectionByLookUp(){};
    virtual Linearization< T > GetOffsetSlope( T surfaceTemperature, T characteristicLength, T airTemperature );

    private:
    const boost::shared_ptr< object::Object< T > > mOffsetLookUp;
    const shared_ptr< object::Object< T > > mSlopeLookUp;
};

template < typename T >
ConvectionByLookUp< T >::ConvectionByLookUp( shared_ptr< object::Object< T > > offsetLookUp, shared_ptr< object::Object< T > > slopeLookUp )
    : mOffsetLookUp( offsetLookUp )
    , mSlopeLookUp( slopeLookUp )
{
}

template < typename T >
Linearization< T > ConvectionByLookUp< T >::GetOffsetSlope( T surfaceTemperature, T characteristicLength, T )
{
    return Linearization< T >( mOffsetLookUp->GetValue( surfaceTemperature, characteristicLength ),
                               mSlopeLookUp->GetValue( surfaceTemperature, characteristicLength ) );
}
}
#endif
