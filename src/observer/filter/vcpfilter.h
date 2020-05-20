/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : vcpfilter.h
* Creation Date : 30-08-2013
* Last Modified : Do 15 Jun 2017 16:43:33 CEST
* Created By : Christopher Leroi
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _VCPFilter_
#define _VCPFilter_

#ifndef _SYMBOLIC_
#ifdef _MATLAB_
#else
typedef double real_T;
#endif


// BOOST
#include <boost/shared_ptr.hpp>

#include "../../electrical/cellelement.h"
#include "filter.h"

namespace observer
{
/// DESCRIPTION
template < typename T = myMatrixType >
class VCPFilter : public Filter< T, electrical::TwoPort, PreparationType< T > >
{
    public:
    typedef Filter< T, electrical::TwoPort, PreparationType< T > > FilterT;
    VCPFilter( real_T *voltageArray = 0, real_T *currentArray = 0, real_T *powerArray = 0, real_T *socArray = 0,
               real_T *surfaceSoc = 0 );
    virtual ~VCPFilter(){};

    void SetArrays( real_T *voltageArray, real_T *currentArray, real_T *powerArray, real_T *socArray, real_T *surfaceSoc );

    virtual void ProcessData( const typename FilterT::Data_t &data,
                              const double t )    // <--- needs to be inplace otherwise VS will barf
    {
        for ( size_t i = 0; i < data.size(); ++i )
        {
            electrical::TwoPort< T > *port = data[i];

            mVoltageArray[i] = port->GetVoltageValue();
            mCurrentArray[i] = port->GetCurrentValue();
            mPowerArray[i] = port->GetPowerValue();
            if ( auto tpWithState = dynamic_cast< electrical::TwoPortWithState< T > * >( port ) )
            {
                mSocArray[i] = tpWithState->GetSocValue();
                auto cell = dynamic_cast< electrical::Cellelement< T > * >( port );
                if ( cell && cell->HasSurfaceSoc() )
                    mSurfaceSocArray[i] = cell->GetSurfaceSocStateValue();
                else
                    mSurfaceSocArray[i] = 0;
            }
            else
            {
                mSocArray[i] = 0;
                mSurfaceSocArray[i] = 0;
            }
        }

        FilterT::ProcessData( data, t );
    };

    private:
    real_T *mVoltageArray;
    real_T *mCurrentArray;
    real_T *mPowerArray;
    real_T *mSocArray;
    real_T *mSurfaceSocArray;

    protected:
};

template < typename T >
VCPFilter< T >::VCPFilter( real_T *voltageArray, real_T *currentArray, real_T *powerArray, real_T *socArray, real_T *surfaceSoc )
    : mVoltageArray( voltageArray )
    , mCurrentArray( currentArray )
    , mPowerArray( powerArray )
    , mSocArray( socArray )
    , mSurfaceSocArray( surfaceSoc )
{
}

template < typename T >
void VCPFilter< T >::SetArrays( real_T *voltageArray, real_T *currentArray, real_T *powerArray, real_T *socArray, real_T *surfaceSoc )
{
    mVoltageArray = voltageArray;
    mCurrentArray = currentArray;
    mPowerArray = powerArray;
    mSocArray = socArray;
    mSurfaceSocArray = surfaceSoc;
}

}    // namespace observer
#endif /* _SYMBOLIC_*/

#endif /* _VCPFilter_ */
