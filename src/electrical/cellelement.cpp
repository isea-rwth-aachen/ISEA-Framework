#include "cellelement.h"

namespace electrical
{

template < typename T >
Cellelement< T >::Cellelement( size_t cellNumber, const boost::shared_ptr< state::ThermalState< double > >& thermalState,
                               const boost::shared_ptr< state::Soc >& socState, const bool observable,
                               typename TwoPort< T >::DataType dataValues, boost::shared_ptr< object::Object< double > > reversibleHeat )
    : TwoPortWithState< T >( socState, thermalState, observable, dataValues )
    , mSurfaceSocSet( false )
    , mAnode( nullptr )
    , mCathode( nullptr )
    , mConfigurationType( CellConfiguration::FULLCELL )
    , mReversibleHeat( reversibleHeat )
    , mCellNumber( cellNumber )
    , mHasVoltageRange( false )
{
    if ( !this->mThermalState )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AddNullptrToObjectException",
                                             "mThermalState", "Cellelement" );

    if ( !this->mSoc )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AddNullptrToObjectException", "mSoc",
                                             "Cellelement" );
}

template < typename T >
Cellelement< T >::Cellelement( const boost::shared_ptr< state::ThermalState< double > >& thermalState,
                               const boost::shared_ptr< state::Soc >& socState, const bool observable,
                               typename TwoPort< T >::DataType dataValues, boost::shared_ptr< object::Object< double > > reversibleHeat )
    : Cellelement< T >( 0, thermalState, socState, observable, dataValues, reversibleHeat )
{
}

#ifndef _SYMBOLIC_
template < typename T >
void Cellelement< T >::LoadInternalData( std::vector< double >& dataVector )
{
    this->mSoc->template SetStoredEnergy< state::SocSetFormat::ABSOLUT >( dataVector.back() );
    dataVector.pop_back();

    TwoPort< T >::LoadInternalData( dataVector );
}

template < typename T >
void Cellelement< T >::SaveInternalData( std::vector< double >& dataVector )
{
    TwoPort< T >::SaveInternalData( dataVector );
    dataVector.push_back( this->mSoc->GetValue() );
}
#endif

template < typename T >
void Cellelement< T >::CalculateStateDependentValues()
{
    TwoPortWithState< T >::CalculateStateDependentValues();
    this->mPowerValue += CalculateReversibleHeat();

#ifndef _SYMBOLIC_
    if ( mSurfaceSocSet )
        mSurfaceSoc->UpdateLookUp();
#endif
}

template < typename T >
bool Cellelement< T >::IsCellelement() const
{
    return true;
}


template < typename T >
double Cellelement< T >::GetSurfaceSocStateValue() const
{
    return mSurfaceSoc->GetValue();
}

template < typename T >
bool Cellelement< T >::HasSurfaceSoc() const
{
    return mSurfaceSocSet;
}

template < typename T >
void Cellelement< T >::SetSurfaceSOC( boost::shared_ptr< state::SurfaceSoc > surfaceSoc )
{
    mSurfaceSoc = surfaceSoc;
    mSurfaceSocSet = true;
}

template < typename T >
void Cellelement< T >::SetCurrentFromActiveBalancer( ScalarUnit currentval )
{
    currentval += this->mCurrent.coeffRef( 0, this->mCurrent.cols() - 1 );
    this->SetCurrent( currentval );
}

template < typename T >
double Cellelement< T >::GetSocStateValue() const
{
    return this->GetSocValue();
}

template < typename T >
const char* Cellelement< T >::GetName() const
{
    return "Cellelement";
}

template < typename T >
size_t Cellelement< T >::GetConfigurationType() const
{
    return mConfigurationType;
}

template < typename T >
bool Cellelement< T >::HasReversibleHeat() const
{
    return !( !mReversibleHeat );
}

template < typename T >
ScalarUnit Cellelement< T >::CalculateReversibleHeat() const
{

    if ( !HasReversibleHeat() )
    {
        return static_cast< ScalarUnit >( 0 );
    }
    return ( this->mThermalState->template GetValue< TemperatureGetFormat::KELVIN >() * this->mCurrentValue *
             mReversibleHeat->GetValue() );
}

template < typename T >
void Cellelement< T >::UpdateThermalStatePowerDissipation( double time )
{
#ifndef _SYMBOLIC_
    this->mThermalState->AddPowerDissipation( this->GetPowerValue(), time );
#endif
}

template < typename T >
void Cellelement< T >::UpdateThermalStatePowerDissipation()
{
#ifndef _SYMBOLIC_
    this->mThermalState->SetFixedPowerDissipation( this->GetPowerValue() );
#endif
}

template < typename T >
void Cellelement< T >::ResetThermalStatePowerDissipation()
{
#ifndef _SYMBOLIC_
    this->mThermalState->ResetPowerDissipation();
#endif
}

template < typename T >
void Cellelement< T >::AddElectrodeElement( const boost::shared_ptr< ElectrodeElement< T > >& electrodeElement )
{
    if ( mConfigurationType != CellConfiguration::HALFCELL )
        return;

    if ( electrodeElement )
    {
        if ( electrodeElement->GetElementType() == ElementType::ANODE_ELEMENT )
        {
            boost::shared_ptr< AnodeElement< T > > anodeElement =
             boost::static_pointer_cast< AnodeElement< T > >( electrodeElement );
            mAnodeElements.push_back( anodeElement );
            return;
        }

        if ( electrodeElement->GetElementType() == ElementType::CATHODE_ELEMENT )
        {
            boost::shared_ptr< CathodeElement< T > > cathodeElement =
             boost::static_pointer_cast< CathodeElement< T > >( electrodeElement );
            mCathodeElements.push_back( cathodeElement );
            return;
        }
    }

    else
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AddNullptrToObjectException",
                                             "mAnodeElements/mCathodeElements", "Cellelement" );
}

template < typename T >
void Cellelement< T >::SetElectrodes( const boost::shared_ptr< ParallelTwoPort< T > >& anode,
                                      const boost::shared_ptr< ParallelTwoPort< T > >& cathode )
{
    if ( !anode )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AddNullptrToObjectException", "mAnode",
                                             "Cellelement" );

    if ( !cathode )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AddNullptrToObjectException",
                                             "mCathode", "Cellelement" );

    mAnode = anode;
    mCathode = cathode;

    mConfigurationType = CellConfiguration::HALFCELL;
}

template < typename T >
const std::vector< boost::shared_ptr< AnodeElement< T > > >& Cellelement< T >::GetAnodeElements() const
{
    return mAnodeElements;
}

template < typename T >
const std::vector< boost::shared_ptr< CathodeElement< T > > >& Cellelement< T >::GetCathodeElements() const
{
    return mCathodeElements;
}

template < typename T >
const boost::shared_ptr< ParallelTwoPort< T > >& Cellelement< T >::GetAnode() const
{
    if ( mConfigurationType != CellConfiguration::HALFCELL )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException", "mAnode",
                                             "Cellelement" );

    return mAnode;
}

template < typename T >
const boost::shared_ptr< ParallelTwoPort< T > >& Cellelement< T >::GetCathode() const
{
    if ( mConfigurationType != CellConfiguration::HALFCELL )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException", "mCathode",
                                             "Cellelement" );

    return mCathode;
}

template < typename T >
ScalarUnit Cellelement< T >::GetOcvVoltageValue() const
{
    return ( GetCathodePotentialValue() + GetAnodePotentialValue() );
}

template < typename T >
ScalarUnit Cellelement< T >::GetAnodeVoltageValue() const
{
    if ( mConfigurationType != CellConfiguration::HALFCELL )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException", "mAnode",
                                             "Cellelement" );

    return mAnode->GetVoltageValue();
}

template < typename T >
ScalarUnit Cellelement< T >::GetAnodePotentialValue() const
{
    if ( mAnodeElements.empty() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException",
                                             "mAnodeElements.front()", "Cellelement" );

    return mAnodeElements.front()->GetElectrodePotentialValue();
}

template < typename T >
ScalarUnit Cellelement< T >::GetCathodeVoltageValue() const
{
    if ( mConfigurationType != CellConfiguration::HALFCELL )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException", "mCathode",
                                             "Cellelement" );

    return mCathode->GetVoltageValue();
}

template < typename T >
ScalarUnit Cellelement< T >::GetCathodePotentialValue() const
{
    if ( mCathodeElements.empty() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException",
                                             "mCathodeElements.front()", "Cellelement" );

    return mCathodeElements.front()->GetElectrodePotentialValue();
}

template < typename T >
void Cellelement< T >::CalculateVoltageRange()
{
    // does not work with multiple electrode particles

    boost::shared_ptr< state::Soc > anodeSoc = this->mAnodeElements.front()->GetSoc();
    boost::shared_ptr< state::Soc > cathodeSoc = this->mCathodeElements.front()->GetSoc();

    double storedChargeAnode = anodeSoc->GetValue< state::SocGetFormat::AS >();
    double storedChargeCathode = cathodeSoc->GetValue< state::SocGetFormat::AS >();

    double capacityAnode = anodeSoc->GetActualCapacity< state::SocGetFormat::AS >();
    double capacityCathode = cathodeSoc->GetActualCapacity< state::SocGetFormat::AS >();

    // maximum charge that can be substracted without either electrode SOC going below 0
    double chargeUntilMinimum = std::min( storedChargeAnode, storedChargeCathode );
    anodeSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( -chargeUntilMinimum );
    cathodeSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( -chargeUntilMinimum );
    this->mMinimumVoltage = this->GetOcvVoltageValue();

    double chargeUntilMaximum = std::min( capacityAnode - storedChargeAnode, capacityCathode - storedChargeCathode );
    anodeSoc->SetStoredEnergy< state::SocSetFormat::ABSOLUT >( storedChargeAnode + chargeUntilMaximum );
    cathodeSoc->SetStoredEnergy< state::SocSetFormat::ABSOLUT >( storedChargeCathode + chargeUntilMaximum );
    this->mMaximumVoltage = this->GetOcvVoltageValue();

    // restore electrode SOCs and system state
    anodeSoc->SetStoredEnergy< state::SocSetFormat::ABSOLUT >( storedChargeAnode );
    cathodeSoc->SetStoredEnergy< state::SocSetFormat::ABSOLUT >( storedChargeCathode );
    this->mHasVoltageRange = true;
}

template < typename T >
void Cellelement< T >::ResetSocValue()
{
    if ( mConfigurationType == CellConfiguration::HALFCELL )
    {
        double chargeBefore = this->mSoc->template GetValue< state::SocGetFormat::AS >();
        this->mSoc->ResetToInitialValue();
        double chargeDifference = this->mSoc->template GetValue< state::SocGetFormat::AS >() - chargeBefore;
        // to keep the balancing intact, the same charge difference has to be applied to all electrode SOCs
        double anodeChargeDifference = chargeDifference / mAnodeElements.size();
        double cathodeChargeDifference = chargeDifference / mCathodeElements.size();
        BOOST_FOREACH ( boost::shared_ptr< AnodeElement< T > > element, mAnodeElements )
        {
            element->GetSoc()->template SetStoredEnergy< state::SocSetFormat::DELTA >( anodeChargeDifference );
        }

        BOOST_FOREACH ( boost::shared_ptr< CathodeElement< T > > element, mCathodeElements )
        {
            element->GetSoc()->template SetStoredEnergy< state::SocSetFormat::DELTA >( cathodeChargeDifference );
        }
    }
    else
    {
        this->mSoc->ResetToInitialValue();
    }
}

template < typename T >
bool Cellelement< T >::HasHalfcells() const
{
    return this->mConfigurationType == CellConfiguration::HALFCELL;
}

template < typename T >
std::pair< ScalarUnit, ScalarUnit > Cellelement< T >::GetVoltageRange()
{
    if ( !mHasVoltageRange )
        CalculateVoltageRange();
    return { mMinimumVoltage, mMaximumVoltage };
}

}    // namespace electrical

template class electrical::Cellelement< myMatrixType >;
