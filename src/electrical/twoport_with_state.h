#ifndef _TWOPORT_WITH_STATE_
#define _TWOPORT_WITH_STATE_

#include "../state/soc.h"
#include "../state/thermal_state.h"
#include "serialtwoport.h"

extern template class electrical::SerialTwoPort< myMatrixType >;

namespace electrical
{
template < typename T = myMatrixType >
class TwoPortWithState : public SerialTwoPort< T >
{
    friend class ::TestTwoPortsWithChildren;

    public:
    TwoPortWithState( const boost::shared_ptr< state::Soc >& socState,
                      const boost::shared_ptr< state::ThermalState< double > >& thermalState, const bool observable = false,
                      typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );

    virtual ~TwoPortWithState(){};

    virtual const char* GetName() const;

    virtual double GetSocValue() const;

    virtual void CalculateStateDependentValues();

    virtual double GetTotalCapacity() const
    {
        return this->mSoc->template GetActualCapacity< state::SocGetFormat::AH >();
    }

    const boost::shared_ptr< state::Soc >& GetSoc() const { return this->mSoc; }
    const boost::shared_ptr< state::ThermalState< double > >& GetThermalState() const { return mThermalState; }

    protected:
    const boost::shared_ptr< state::Soc > mSoc;
    const boost::shared_ptr< state::ThermalState< double > > mThermalState;
};

}    // namespace electrical
#endif
