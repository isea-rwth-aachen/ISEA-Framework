#ifndef _ELECTRODE_
#define _ELECTRODE_

#include "paralleltwoport.h"

namespace electrical
{

/// Parallel twoport that holds all AnodeElements or CathodeElements of an electrode.
/// This class is created automatically by the factory and cannot be added to the ECM manually.
/// It forces the calculation of current and voltage values even if the twoport is not observable.
template < typename MatrixT >
class Electrode : public ParallelTwoPort< MatrixT >
{
    public:
    Electrode( const bool observable = false, typename TwoPort< MatrixT >::DataType dataValues =
                                               typename TwoPort< MatrixT >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    virtual ~Electrode(){};

    virtual const char* GetName() const;
    virtual void CalculateStateDependentValues();
};

template < typename MatrixT >
Electrode< MatrixT >::Electrode( const bool observable, typename TwoPort< MatrixT >::DataType dataValues )
    : ParallelTwoPort< MatrixT >( observable, dataValues )
{
}

template < typename MatrixT >
const char* Electrode< MatrixT >::GetName() const
{
    return "Electrode";
}

template < typename MatrixT >
void Electrode< MatrixT >::CalculateStateDependentValues()
{
    ParallelTwoPort< MatrixT >::CalculateStateDependentValues();
    // for observable TwoPorts, the the values are already calculated
    if ( !this->IsObservable() )
    {
        this->CalculateCurrentValue();
        this->CalculateVoltageValue();
    }
}

}    // namespace electrical

#endif