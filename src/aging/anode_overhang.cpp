#include "anode_overhang.h"

namespace aging
{
AnodeOverhang::AnodeOverhang( const double agingStepTime, const boost::shared_ptr< object::Object< double > >& voltage, 
                              const boost::shared_ptr< object::Object< double > >& dva,
                              const double& activCoef, const double& tauCoef,
                              const boost::shared_ptr< state::Soc >& socState,
                              const double socWhereOffsetIsZero, bool isEnabled )
    : AgingBase( agingStepTime, isEnabled )
    , mVoltage( voltage )
    , mDVA (dva)
    , mActivCoef( activCoef )
    , mTauCoef( tauCoef )
    , mSocState( socState )
    , mTotalCalculationTime( 0.0 )
{
    if ( this->mIsEnabled )
    {
        this->mAnodeOffset =
         ( socWhereOffsetIsZero - this->mSocState->template GetValue< state::SocGetFormat::PERCENT >() ) /
         100 * this->mSocState->template GetActualCapacity< state::SocGetFormat::AS >();
    }
    else
    {
        this->mAnodeOffset = 0.0;
    }
}

size_t AnodeOverhang::GetType() const { return AgingType::ANODE_OVERHANG; }

const boost::shared_ptr< state::Soc >& AnodeOverhang::GetSoc() const { return this->mSocState; }

void AnodeOverhang::CalculateAging( const TwoportState&, double timestep, double scaleFactor )
{
    if ( this->mIsEnabled && this->mTotalCalculationTime > 0.0 )
    {
        double stepTimeSeconds = timestep * scaleFactor;
        double anodeVoltageSum = 0.0;
        double temperatureSum = 0.0;
        double previousTime = 0.0;
        double dt;
        for ( size_t i = 0; i < mTimeValues.size(); ++i )
        {
            dt = mTimeValues[i] - previousTime;
            anodeVoltageSum += mAnodeVoltageValues[i] * dt;
            temperatureSum += mTemperatureValues[i] * dt;
            previousTime = mTimeValues[i];
        }
        double anodeVoltage = anodeVoltageSum / this->mTotalCalculationTime;
        double temperature = temperatureSum / this->mTotalCalculationTime;
        double tau = this->mTauCoef * std::exp( this->mActivCoef / temperature ) * 3600 * 24;
        
        double overhangVoltage = mVoltage->GetValue();
        
        double capacitance = 1 / ( mDVA->GetValue(this->mSocState->GetValue< state::SocGetFormat::PERCENT >()) 
         * this->mSocState->GetActualCapacity< state::SocGetFormat::AS >() );      
       
        double additionalCharge = capacitance * ( overhangVoltage - anodeVoltage ) * ( 1 - exp( -stepTimeSeconds / tau ) );
        this->mSocState->SetStoredEnergy< state::SocSetFormat::DELTA >( additionalCharge );
        this->mAnodeOffset -= additionalCharge;

        mTimeValues.clear();
        mAnodeVoltageValues.clear();
        mTemperatureValues.clear();
        this->mTotalCalculationTime = 0.0;
    }
}

double AnodeOverhang::CalculateLinearCapacitance()
{
    double saveCharge = this->mSocState->GetValue< state::SocGetFormat::AS >();
    double overhangVoltage = mVoltage->GetValue();

    // Evaluate two additional points on the Anode QOCV for linearisation
    double overhangCap = this->mSocState->GetActualCapacity();
    double chargeDiff = 0.01 * overhangCap;
    double chargePlus = saveCharge + chargeDiff;
    double chargeMinus = saveCharge - chargeDiff;
    
    this->mSocState->SetStoredEnergy< state::SocSetFormat::ABSOLUT >( chargePlus );
    double overhangVoltagePlus = mVoltage->GetValue();
    this->mSocState->SetStoredEnergy< state::SocSetFormat::ABSOLUT >( chargeMinus );
    double overhangVoltageMinus = mVoltage->GetValue();

    this->mSocState->SetStoredEnergy< state::SocSetFormat::ABSOLUT >( saveCharge );

    double calculatedCapacity = (2 * chargeDiff) / (overhangVoltagePlus - overhangVoltageMinus);
    
    if (calculatedCapacity > (saveCharge / overhangVoltage) )
    {   
        calculatedCapacity = saveCharge / overhangVoltage;
    }

    return calculatedCapacity;
}


void AnodeOverhang::CollectData( const TwoportState& twoportState, const TwoportState&, double timestep )
{
    if ( this->mIsEnabled )
    {
        this->mTotalCalculationTime += timestep;
        mTimeValues.push_back( mTotalCalculationTime );
        mAnodeVoltageValues.push_back( -twoportState.mElectricalData->mVoltageValue );
        mTemperatureValues.push_back( twoportState.mThermalState->GetValue< TemperatureGetFormat::KELVIN >() );
    }
}

void AnodeOverhang::ResetToPointInTime( double time )
{
    auto firstDelete = std::upper_bound( mTimeValues.begin(), mTimeValues.end(), time );
    std::ptrdiff_t firstDeleteIndex = firstDelete - mTimeValues.begin();
    mTimeValues.erase( firstDelete, mTimeValues.end() );
    mAnodeVoltageValues.erase( mAnodeVoltageValues.begin() + firstDeleteIndex, mAnodeVoltageValues.end() );
    mTemperatureValues.erase( mTemperatureValues.begin() + firstDeleteIndex, mTemperatureValues.end() );

    mTotalCalculationTime = time;
}

}    // namespace aging
