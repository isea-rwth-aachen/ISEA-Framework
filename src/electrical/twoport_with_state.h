#ifndef _TWOPORT_WITH_STATE_
#define _TWOPORT_WITH_STATE_

#include "../state/soc.h"
#include "../state/thermal_state.h"
#include "../state/crate.h"
#include "../state/cdirection.h"
#include "../state/lhd.h"
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
    TwoPortWithState( const boost::shared_ptr< state::Soc >& socState,
                      const boost::shared_ptr< state::ThermalState< double > >& thermalState,
                      const boost::shared_ptr< state::CRateState< double > >& crateState, const bool observable = false,
                      typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    TwoPortWithState( const boost::shared_ptr< state::Soc >& socState,
                      const boost::shared_ptr< state::ThermalState< double > >& thermalState,
                      const boost::shared_ptr< state::CDirection< double > >& cdirechtionState, const bool observable = false,
                      typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    TwoPortWithState( const boost::shared_ptr< state::Soc >& socState,
                      const boost::shared_ptr< state::ThermalState< double > >& thermalState,
                      const boost::shared_ptr< state::CRateState< double > >& crateState,
                      const boost::shared_ptr< state::LHDState< double > >& lhdState, const bool observable = false,
                      typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    TwoPortWithState( const boost::shared_ptr< state::Soc >& socState,
                      const boost::shared_ptr< state::ThermalState< double > >& thermalState,
                      const boost::shared_ptr< state::CDirection< double > >& cdirechtionState,
                      const boost::shared_ptr< state::LHDState< double > >& lhdState, const bool observable = false,
                      typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );

    virtual ~TwoPortWithState(){};

    virtual const char* GetName() const;

    virtual double GetSocValue() const;

    virtual double GetCrateValue() const;

    virtual double GetCdirectionValue() const;

    virtual double GetLHDValue() const;

    virtual void CalculateStateDependentValues();

    virtual double GetTotalCapacity() const
    {
        return this->mSoc->template GetActualCapacity< state::SocGetFormat::AH >();
    }

    const boost::shared_ptr< state::Soc >& GetSoc() const { return this->mSoc; }
    const boost::shared_ptr< state::ThermalState< double > >& GetThermalState() const { return mThermalState; }
    const boost::shared_ptr< state::CRateState< double > >& GetCrateState() const { return mCrate; }
    const boost::shared_ptr< state::CDirection< double > >& GetCdirectionState() const { return mCdirection; }
    const boost::shared_ptr< state::LHDState< double > >& GetLHDState() const { return mLHD; }

    protected:
    const boost::shared_ptr< state::Soc > mSoc;
    const boost::shared_ptr< state::ThermalState< double > > mThermalState;
    const boost::shared_ptr< state::CRateState< double > > mCrate;
    const boost::shared_ptr< state::CDirection< double > > mCdirection;
    const boost::shared_ptr< state::LHDState< double > > mLHD;

    bool CrateDefined;
    bool CdirectionDefined;
    bool LHDDefined;
};

}    // namespace electrical
#endif
