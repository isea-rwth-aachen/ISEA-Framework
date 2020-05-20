#ifndef _COOLING_
#define _COOLING_

#include "../../geometry/two_dim.h"
#include "../../object/lookup_obj1d.h"
#include "../thermal_structs.h"
#include <boost/shared_ptr.hpp>


namespace thermal
{
using boost::shared_ptr;
using std::vector;

/// Cooling is the interface class that returns physics data of cooling
template < typename T = double >
class Cooling
{
    public:
    Cooling(){};
    virtual ~Cooling(){};
    /// Returns offset and slope for first order Taylor approximation of cooling power per area
    virtual Linearization< T > GetOffsetSlope( T surfaceTemperature, T airTemperature ) const = 0;
    virtual bool IsDirichletBoundaryCondition() const = 0;

    private:
};


/// DirichletBoundaryCondition returns the its temperature at the boundary surface
template < typename T = double >
class DirichletBoundaryCondition : public Cooling< T >
{
    public:
    DirichletBoundaryCondition( T surfaceTemperature );
    virtual ~DirichletBoundaryCondition(){};
    Linearization< T > GetOffsetSlope( T surfaceTemperature, T airTemperature ) const;
    bool IsDirichletBoundaryCondition() const { return true; };
    T GetSurfaceTemperature() const { return mSurfaceTemperature; };

    private:
    T mSurfaceTemperature;
};

template < typename T >
DirichletBoundaryCondition< T >::DirichletBoundaryCondition( T surfaceTemperature )
    : mSurfaceTemperature( surfaceTemperature )
{
}

template < typename T >
Linearization< T > DirichletBoundaryCondition< T >::GetOffsetSlope( T, T ) const
{
    return Linearization< T >( mSurfaceTemperature, mSurfaceTemperature );
}


/// Radiation returns the cooling of an area by radiation with the Stefan-Boltzman law
template < typename T = double >
class Radiation : public Cooling< T >
{
    public:
    Radiation();
    virtual ~Radiation(){};
    Linearization< T > GetOffsetSlope( T surfaceTemperature, T airTemperature ) const;
    bool IsDirichletBoundaryCondition() const { return false; };

    private:
    T mHelp_Factor;
    T mAbsoluteZeroInC;
};

template < typename T >
Radiation< T >::Radiation()
    : mAbsoluteZeroInC( -273.15 )
{
    const T sigma = 0.0000000567;
    const T epsilon = 1.0;
    mHelp_Factor = sigma * epsilon;
}

template < typename T >
Linearization< T > Radiation< T >::GetOffsetSlope( T surfaceTemperature, T airTemperature ) const
{
    if ( surfaceTemperature < airTemperature )
        return Linearization< T >( 0.0, 0.0 );

    if ( airTemperature < mAbsoluteZeroInC )
        airTemperature = mAbsoluteZeroInC;
    surfaceTemperature -= mAbsoluteZeroInC;
    airTemperature -= mAbsoluteZeroInC;

    return Linearization< T >( mHelp_Factor * ( pow( surfaceTemperature, 4.0 ) - pow( airTemperature, 4.0 ) ),
                               mHelp_Factor * 4.0 * pow( surfaceTemperature, 3.0 ) );
}


/// CoolingByLookUp returns the cooling of an area defined by a look up table
template < typename T = double >
class CoolingByLookUp : public Cooling< T >
{
    public:
    ///@param offsetLookUp,slopeLookUp Value returned in Linearization is Object::GetValue(surfaceTemperature)
    CoolingByLookUp( shared_ptr< object::Object< T > > offsetLookUp, shared_ptr< object::Object< T > > slopeLookUp );
    virtual ~CoolingByLookUp(){};
    virtual Linearization< T > GetOffsetSlope( T surfaceTemperature, T airTemperature ) const;
    bool IsDirichletBoundaryCondition() const { return false; };
    const shared_ptr< object::Object< T > >& GetOffsetLookUp() const;
    const shared_ptr< object::Object< T > >& GetSlopeLookUp() const;

    private:
    const shared_ptr< object::Object< T > > mOffsetLookUp;
    const shared_ptr< object::Object< T > > mSlopeLookUp;
};

template < typename T >
CoolingByLookUp< T >::CoolingByLookUp( shared_ptr< object::Object< T > > offsetLookUp, shared_ptr< object::Object< T > > slopeLookUp )
    : mOffsetLookUp( offsetLookUp )
    , mSlopeLookUp( slopeLookUp )
{
}

template < typename T >
Linearization< T > CoolingByLookUp< T >::GetOffsetSlope( T surfaceTemperature, T ) const
{
    return Linearization< T >( mOffsetLookUp->GetValue( surfaceTemperature ), mSlopeLookUp->GetValue( surfaceTemperature ) );
}

template < typename T >
const shared_ptr< object::Object< T > >& CoolingByLookUp< T >::GetOffsetLookUp() const
{
    return mOffsetLookUp;
}

template < typename T >
const shared_ptr< object::Object< T > >& CoolingByLookUp< T >::GetSlopeLookUp() const
{
    return mSlopeLookUp;
}

/// CoolingByConstantValue returns steady value passed in constructor for cooling of an area
template < typename T = double >
class CoolingByConstantValue : public Cooling< T >
{
    public:
    CoolingByConstantValue( T value );
    virtual ~CoolingByConstantValue(){};
    virtual Linearization< T > GetOffsetSlope( T surfaceTemperature, T airTemperature ) const;
    bool IsDirichletBoundaryCondition() const { return false; };
    const T& GetValue() const;

    private:
    const Linearization< T > mOffsetSlope;
};

template < typename T >
CoolingByConstantValue< T >::CoolingByConstantValue( T value )
    : mOffsetSlope( value, 0.0 )
{
}

template < typename T >
Linearization< T > CoolingByConstantValue< T >::GetOffsetSlope( T, T ) const
{
    return mOffsetSlope;
}

template < typename T >
const T& CoolingByConstantValue< T >::GetValue() const
{
    return mOffsetSlope.mOffset;
}
}    // namespace thermal
#endif
