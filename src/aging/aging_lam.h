#ifndef _AGING_LAM_
#define _AGING_LAM_

#include "../object/object.h"
#include "aging_base.h"

namespace factory
{
template < typename T >
class AgingClassWrapper;
}

namespace aging
{

class AgingLAM : public AgingBase
{
    friend factory::AgingClassWrapper< AgingLAM >;

    public:
    AgingLAM( const double agingStepTime, const bool isEnabled, const boost::shared_ptr< object::Object< double > > &value );

    virtual size_t GetType() const;

    virtual void CalculateAging( const TwoportState &twoportState, double timestep, double scaleFactor );
    virtual void CollectData( const TwoportState &twoportState, const TwoportState &cellState, double timestep );
    virtual void ResetToPointInTime( double time );
    virtual double GetCapacityFactor();
    virtual double GetSocOffset();    // in As
    virtual double GetResistanceFactor();

    virtual void AcceptVisitor( visitor::Visitor< AgingBase > &visitor );

    private:
    const boost::shared_ptr< object::Object< double > > mValue;
    double mTotalLam;
    double mCycles;
};

}    // namespace aging

#endif