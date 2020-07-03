#ifndef _MATLABFILTERAGING_
#define _MATLABFILTERAGING_

#ifdef BUILD_AGING_SIMULATION
#ifndef __NO_STRING__
#ifndef _DS1006

#include "../../aging/aging_twoport.h"
#include "../../aging/anode_overhang.h"
#include "../../aging/calendarian_aging.h"
#include "../../aging/cyclical_aging.h"
#include "../../aging/generic_aging.h"
#include "../../visitor/visitor.h"
#include "matlabFilter.h"

namespace observer
{

template < typename T >
class MatlabFilterAging : public MatlabFilter< T, aging::AgingTwoPort, PreparationType< T > >,
                          public visitor::Visitor< aging::AgingBase >
{
    typedef Filter< T, aging::AgingTwoPort, PreparationType< T > > FilterT;

    public:
    MatlabFilterAging( std::string filename, const size_t maxSize = 100000, const std::string &filenamePrefix = "" );

    virtual ~MatlabFilterAging() { this->WriteToDisk(); }

    virtual void PrepareFilter( PreparationType< T > &prePareData );

    virtual void ProcessData( const typename FilterT::Data_t &data, const double t );

    virtual void VisitCalendarianAging( aging::CalendarianAging &calendarianAging );
    virtual void VisitCyclicalAging( aging::CyclicalAging &cyclicalAging );
    virtual void VisitAnodeOverhang( aging::AnodeOverhang &anodeOverhang );
    virtual void VisitGenericAging( aging::GenericAging &genericAging );

    private:
    size_t mCellNumber;

    // general
    std::vector< std::vector< double > > mCellAgeValues;
    std::vector< std::vector< double > > mChargeThroughputValues;
    std::vector< std::vector< double > > mFullCycles;
    std::vector< std::vector< double > > mCapacityFactors;
    std::vector< std::vector< double > > mResistanceFactors;
    std::vector< std::vector< double > > mSocOffsets;

    // calendric
    std::vector< std::vector< double > > mCapacityFactorCalendarian;
    std::vector< std::vector< double > > mResistanceFactorCalendarian;
    std::vector< std::vector< double > > mAlphaCapacity;
    std::vector< std::vector< double > > mAlphaResistance;
    std::vector< std::vector< double > > mSocOffsetCalendarian;

    // cyclic
    std::vector< std::vector< double > > mCapacityFactorCyclical;
    std::vector< std::vector< double > > mResistanceFactorCyclical;
    std::vector< std::vector< double > > mBetaCapacity;
    std::vector< std::vector< double > > mBetaResistance;
    std::vector< std::vector< double > > mSocOffsetCyclical;

    // anode overhang
    std::vector< std::vector< double > > mAnodeOverhangSocs;
    std::vector< std::vector< double > > mAnodeOverhangVoltages;
    std::vector< std::vector< double > > mAnodeOverhangAnodeOffsets;

    // generic aging
    std::vector< std::vector< double > > mGenericCapacityFactors;
    std::vector< std::vector< double > > mGenericSocOffsets;
    std::vector< std::vector< double > > mGenericResistanceFactors;
};

template < typename T >
MatlabFilterAging< T >::MatlabFilterAging( std::string filename, const size_t maxSize, const std::string &filenamePrefix )
    : MatlabFilter< T, aging::AgingTwoPort, PreparationType< T > >( filename, maxSize, filenamePrefix )
{
    this->mMatlabMatrices = {{&mCellAgeValues, "CellAge"},
                             {&mChargeThroughputValues, "ChargeThroughput"},
                             {&mFullCycles, "FullCycles"},
                             {&mCapacityFactors, "RelativeCapacity"},
                             {&mResistanceFactors, "RelativeResistance"},
                             {&mSocOffsets, "SocOffset"},
                             {&mCapacityFactorCalendarian, "calendric.CapacityFactor"},
                             {&mResistanceFactorCalendarian, "calendric.ResistanceFactor"},
                             {&mAlphaCapacity, "calendric.AlphaCapacity"},
                             {&mAlphaResistance, "calendric.AlphaResistance"},
                             {&mSocOffsetCalendarian, "calendric.SocOffset"},
                             {&mCapacityFactorCyclical, "cyclic.CapacityFactor"},
                             {&mResistanceFactorCyclical, "cyclic.ResistanceFactor"},
                             {&mBetaCapacity, "cyclic.BetaCapacity"},
                             {&mBetaResistance, "cyclic.BetaResistance"},
                             {&mSocOffsetCyclical, "cyclic.SocOffset"},
                             {&mAnodeOverhangSocs, "anodeOverhang.AnodeOverhangSoc"},
                             {&mAnodeOverhangVoltages, "anodeOverhang.AnodeOverhangVoltage"},
                             {&mAnodeOverhangAnodeOffsets, "anodeOverhang.AnodeSocOffset"},
                             {&mGenericCapacityFactors, "genericAging.CapacityFactor"},
                             {&mGenericSocOffsets, "genericAging.SocOffset"},
                             {&mGenericResistanceFactors, "genericAging.ResistanceFactor"}};
}

template < typename T >
void MatlabFilterAging< T >::PrepareFilter( PreparationType< T > &prePareData )
{
    MatlabFilter< T, aging::AgingTwoPort, PreparationType< T > >::PrepareFilter( prePareData );
    this->InitializeMatrices( prePareData.mNumberOfElements );
}

template < typename T >
void MatlabFilterAging< T >::ProcessData( const typename FilterT::Data_t &data, const double t )
{
    for ( size_t i = 0; i < data.size(); ++i )
    {
        aging::AgingTwoPort< T > *agingTwoPort = data[i];
        const auto &agingState = *agingTwoPort->GetAgingState();
        const double chargeThroughput = agingState.GetChargeThroughput() / 3600;
        const double fullCycles =
         chargeThroughput / agingTwoPort->GetTwoPort()->GetSoc()->template GetInitialCapacity< state::SocGetFormat::AH >();

        mCellAgeValues[i].push_back( agingState.GetCellAge() / 3600 / 24 );
        mChargeThroughputValues[i].push_back( chargeThroughput );
        mFullCycles[i].push_back( fullCycles );
        mCapacityFactors[i].push_back( agingState.GetCapacityFactor() );
        mResistanceFactors[i].push_back( agingState.GetResistanceFactor() );
        mSocOffsets[i].push_back( agingState.GetSocOffset() );

        mCellNumber = i;

        for ( auto &agingObject : agingTwoPort->GetAgingEffects() )
        {
            agingObject->AcceptVisitor( *this );
        }
    }
    MatlabFilter< T, aging::AgingTwoPort, PreparationType< T > >::ProcessData( data, t );
}

template < typename T >
void MatlabFilterAging< T >::VisitCalendarianAging( aging::CalendarianAging &calendarianAging )
{
    mCapacityFactorCalendarian[mCellNumber].push_back( calendarianAging.GetCapacityFactor() );
    mSocOffsetCalendarian[mCellNumber].push_back( calendarianAging.GetSocOffset() );
    mResistanceFactorCalendarian[mCellNumber].push_back( calendarianAging.GetResistanceFactor() );
    mAlphaCapacity[mCellNumber].push_back( calendarianAging.GetStressFactorCapacity() );
    mAlphaResistance[mCellNumber].push_back( calendarianAging.GetStressFactorResistance() );
}

template < typename T >
void MatlabFilterAging< T >::VisitCyclicalAging( aging::CyclicalAging &cyclicalAging )
{
    mCapacityFactorCyclical[mCellNumber].push_back( cyclicalAging.GetCapacityFactor() );
    mSocOffsetCyclical[mCellNumber].push_back( cyclicalAging.GetSocOffset() );
    mResistanceFactorCyclical[mCellNumber].push_back( cyclicalAging.GetResistanceFactor() );
    mBetaCapacity[mCellNumber].push_back( cyclicalAging.GetStressFactorCapacity() );
    mBetaResistance[mCellNumber].push_back( cyclicalAging.GetStressFactorResistance() );
}

template < typename T >
void MatlabFilterAging< T >::VisitAnodeOverhang( aging::AnodeOverhang &anodeOverhang )
{
    mAnodeOverhangSocs[mCellNumber].push_back( anodeOverhang.GetSoc()->GetValue() );
    mAnodeOverhangVoltages[mCellNumber].push_back( anodeOverhang.GetVoltage() );
    mAnodeOverhangAnodeOffsets[mCellNumber].push_back( anodeOverhang.GetSocOffset() );
}

template < typename T >
void MatlabFilterAging< T >::VisitGenericAging( aging::GenericAging &genericAging )
{
    mGenericCapacityFactors[mCellNumber].push_back( genericAging.GetCapacityFactor() );
    mGenericSocOffsets[mCellNumber].push_back( genericAging.GetSocOffset() );
    mGenericResistanceFactors[mCellNumber].push_back( genericAging.GetResistanceFactor() );
}
}    // namespace observer

#endif /* _DS1006 */
#endif /* __NO_STRING__ */
#endif /* BUILD_AGING_SIMULATION */
#endif /* _MATLABFILTERAGING_ */
