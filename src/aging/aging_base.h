// aging_base.h
// Autor: Christopher Abele, Andreas Hase

#ifndef _AGINGBASE_
#define _AGINGBASE_

#include <boost/shared_ptr.hpp>

#include <cstddef>
#include <string>

#include "../electrical/electrical_data_struct.h"
#include "../state/soc.h"
#include "../state/thermal_state.h"
#include "../visitor/visitor.h"

namespace aging
{

/// For type assignment of the derived classes
enum AgingType
{
    CALENDARIAN,
    CYCLICAL,
    ANODE_OVERHANG,
    GENERIC,
    LAM,
    LLI
};

struct TwoportState
{
    TwoportState( const boost::shared_ptr< ElectricalDataStruct< double > > &electricalData = nullptr,
                  const boost::shared_ptr< state::Soc > &socState = nullptr,
                  const boost::shared_ptr< state::ThermalState< double > > &thermalState = nullptr )
        : mElectricalData( electricalData )
        , mSocState( socState )
        , mThermalState( thermalState )
        , mCellAge( 0.0 )
        , mChargeThroughput( 0.0 )
    {
    }

    boost::shared_ptr< ElectricalDataStruct< double > > mElectricalData;
    boost::shared_ptr< state::Soc > mSocState;
    boost::shared_ptr< state::ThermalState< double > > mThermalState;
    double mCellAge;
    double mChargeThroughput;
};

/// Base class for all aging types
class AgingBase
{
    public:
    /// Constructor
    AgingBase( const double agingStepTime, const bool isEnabled );

    /// Returns the type of the derived class "AgingType::CALENDARIAN" or "AgingType::CYCLICAL"
    virtual size_t GetType() const = 0;

    virtual void CalculateAging( const TwoportState &twoportState, double timestep, double scaleFactor ) = 0;
    virtual void CollectData( const TwoportState &twoportState, const TwoportState &cellState, double timestep ) = 0;
    /// delete data that was collected after the given time
    virtual void ResetToPointInTime( double time ) = 0;
    virtual double GetCapacityFactor() = 0;
    virtual double GetSocOffset() = 0;    // in As
    virtual double GetResistanceFactor() = 0;

    virtual void AcceptVisitor( visitor::Visitor< AgingBase > &visitor ) = 0;

    protected:
    const double mAgingStepTime;    // [days]
    const bool mIsEnabled;
};

}    // namespace aging

#endif    // _AGINGBASE_
