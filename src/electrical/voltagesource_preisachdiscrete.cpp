#include "voltagesource_preisachdiscrete.h"

// TODO: change mSOC to mSOC at time t
namespace electrical
{

template < typename T >
VoltageSource_PreisachDiscrete< T >::VoltageSource_PreisachDiscrete( boost::shared_ptr< state::Soc >& socState, int Discretization,
                                                                     boost::shared_ptr< object::Object< double > > Weights,
                                                                     boost::shared_ptr< object::Object< double > > SwitchUpTH,
                                                                     boost::shared_ptr< object::Object< double > > SwitchDownTH,
                                                                     const bool observable, typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( Weights, observable, dataValues )
    , mSOC( socState )
    , mLastSoc( socState->GetValue() )
    , mDiscretization( Discretization )
    , mSwitchUpTH( SwitchUpTH )
    , mSwitchDownTH( SwitchDownTH )
{
    int size_OperatorsMatrix = Discretization * Discretization;
    mOperatorsMatrix = new int[size_OperatorsMatrix];
    double mcurrentSOC = socState->GetValue();
    for ( int i = 0; i < mDiscretization; i++ )
        for ( int j = 0; j < mDiscretization; j++ )
        {
            mOperatorsMatrix[i * mDiscretization + j] = 1;
            this->mVoltageValue = this->mVoltageValue + ( *mObject )( j, i );
        }
    if ( mcurrentSOC < 100 )
        TurnOffOperators( mcurrentSOC );
}

template < typename T >
VoltageSource_PreisachDiscrete< T >::VoltageSource_PreisachDiscrete( boost::shared_ptr< state::Soc >& socState, int n,
                                                                     object::Object< double >* obj1, object::Object< double >* obj2,
                                                                     object::Object< double >* obj3, const bool observable,
                                                                   typename TwoPort< T >::DataType dataValues )
    : VoltageSource_PreisachDiscrete< T >( socState, n, boost::shared_ptr< object::Object< double > >( obj1 ),
                                           boost::shared_ptr< object::Object< double > >( obj2 ), boost::shared_ptr< object::Object< double > >( obj3 ),
                                           observable, dataValues )
{
}

template < typename T >
T* VoltageSource_PreisachDiscrete< T >::GetVoltage()
{
    UpdateOperatorsStatus();
    this->mVoltage.coeffRef( 0, this->mCurrent.cols() - 1 ) = this->mVoltageValue;
    return TwoPort< T >::GetVoltage();
}


template < typename T >
void VoltageSource_PreisachDiscrete< T >::TurnOnOperators( double s )
{
    double i = ( *mSwitchUpTH )( s );
    int index = (int)i;
    for ( int i = 0; i < index ; ++i )
        for ( int j = 0; j < mDiscretization; ++j )
            if ( mOperatorsMatrix[i * mDiscretization + j] == 0 )
            {
                mOperatorsMatrix[i * mDiscretization + j] = 1;
                this->mVoltageValue = this->mVoltageValue + (*mObject)( j, i );
            }
}

template < typename T >
void VoltageSource_PreisachDiscrete< T >::TurnOffOperators( double s )
{
    double i = ( *mSwitchDownTH )( -s );
    double b = 0;
    int index = (int) i;
    for ( int j = mDiscretization - index; j < mDiscretization; ++j )
        for ( int i = 0; i < mDiscretization; ++i )
            if ( ( mOperatorsMatrix[i * mDiscretization + j] == 1 ) && ( j != 0 ) )
            {
                mOperatorsMatrix[i * mDiscretization + j] = 0;
                b = ( *mObject )( j, i );
                this->mVoltageValue = this->mVoltageValue -b  ;
            }
}

template < typename T >
void VoltageSource_PreisachDiscrete< T >::UpdateOperatorsStatus()
{
    double nowSoc = mSOC->GetValue();
    if ( nowSoc < 0 )
        nowSoc = 0;
    if ( nowSoc > 100 )
        nowSoc = 100;
    if ( nowSoc > mLastSoc )
        TurnOnOperators( nowSoc );
    else if ( nowSoc < mLastSoc )
        TurnOffOperators( nowSoc );
    mLastSoc = nowSoc;

}

template < typename T >
const char* VoltageSource_PreisachDiscrete< T >::GetName() const
{
    return "VoltageSource_PreisachDiscrete";
}

}    // namespace electrical

template class electrical::VoltageSource_PreisachDiscrete< myMatrixType >;