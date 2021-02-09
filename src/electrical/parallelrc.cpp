/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : parallelrc.cpp
* Creation Date : 20-03-2013
* Last Modified : Di 01 Okt 2013 23:14:32 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "parallelrc.h"

namespace electrical
{

template < typename T >
ParallelRC< T >::ParallelRC( boost::shared_ptr< object::Object< double > > objR, boost::shared_ptr< object::Object< double > > objC,
                             const bool observable, typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( objR, observable, dataValues )
    , mUID( 0 )
    , mObjectTau( objC )
    , mConstVoltageVector( T() )
    , mDGLPart( T() )
{
}

template < typename T >
ParallelRC< T >::ParallelRC( object::Object< double >* objR, object::Object< double >* objC, const bool observable,
                             typename TwoPort< T >::DataType dataValues )
    : ParallelRC< T >( boost::shared_ptr< object::Object< double > >( objR ),
                       boost::shared_ptr< object::Object< double > >( objC ), observable, dataValues )
{
}

template < typename T >
void ParallelRC< T >::SetCurrent( const T current )
{
    TwoPort< T >::SetCurrent( current );
    SetVoltage();
}

template < typename T >
void ParallelRC< T >::SetCurrent( const ScalarUnit currentval )
{
    TwoPort< T >::SetCurrent( currentval );
}

template < typename T >
T* ParallelRC< T >::GetVoltage()
{

    if ( this->HasSystem() )
    {
        mDGLPart = this->mCurrent * this->GetValue() / GetTauValue();
        mDGLPart.coeffRef( this->mCurrent.rows() - 1, mUID ) -= ScalarUnit( 1.0 ) / GetTauValue();
        this->mStateSystemGroup->mDglStateSystem.AddEquations( mUID, mDGLPart );
    }
    return TwoPort< T >::GetVoltage();
}

template < typename T >
void ParallelRC< T >::SetVoltage()
{
    mConstVoltageVector.resize( this->mCurrent.rows(), this->mCurrent.cols() );
    mConstVoltageVector.setZero();
    mConstVoltageVector.coeffRef( 0, mUID ) = 1;
    this->mVoltage = mConstVoltageVector;
    mDGLPart.resize( this->mCurrent.rows(), this->mCurrent.cols() );
    mDGLPart.setZero();
}

template < typename T >
void ParallelRC< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{
    mUID = stateSystemGroup->mDglStateSystem.GetNewEquation();
    TwoPort< T >::SetSystem( stateSystemGroup );
}

template < typename T >
void ParallelRC< T >::CalculateStateDependentValues()
{
    this->CalculateVoltageValue();

    if ( this->mObservable )
        this->CalculateCurrentValue();

    this->mPowerValue = ::std::pow( this->mVoltageValue, static_cast< int >( 2 ) ) / this->GetValue();
}

template < typename T >
const char* ParallelRC< T >::GetName() const
{
    return "ParallelRC";
}

template < typename T >
ScalarUnit ParallelRC< T >::GetValue() const
{
#ifndef _SYMBOLIC_
    return ElectricalElement< T >::GetValue();
#else
    return ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjR" );
#endif
}


template < typename T >
ScalarUnit ParallelRC< T >::GetValueC() const
{
#ifndef _SYMBOLIC_
    return ( ( *mObjectTau )() / this->GetValue() );
#else
    return ( *mObjectTau )() / ( *ElectricalElement< T >::mObject )();
#endif
}

template < typename T >
const object::Object< double >* ParallelRC< T >::GetTauObject() const
{
    return mObjectTau.get();
}

template < typename T >
ScalarUnit ParallelRC< T >::GetTauValue() const
{
#ifndef _SYMBOLIC_
    return ( *mObjectTau )();
#else
    return ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjTau" );
#endif
}

template < typename T >
void ParallelRC< T >::SetResistanceFactor( const double factor )
{
    this->mObject->SetFactor( factor );
}

}    // namespace electrical

template class electrical::ParallelRC< myMatrixType >;
