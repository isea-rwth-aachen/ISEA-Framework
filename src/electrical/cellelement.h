/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : cellelement.h
* Creation Date : 10-11-2012
* Last Modified : Di 08 Mär 2016 15:48:34 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef _CELLELEMENT_
#define _CELLELEMENT_

// BOOST
#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>


#include "anode_element.h"
#include "cathode_element.h"
#include "paralleltwoport.h"
#include "twoport_with_state.h"
#include "voltagesource.h"

#include "../state/soc.h"
#include "../state/surface_soc.h"
#include "../state/thermal_state.h"

#include "../object/lookup_obj1d_with_state.h"
#include "../object/lookup_obj2d_with_state.h"

class TestElectricalElement;    // friend
namespace electrical
{

enum CellConfiguration
{
    FULLCELL,
    HALFCELL
};

/// Basic behaviour of a cellelement with Temperature, Soc and an electrical circuit which is represented through a
/// serial TwoPort with an Soc
template < typename T = myMatrixType >
class Cellelement : public TwoPortWithState< T >
{
    friend class ::TestElectricalElement;

    public:
    explicit Cellelement(
     size_t cellNumber, const boost::shared_ptr< state::ThermalState< double > >& thermalState,
     const boost::shared_ptr< state::Soc >& socState, const bool observable = false,
     typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ),
     boost::shared_ptr< object::Object< double > > reversibleHeat = boost::shared_ptr< object::Object< double > >( 0 ) );

    explicit Cellelement(
     const boost::shared_ptr< state::ThermalState< double > >& thermalState,
     const boost::shared_ptr< state::Soc >& socState, const bool observable = false,
     typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ),
     boost::shared_ptr< object::Object< double > > reversibleHeat = boost::shared_ptr< object::Object< double > >( 0 ) );

    virtual ~Cellelement(){};

    virtual bool IsCellelement() const;    ///< Is this element a cellelement?
    virtual void SetCurrentFromActiveBalancer( ScalarUnit current );    ///< This function gives the chance to actively balance a battery with a current

    double GetSocStateValue() const;
    double GetSurfaceSocStateValue() const;

    template < state::SocGetFormat FormatT = state::SocGetFormat::AH >
    double GetCapacity() const;

#ifndef _SYMBOLIC_
    virtual void LoadInternalData( std::vector< double >& dataVector );
    virtual void SaveInternalData( std::vector< double >& dataVector );
#endif

    virtual void CalculateStateDependentValues();
    ScalarUnit CalculateReversibleHeat() const;

    virtual bool HasSurfaceSoc() const;

    virtual void SetSurfaceSOC( boost::shared_ptr< state::SurfaceSoc > surfaceSoc );

    virtual const char* GetName() const;

    virtual size_t GetConfigurationType() const;

    void UpdateThermalStatePowerDissipation( double time );    ///<Stores power and time of this cell elements into its thermal state
    void UpdateThermalStatePowerDissipation();    ///<Stores power of this cell elements into its thermal state
    void ResetThermalStatePowerDissipation();     ///<Resets power dissipation data of thermal state

    void AddElectrodeElement( const boost::shared_ptr< ElectrodeElement< T > >& electrodeElement );

    void SetElectrodes( const boost::shared_ptr< ParallelTwoPort< T > >& anode,
                        const boost::shared_ptr< ParallelTwoPort< T > >& cathode );

    const std::vector< boost::shared_ptr< AnodeElement< T > > >& GetAnodeElements() const;
    const std::vector< boost::shared_ptr< CathodeElement< T > > >& GetCathodeElements() const;

    const boost::shared_ptr< ParallelTwoPort< T > >& GetAnode() const;
    const boost::shared_ptr< ParallelTwoPort< T > >& GetCathode() const;

    ScalarUnit GetOcvVoltageValue() const;

    ScalarUnit GetAnodeVoltageValue() const;
    ScalarUnit GetAnodePotentialValue() const;

    ScalarUnit GetCathodeVoltageValue() const;
    ScalarUnit GetCathodePotentialValue() const;

    std::pair< ScalarUnit, ScalarUnit > GetVoltageRange();
    /// Calculates the voltages at 0% and 100% SOC
    void CalculateVoltageRange();
    /// Resets the cell SOC to its initial value and adjusts the electrode SOCs so that the balancing stays the same
    void ResetSocValue();

    bool HasReversibleHeat() const;
    bool HasHalfcells() const;

    const size_t mCellNumber;

    private:
    bool mSurfaceSocSet;

    boost::shared_ptr< state::SurfaceSoc > mSurfaceSoc;

    boost::shared_ptr< ParallelTwoPort< T > > mAnode;
    boost::shared_ptr< ParallelTwoPort< T > > mCathode;

    size_t mConfigurationType;

    std::vector< boost::shared_ptr< AnodeElement< T > > > mAnodeElements;
    std::vector< boost::shared_ptr< CathodeElement< T > > > mCathodeElements;

    boost::shared_ptr< object::Object< double > > mReversibleHeat;

    ScalarUnit mMinimumVoltage;
    ScalarUnit mMaximumVoltage;
    bool mHasVoltageRange;
};

template < typename T >
template < state::SocGetFormat FormatT >
double Cellelement< T >::GetCapacity() const
{
    return this->mSoc->template GetActualCapacity< FormatT >();
}

}    // namespace electrical

#endif /* _CELLELEMENT_ */
