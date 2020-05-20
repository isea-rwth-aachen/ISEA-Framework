/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : parallelrc.h
* Creation Date : 10-11-2012
* Last Modified : Do 10 Mär 2016 12:36:32 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _PARALLELRC_
#define _PARALLELRC_

// STD
#include <cmath>

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "electricalelement.h"

// friend
class TestElectricalElement;
class TestElectricalFactory;
class TestXML;

namespace electrical
{
/// This is an optimzed version of a parallel RC circuit. It mimics the equation  dU = ib/c - Uc /(RC).  This is one
/// equation less than through a parallel R and C Element
template < typename T = myMatrixType >
class ParallelRC : public ElectricalElement< T >
{
    friend class ::TestElectricalElement;
    friend class ::TestXML;
    friend class ::TestElectricalFactory;

    public:
    explicit ParallelRC( boost::shared_ptr< object::Object< double > > objR,
                         boost::shared_ptr< object::Object< double > > objC, const bool observable = false,
                         typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    explicit ParallelRC( object::Object< double >* objR, object::Object< double >* objC, const bool observable = false,
                         typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );

    virtual ~ParallelRC(){};

    virtual T* GetVoltage();                                   ///< returns the voltage over the RC element  Urc  = 1
    virtual void SetCurrent( const T current );                ///< Sets the current
    virtual void SetCurrent( const ScalarUnit currentval );    ///< Sets the current value
    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );
    virtual void CalculateStateDependentValues();
    virtual ScalarUnit GetValueC() const;    ///< Get Value of Object
    virtual ScalarUnit GetValue() const;     ///< Get Value of Object

    const object::Object< double >* GetTauObject() const;
    virtual ScalarUnit GetTauValue() const;    ///< Get value of tau Object
    virtual void SetResistanceFactor( const double factor );

    virtual const char* GetName() const;

    private:
    ParallelRC();
    void SetVoltage();

    size_t mUID;

    boost::shared_ptr< object::Object< double > > mObjectTau;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup for capacity

    T mConstVoltageVector;
    T mDGLPart;

    protected:
};


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
#endif /* _PARALLELRC_ */
