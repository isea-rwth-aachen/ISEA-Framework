/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : zarc.h
* Creation Date : 11-11-2012
* Last Modified : Do 10 Mär 2016 13:01:10 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ZARC_
#define _ZARC_

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

// friend
class TestZarc;
class TestXML;
class TestFactoryLookUpCellElements;
class TestElectricalFactory;

namespace electrical
{

/// This TwoPort describes the behaviour of a zarc element (3 parallelRC)
template < typename T = myMatrixType >
class Zarc : public TwoPort< T >
{
    friend class ::TestZarc;
    friend class ::TestXML;
    friend class ::TestFactoryLookUpCellElements;
    friend class ::TestElectricalFactory;

    public:
    explicit Zarc( boost::shared_ptr< object::Object< double > > lookupRelaxationTime,
                   boost::shared_ptr< object::Object< double > > lookupOhmicResistance,
                   boost::shared_ptr< object::Object< double > > lookupPhi, const double samplingRate = 0.001,
                   const bool observable = false,
                   typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );    ///< The zarc element needs a thermal and an electrical state to be created, and does not take care of these. These  are just beeing utelized for the lookups

    virtual ~Zarc(){};

    virtual void SetCurrent( const T current );    ///< Sets the current

    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );
    virtual T* GetVoltage();    ///< Returns the voltage over the Zarc element
    virtual void CalculateStateDependentValues();
    virtual void SetResistanceFactor( const double factor );

    const std::vector< ScalarUnit >& GetRValues() const;
    const std::vector< ScalarUnit >& GetCValues() const;

    const std::vector< double >& GetTau() const;

    virtual const char* GetName() const;
    size_t GetNumberOfElements() const;

    const std::vector< ScalarUnit >& GetVoltageValues() const;

    inline const object::Object< double >* GetRObject() const { return mLookupOhmicResistance.get(); }
    inline const object::Object< double >* GetTauObject() const { return mLookupRelaxationTime.get(); }
    inline const object::Object< double >* GetPhiObject() const { return mLookupPhi.get(); }

    inline const lookup::Lookup1D< double >& GetLookupPhi2TauFactor() const { return mLookupPhi2TauFactor; }
    inline const lookup::Lookup1D< double >& GetLookupPhi2RFactorInnen() const { return mLookupPhi2RFactorInnen; }
    inline const lookup::Lookup1D< double >& GetLookupPhi2RFactorAussen() const { return mLookupPhi2RFactorAussen; }

    private:
    struct RcElements
    {
        ScalarUnit rFactorAussen, rFactorInnen, tauFactor, const_r, const_tau;
    };
    RcElements mCurrentRcElements;
    typedef void ( Zarc::*rcImplementation )( const RcElements& rc, T& voltageEquation );
    typedef void ( Zarc::*ohmicResistanceFunction )();

    void InitializeZarc( const double samplingRate );
    void FirstRcVoltage( const RcElements& rc, T& voltageEquation );
    void SecondRcVoltage( const RcElements& rc, T& voltageEquation );
    void ThirdRcVoltage( const RcElements& rc, T& voltageEquation );

    void AddOhmicResistanceToVoltage1R();
    void AddOhmicResistanceToVoltage2R();
    void AddOhmicResistanceToVoltage3R();

    void SetVoltage();
    void CalculateZarcElements( const double samplingRate );

    T mZarcVoltageValue;
    T mVoltageEquation;
    std::vector< ScalarUnit > mRValues;
    std::vector< ScalarUnit > mCValues;

    void CalculateLookupValues();
    mutable std::vector< ScalarUnit > mVoltageValues;    // ugly with mutable

    boost::shared_ptr< object::Object< double > > mLookupOhmicResistance;
    boost::shared_ptr< object::Object< double > > mLookupRelaxationTime;
    boost::shared_ptr< object::Object< double > > mLookupPhi;

    const lookup::Lookup1D< double > mLookupPhi2TauFactor;
    const lookup::Lookup1D< double > mLookupPhi2RFactorInnen;
    const lookup::Lookup1D< double > mLookupPhi2RFactorAussen;

    std::vector< size_t > mUIDs;
    std::vector< rcImplementation > mFunctionVector;
    std::vector< ohmicResistanceFunction > mOhmicFunctionVector;
    std::vector< double > mTau;

    static const double mPhi[];
    static const double mTauFactor[];
    static const double mRFactorAussen[];
    static const double mRFactorInnen[];

    size_t mNumberOfElements;
    static const size_t THREE_TAU = 3;
    static const size_t MAX_RC_ELEMENTS = 3;

    protected:
};

}    // namespace electrical
#endif /* _ZARC_ */
