/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : twoport.h
* Creation Date : 30-10-2012
* Last Modified : 11.07.2018 21:12:02
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TWOPORT_
#define _TWOPORT_

// ETC
#include "../exceptions/error_proto.h"
#include "../system/stateSystemGroup.h"
#include "electrical_data_struct.h"

// STD
#include <vector>
#ifdef __EXCEPTIONS__
#include <stdexcept>
#endif /* __EXCEPTIONS__ */

// BOOST
#include <boost/shared_ptr.hpp>

// Friend
class TestRmphn;
class TestDaeSystem;
class TestStateSystem;
class TestLinearSystem;
class TestElectricalFactory;

class TestVoltageCurrentPowerInjection;

namespace electrical
{

/// Class is the basic interface for any TwoPort. In case of creating a new TwoPort this should be the least basic
/// class. The relation should rather be "HAS A" instead of "IS A"
template < typename T = myMatrixType >
class TwoPort
{

    friend class ::TestRmphn;
    friend class ::TestDaeSystem;
    friend class ::TestStateSystem;
    friend class ::TestLinearSystem;
    friend class ::TestVoltageCurrentPowerInjection;
    friend class ::TestElectricalFactory;

    public:
    typedef boost::shared_ptr< ElectricalDataStruct< ScalarUnit > > DataType;
    explicit TwoPort( const bool observable = false, DataType dataValues = DataType( new ElectricalDataStruct< ScalarUnit > ) );
    virtual ~TwoPort(){};

    virtual void SetCurrent( const T current );    ///< Sets the current
    const T GetCurrent() const;                    ///< Sets the current vector

    void UpdateStateSystemGroup();    ///<Calculates all voltage equations and saves them in the StateSystem
    void SetInitialCurrent( ScalarUnit current = 0 );    ///< Only called at the RootTwoPort. Creates the current vector and sends it into the network. It also makes the TwoPort observable.
    virtual void SetCurrent( const ScalarUnit currentval );    ///< Sets the current value

    virtual T* GetVoltage();    ///< Return a Voltage over the TwoPort including children

    const T* GetConstVoltage() const;    ///< Return a Voltage over the TwoPort including children

    virtual bool HasChildren() const;        ///< If this twoport has a child
    virtual bool CanHaveChildren() const;    ///< If this TwoPort is able to have children e.g. mChildren
    virtual bool IsCellelement() const;      ///< Is this element a cell element?

    virtual double GetTotalCapacity() const
    {
        return 0;
    }    ///< If the element has a capacity (e.g. Cellelement) we return  its capacity

    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );    ///< This function sets the system, where the TwoPorts registeres itself and submits its equations Dgl or Alg

    bool HasSystem() const;    ///< Does this TwoPort has a system set?

    bool IsObservable() const;    ///< Is this TwoPort observable?

    size_t GetID() const;
    void SetID( size_t iD );

    ScalarUnit GetCurrentValue() const;    ///< Returns the precalculated current
    ScalarUnit GetVoltageValue() const;    ///< Returns the precalculated voltage
    ScalarUnit GetPowerValue() const;      ///< Returns the precalculated power

#ifndef _SYMBOLIC_

    virtual void LoadInternalData( std::vector< double >& dataVector );    ///< Loads the StateDependentValues from a vector. Some kind serialization. Used for parallelization.
    virtual void LoadInternalData( const double& current, const double& voltage, const double& power );
    virtual void SaveInternalData( std::vector< double >& dataVector );    ///< Saves the StateDependentValues  to a vector.
#endif

    virtual void CalculateStateDependentValues();    ///< Calculates the StateDependentValues. Should be overwritten.
    virtual const char* GetName() const;
    virtual void SetResistanceFactor( const double factor );    // Sets a factor which all resistances are multiplied by
    const DataType& GetElectricalData() const;

    protected:
    /// Calculate the current of the previous simulation step.
    void CalculateCurrentValue();

    /// Calculate the voltage of the previous simulation step.
    void CalculateVoltageValue();

    DataType mDataStruct = 0;

    size_t mID;
    T mCurrent;
    T mVoltage;
    ScalarUnit& mCurrentValue;
    ScalarUnit& mVoltageValue;
    ScalarUnit& mPowerValue;

    systm::StateSystemGroup< T >* mStateSystemGroup;

    bool mObservable;
};

template < typename T >
void TwoPort< T >::CalculateStateDependentValues()
{
    if ( this->mObservable )
    {
        this->CalculateCurrentValue();
        this->CalculateVoltageValue();
    }
}

#ifndef _SYMBOLIC_

template < typename T >
void TwoPort< T >::LoadInternalData( std::vector< double >& dataVector )
{
    mPowerValue = dataVector.back();
    dataVector.pop_back();

    mVoltageValue = dataVector.back();
    dataVector.pop_back();

    mCurrentValue = dataVector.back();
    dataVector.pop_back();
}

template < typename T >
void TwoPort< T >::LoadInternalData( const double& current, const double& voltage, const double& power )
{
    mCurrentValue = current;
    mVoltageValue = voltage;
    mPowerValue = power;
}

template < typename T >
void TwoPort< T >::SaveInternalData( std::vector< double >& dataVector )
{
    dataVector.push_back( mCurrentValue );
    dataVector.push_back( mVoltageValue );
    dataVector.push_back( mPowerValue );
}
#endif

template < typename T >
ScalarUnit TwoPort< T >::GetCurrentValue() const
{
    return mCurrentValue;
}

template < typename T >
ScalarUnit TwoPort< T >::GetVoltageValue() const
{
    return mVoltageValue;
}

template < typename T >
ScalarUnit TwoPort< T >::GetPowerValue() const
{
    return mPowerValue;
}


template < typename T >
inline ScalarUnit ReturnFirstElement( T& mat )
{
    return mat.coeffRef( 0, 0 );
}

#ifdef _EIGEN_
template <>
ScalarUnit ReturnFirstElement( Eigen::SparseMatrix< double, Eigen::RowMajor >& mat );
#endif

template < typename T >
void TwoPort< T >::CalculateCurrentValue()
{
    T m = ( mCurrent * this->mStateSystemGroup->mStateVector );
    this->mCurrentValue = ReturnFirstElement( m );
}

template < typename T >
void TwoPort< T >::CalculateVoltageValue()
{
    T m = ( mVoltage * this->mStateSystemGroup->mStateVector );
    this->mVoltageValue = ReturnFirstElement( m );
}

template < typename T >
TwoPort< T >::TwoPort( const bool observable, DataType dataValues )
    : mDataStruct( dataValues )
    , mID( 0 )
    , mCurrent( T() )
    , mVoltage( T() )
    , mCurrentValue( mDataStruct->mCurrentValue )
    , mVoltageValue( mDataStruct->mVoltageValue )
    , mPowerValue( mDataStruct->mPowerValue )
    , mStateSystemGroup( 0 )
    , mObservable( observable )
{
}

template < typename T >
void TwoPort< T >::UpdateStateSystemGroup()
{
    if ( mID )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "OnlyRootport" );

    this->GetVoltage();
}

template < typename T >
void TwoPort< T >::SetInitialCurrent( ScalarUnit current )
{
    if ( mID )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "OnlyRootport" );
    mObservable = true;    // Das RootElement soll VCP automatisch berechnen und bereitstellen

    T tmpcurrent;
    tmpcurrent.resize( 1, 1 + mStateSystemGroup->GetStateCount() );
    tmpcurrent.setZero();
    tmpcurrent.coeffRef( 0, mStateSystemGroup->GetStateCount() ) = current;
    this->SetCurrent( tmpcurrent );
}

template < typename T >
bool TwoPort< T >::HasSystem() const
{
    return mStateSystemGroup != 0;    // double negation for bool conversion
}

template < typename T >
size_t TwoPort< T >::GetID() const
{
    return mID;
}

template < typename T >
void TwoPort< T >::SetID( size_t iD )
{
    mID = iD;
}

template < typename T >
T* TwoPort< T >::GetVoltage()
{
    return &mVoltage;
}

template < typename T >
const T* TwoPort< T >::GetConstVoltage() const
{
    return static_cast< const T* >( &mVoltage );
}

template < typename T >
bool TwoPort< T >::IsObservable() const
{
    return mObservable;
}

template < typename T >
void TwoPort< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{
    if ( HasSystem() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SystemAlreadyDefined" );
    mStateSystemGroup = stateSystemGroup;
}

template < typename T >
void TwoPort< T >::SetCurrent( const T current )
{
    mCurrent = current;
    mVoltage.resize( mCurrent.rows(), mCurrent.cols() );
    mVoltage.setZero();
}

template < typename T >
const T TwoPort< T >::GetCurrent() const
{
    return mCurrent;
}

template < typename T >
void TwoPort< T >::SetCurrent( const ScalarUnit currentval )
{
    this->mCurrent.coeffRef( 0, this->mCurrent.cols() - 1 ) = currentval;
}

template < typename T >
bool TwoPort< T >::IsCellelement() const
{
    return false;
}

template < typename T >
bool TwoPort< T >::CanHaveChildren() const
{
    return false;
}

template < typename T >
bool TwoPort< T >::HasChildren() const
{
    return false;
}

template < typename T >
const char* TwoPort< T >::GetName() const
{
    return "TwoPort";
}

template < typename T >
void TwoPort< T >::SetResistanceFactor( const double )
{
}

template < typename T >
const boost::shared_ptr< ElectricalDataStruct< ScalarUnit > >& TwoPort< T >::GetElectricalData() const
{
    return mDataStruct;
}

}    // namespace electrical

#endif /* _TWOPORT_ */
