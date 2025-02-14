#ifndef _VOLTAGESOURCEPREISACHDISCRETE_
#define _VOLTAGESOURCEPREISACHDISCRETE_

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../object/Object.h"
#include "../state/soc.h"
#include "electricalelement.h"
#include <vector>

class TestSurfaceSoc;

namespace electrical
{

/// This TwoPort describes a VoltageSource with the hysteresis using the Discrete Preisach model
template < typename T = myMatrixType >
class VoltageSource_PreisachDiscrete : public ElectricalElement< T >
{

    friend class ::TestSurfaceSoc;

    public:
    explicit VoltageSource_PreisachDiscrete(
     boost::shared_ptr< state::Soc >& socState, int n, boost::shared_ptr< object::Object< double > > Weights,
     boost::shared_ptr< object::Object< double > > SwitchUpTH,  boost::shared_ptr< object::Object< double > > SwitchDownTH,
        const bool observable = false, typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );

    explicit VoltageSource_PreisachDiscrete(
     boost::shared_ptr< state::Soc >& socState, int n, object::Object< double >* Weights, 
        object::Object< double >* SwitchUpTH, object::Object< double >* SwitchDownTH, const bool observable = false,
        typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );

    virtual ~VoltageSource_PreisachDiscrete() { delete[] mOperatorsMatrix; };

    virtual T* GetVoltage();

    void TurnOnOperators( double s );

    void TurnOffOperators( double s );

    virtual const char* GetName() const;

    int GetDiscretization() { return mDiscretization; };

    void UpdateOperatorsStatus();    // updates the status of the operator matrix (ON : +1, OFF : -1) 

    private:

    protected:
    int *mOperatorsMatrix; // matrix that describes the status of the Operators (ON : +1, OFF : -1)
    boost::shared_ptr< state::Soc > mSOC;
    double mLastSoc;
    const int mDiscretization;
    boost::shared_ptr< object::Object< double > > mSwitchUpTH;    // matrix that describe the SOC at whcih the operators switch from -1 to 1 (ON : +1, OFF : -1)
    boost::shared_ptr< object::Object< double > > mSwitchDownTH;   // matrix that describe the SOC at whcih the operators switch from 1 to -1 (ON : +1, OFF : -1)
};
}    // namespace electrical
#endif /* _VOLTAGESOURCEPREISACHDISCRETE_ */