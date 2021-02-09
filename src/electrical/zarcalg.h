/*
 * ZarcAlgalg.h
 *
 *  Created on: 04.12.2013
 *      Author: chris
 */
#ifndef ZARCALG_H_
#define ZARCALG_H_

#include "twoport.h"
#include <boost/shared_ptr.hpp>

// STD
#include <cmath>

#ifdef __EXCEPTIONS__
#include <stdexcept>
#endif /* __EXCEPTIONS__ */

#include "../object/lookup_obj1d.h"
#include "../object/object.h"
#include "../operators/vectorOperator.h"
#include "../state/soc.h"
#include "../state/thermal_state.h"

class TestZarc;
class TestXML;
class TestFactoryLookUpCellElements;

namespace electrical
{

/// This TwoPort describes the behaviour of a ZarcAlg element (3 parallelRC)
template < typename T = myMatrixType >
class ZarcAlg : public TwoPort< T >
{
    friend class ::TestZarc;
    friend class ::TestXML;
    friend class ::TestFactoryLookUpCellElements;

    public:
    ZarcAlg( boost::shared_ptr< object::Object< double > > lookupRelaxationTime,
             boost::shared_ptr< object::Object< double > > lookupOhmicResistance,
             boost::shared_ptr< object::Object< double > > lookupPhi, const double samplingRate = 0.001,
             const bool observable = false,
             typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );    ///< The ZarcAlg element needs a thermal and an electrical state to be created, and does not take care of these. These  are just beeing utelized for the lookups

    virtual ~ZarcAlg(){};

    virtual T* GetVoltage();    ///< Returns the voltage over the ZarcAlg element
    std::vector< double >& GetTau() const;
    virtual void CalculateStateDependentValues();

    virtual const char* GetName() const;
    virtual void SetResistanceFactor( const double factor );

    inline const object::Object< double >* GetRObject() const { return mLookupOhmicResistance.get(); }
    inline const object::Object< double >* GetTauObject() const { return mLookupRelaxationTime.get(); }
    inline const object::Object< double >* GetPhiObject() const { return mLookupPhi.get(); }

    inline const lookup::Lookup1D< double >& GetLookupPhi2TauFactor() const { return mLookupPhi2TauFactor; }
    inline const lookup::Lookup1D< double >& GetLookupPhi2RFactorInnen() const { return mLookupPhi2RFactorInnen; }
    inline const lookup::Lookup1D< double >& GetLookupPhi2RFactorAussen() const { return mLookupPhi2RFactorAussen; }

    private:
    struct RcElements
    {
        double rFactorAussen, rFactorInnen, tauFactor, const_r, const_tau;
    };
    RcElements mCurrentRcElements;

    void InitializeZarc( const double samplingRate );

    void CalculateZarcElements( const double samplingRate );

    boost::shared_ptr< object::Object< double > > mLookupOhmicResistance;
    boost::shared_ptr< object::Object< double > > mLookupRelaxationTime;
    boost::shared_ptr< object::Object< double > > mLookupPhi;

    const lookup::Lookup1D< double > mLookupPhi2TauFactor;
    const lookup::Lookup1D< double > mLookupPhi2RFactorInnen;
    const lookup::Lookup1D< double > mLookupPhi2RFactorAussen;

    std::vector< size_t > mUIDs;
    std::vector< double > mTau;

    static const double mPhi[];
    static const double mTauFactor[];
    static const double mRFactorAussen[];
    static const double mRFactorInnen[];

    size_t mNumberOfElements;
    static const size_t THREE_TAU = 3;
    static const size_t MAX_RC_ELEMENTS = 3;

    double mCapacitorVoltages[MAX_RC_ELEMENTS];

    protected:
};

} /* namespace electrical */

#endif /* ZARCALG_H_ */
