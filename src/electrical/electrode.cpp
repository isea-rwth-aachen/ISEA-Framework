#include "electrode.h"

namespace electrical
{

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

template class electrical::Electrode< myMatrixType >;