/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : surface_soc.h
* Creation Date : 30-06-2014
* Last Modified : Mi 02 Jul 2014 11:41:57 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SURFACE_SOC_
#define _SURFACE_SOC_

#ifdef max
#pragma push_macro( "max" )
#undef max
#define RESTORE_MAX_MACRO
#endif

// STD
#include <boost/shared_ptr.hpp>
#include <limits>
#include <vector>

// ETC
#include "dgl_state.h"

#include "../object/object.h"

#include "../misc/matrixInclude.h"


namespace electrical
{
template < typename T >
class TwoPort;
};


namespace state
{
/// This class describes the behaviour of the State of Charge depending on the capacity
class SurfaceSoc : public State
{
    public:
    SurfaceSoc();
    /// This function has to be called manually. After the call mCurrentValue will be updated with the result of the
    /// reverse lookup.
    /// This can't be done automatically/with state as this would lead to inconsistent states for different twoports,
    /// depending on their creation order
    void UpdateLookUp();

    void SetReverseLookup( boost::shared_ptr< object::Object< double > > reverseLookup );    //< A setter method for the
    // reverse lookup object
    void SetElementsForLookup( std::vector< electrical::TwoPort< myMatrixType >* > vec );    //< This setter provides a
    // list of all twoPorts
    // that are used for
    // calculating the outer
    // shells overpotential

    virtual double GetMaxDt() const { return ::std::numeric_limits< double >::max(); }
    virtual ~SurfaceSoc(){};

    double GetValue() const;
    double GetLastValue() const;

    private:
    boost::shared_ptr< object::Object< double > > mReverseLookup;
    std::vector< ::electrical::TwoPort< myMatrixType >* > mVecElements;
    double mCurrentValue;
    double mLastValue;
};
}    // namespace state

#ifdef RESTORE_MAX_MACRO
#pragma pop_macro( "size" )
#undef RESTORE_MAX_MACRO
#endif

#endif /* _SURFACE_SOC_ */
