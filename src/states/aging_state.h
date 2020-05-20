#ifndef _AGING_STATE_
#define _AGING_STATE_

#include "../aging/aging_base.h"
#include "../misc/matrixInclude.h"
#include "state.h"
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace electrical
{
template < typename T >
class Cellelement;
}

namespace state
{

enum class AgingStateSetFormat
{
    ABSOLUT,
    DELTA,
    FACTOR
};

/// Manages all aging effect of a CellElement and stores the aging parameters
class AgingState : public State
{
    public:
    AgingState( double cellAge = 0, double chargeThroughput = 0 );

    virtual double GetCellAge() const;
    virtual double GetChargeThroughput() const;
    virtual double GetValue() const;

    virtual double GetCapacityFactor() const;
    virtual double GetSocOffset() const;
    virtual double GetResistanceFactor() const;

    virtual void SetCellAge( const double &value, AgingStateSetFormat format = AgingStateSetFormat::ABSOLUT );
    virtual void SetChargeThroughput( const double &value, AgingStateSetFormat format = AgingStateSetFormat::ABSOLUT );
    virtual void SetCapacityFactor( const double &value, AgingStateSetFormat format = AgingStateSetFormat::ABSOLUT );
    virtual void SetSocOffset( const double &value, AgingStateSetFormat format = AgingStateSetFormat::ABSOLUT );
    virtual void SetResistanceFactor( const double &value, AgingStateSetFormat format = AgingStateSetFormat::ABSOLUT );

    double mCapacityFactor;
    double mSocOffset;
    double mResistanceFactor;

    double mCellAge;
    double mChargeThroughput;

    protected:
    void ChangeInternalValue( double &value, const double &newValue, AgingStateSetFormat format );
};
}    // namespace state

#endif
