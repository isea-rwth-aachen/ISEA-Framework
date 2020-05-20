#ifndef _TOLERANCE_
#define _TOLERANCE_

#include "angle.h"
#include "../exceptions/error_proto.h"
//#include "../thermal/exceptions/initialization_error.h"


namespace geometry
{

/// Tolerance contains information to handle geometrical unprecision
template < typename T = double >
struct Tolerance
{
    /**
     * @param[in] length Maximum distance of two points to be still considered equal
     * @param[in] angle Maximum angle difference between two vectors to be still considered to have the same direction
     * @param[in] percentOfQuantity Maximum difference two physical) quantities in percentage to be still considered
     * equal
     */
    explicit Tolerance( T length = 0.0, const Angle< T > &angle = Angle< T >(), T percentOfQuantity = 0.0 )
        : mLength( length )
        , mAngle( angle )
        , mPercentOfQuantity( percentOfQuantity )
    {
        if ( length < 0.0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "LengthSmallerZero" );
        if ( angle.GetRad() < 0.0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AngleSmallerZero" );
        if ( percentOfQuantity < 0.0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "PercentOfQunatitySmallerZero" );
    }

    T mLength;
    Angle< T > mAngle;
    T mPercentOfQuantity;
};
}
#endif
