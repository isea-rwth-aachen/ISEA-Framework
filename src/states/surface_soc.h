/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : surface_soc.h
* Creation Date : 30-06-2014
* Last Modified : Mi 02 Jul 2014 11:41:57 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SURFACE_SOC_
#define _SURFACE_SOC_

// STD
#include <vector>
#include <limits>
#include <boost/shared_ptr.hpp>

// ETC
#include "dgl_state.h"

#include "../object/object.h"

#include "../misc/matrixInclude.h"


namespace electrical
{
template < typename T >
class TwoPort;
// namespace state { class Soc; }
};


/// DESCRIPTION
namespace electrical
{
namespace state
{
/// This class describes the behaviour of the State of Charge depending on the capacity
class SurfaceSoc : public ::state::State    //:  public Soc
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
    void SetElementsForLookup( std::vector< electrical::TwoPort< myMatrixType > * > vec );    //< This setter provides a
    // list of all twoPorts
    // that are used for
    // calculating the outer
    // shells overpotential

    virtual double GetMaxDt() const { return ::std::numeric_limits< double >::max(); }
    virtual ~SurfaceSoc(){};

    double GetValue() const;

    private:
    boost::shared_ptr< object::Object< double > > mReverseLookup;
    std::vector< ::electrical::TwoPort< myMatrixType > * > mVecElements;
    double mCurrentValue;
};
} /* state */
}
#endif /* _SURFACE_SOC_ */
