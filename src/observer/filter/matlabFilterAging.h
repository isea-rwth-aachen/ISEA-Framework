#ifndef _MATLABFILTERAGING_
#define _MATLABFILTERAGING_

#ifdef BUILD_AGING_SIMULATION
#ifndef __NO_STRING__
#ifndef _DS1006

#include "../../aging/aging_twoport.h"
#include "../../aging/anode_overhang.h"
#include "../../aging/calendarian_aging.h"
#include "../../aging/cyclical_aging.h"
#include "../../aging/calendarian_aging2.h"
#include "../../aging/cyclical_aging2.h"
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
    typedef MatlabFilter< T, aging::AgingTwoPort, PreparationType< T > > MatlabFilterT;

    public:
    MatlabFilterAging( std::string filename, const size_t maxSize, const std::string &filenamePrefix, bool ocvOutput );

    virtual ~MatlabFilterAging() { this->WriteToDisk(); }

    virtual void PrepareFilter( PreparationType< T > &prePareData );
    virtual void ProcessData( const typename FilterT::Data_t &data, const double t );
    virtual void WriteToDisk();

    virtual void Visit( aging::CalendarianAging &calendarianAging );
    virtual void Visit( aging::CyclicalAging &cyclicalAging );
    virtual void Visit( aging::CalendarianAging2 &calendarianAging2 );
    virtual void Visit( aging::CyclicalAging2 &cyclicalAging2 );
    virtual void Visit( aging::AnodeOverhang &anodeOverhang );
    virtual void Visit( aging::GenericAging &genericAging );
    virtual void Visit( aging::AgingLAM &agingLam );
    virtual void Visit( aging::AgingLLI &agingLli );

    private:
    size_t mCellNumber;
    double mStep;
    size_t mNumberOfElements;
    std::vector< std::vector< std::vector< double > > > mOCVMatrix;
    bool mOCVOutput;
};

template < typename T >
MatlabFilterAging< T >::MatlabFilterAging( std::string filename, const size_t maxSize, const std::string &filenamePrefix, bool ocvOutput )
    : MatlabFilter< T, aging::AgingTwoPort, PreparationType< T > >( filename, maxSize, filenamePrefix )
    , mOCVOutput( ocvOutput )
{
    this->mMatlabMatrices["CellAge"];
    this->mMatlabMatrices["ChargeThroughput"];
    this->mMatlabMatrices["FullCycles"];
    this->mMatlabMatrices["Capacity"];
    this->mMatlabMatrices["RelativeCapacity"];
    this->mMatlabMatrices["RelativeResistance"];
    this->mMatlabMatrices["SocOffset"];
    this->mMatlabMatrices["calendric.CapacityFactor"];
    this->mMatlabMatrices["calendric.ResistanceFactor"];
    this->mMatlabMatrices["calendric.AlphaCapacity"];
    this->mMatlabMatrices["calendric.AlphaResistance"];
    this->mMatlabMatrices["calendric.SocOffset"];
    this->mMatlabMatrices["cyclic.CapacityFactor"];
    this->mMatlabMatrices["cyclic.ResistanceFactor"];
    this->mMatlabMatrices["cyclic.BetaCapacity"];
    this->mMatlabMatrices["cyclic.BetaResistance"];
    this->mMatlabMatrices["cyclic.SocOffset"];
    this->mMatlabMatrices["calendric2.CapacityFactor"];
    this->mMatlabMatrices["calendric2.ResistanceFactor"];
    this->mMatlabMatrices["calendric2.AlphaCapacity"];
    this->mMatlabMatrices["calendric2.AlphaResistance"];
    this->mMatlabMatrices["calendric2.SocOffset"];
    this->mMatlabMatrices["cyclic2.CapacityFactor"];
    this->mMatlabMatrices["cyclic2.ResistanceFactor"];
    this->mMatlabMatrices["cyclic2.BetaCapacity"];
    this->mMatlabMatrices["cyclic2.BetaResistance"];
    this->mMatlabMatrices["cyclic2.SocOffset"];
    this->mMatlabMatrices["anodeOverhang.AnodeOverhangSoc"];
    this->mMatlabMatrices["anodeOverhang.AnodeOverhangVoltage"];
    this->mMatlabMatrices["anodeOverhang.AnodeSocOffset"];
    this->mMatlabMatrices["genericAging.CapacityFactor"];
    this->mMatlabMatrices["genericAging.ResistanceFactor"];
    this->mMatlabMatrices["genericAging.SocOffset"];
}

template < typename T >
void MatlabFilterAging< T >::PrepareFilter( PreparationType< T > &prePareData )
{
    MatlabFilter< T, aging::AgingTwoPort, PreparationType< T > >::PrepareFilter( prePareData );
    mNumberOfElements = prePareData.mNumberOfElements;
    this->InitializeMatrices( mNumberOfElements );
    if ( mOCVOutput )
        this->mOCVMatrix.resize( mNumberOfElements );
}

template < typename T >
void MatlabFilterAging< T >::ProcessData( const typename FilterT::Data_t &data, const double t )
{
    mStep = t;

    for ( size_t i = 0; i < data.size(); ++i )
    {
        aging::AgingTwoPort< T > *agingTwoPort = data[i];
        const auto &agingState = *agingTwoPort->GetAgingState();
        const double chargeThroughput = agingState.GetChargeThroughput() / 3600;
        const double fullCycles =
         chargeThroughput / agingTwoPort->GetTwoPort()->GetSoc()->template GetInitialCapacity< state::SocGetFormat::AH >();

        this->mMatlabMatrices["CellAge"][i].push_back( agingState.GetCellAge() / 3600 / 24 );
        this->mMatlabMatrices["ChargeThroughput"][i].push_back( chargeThroughput );
        this->mMatlabMatrices["FullCycles"][i].push_back( fullCycles );
        this->mMatlabMatrices["Capacity"][i].push_back(
         agingTwoPort->GetTwoPort()->GetSoc()->template GetActualCapacity< state::SocGetFormat::AH >() );
        this->mMatlabMatrices["RelativeCapacity"][i].push_back( agingState.GetCapacityFactor() );
        this->mMatlabMatrices["RelativeResistance"][i].push_back( agingState.GetResistanceFactor() );
        this->mMatlabMatrices["SocOffset"][i].push_back( agingState.GetSocOffset() );

        mCellNumber = i;

        for ( auto &agingObject : agingTwoPort->GetAgingEffects() )
        {
            agingObject->AcceptVisitor( *this );
        }

        if ( mOCVOutput && agingTwoPort->GetTwoPort()->IsCellelement() )
        {
            electrical::Cellelement< T > *cell =
             static_cast< electrical::Cellelement< T > * >( agingTwoPort->GetTwoPort().get() );
            if ( cell->HasHalfcells() )
            {
                const size_t nSteps = 100;
                this->mOCVMatrix[i].push_back( std::vector< double >( nSteps + 1 ) );
                std::vector< double > &ocvVector = this->mOCVMatrix[i].back();
                state::Soc &cellSoc = *cell->GetSoc();
                state::Soc &anodeSoc = *cell->GetAnodeElements().front()->GetSoc();
                state::Soc &cathodeSoc = *cell->GetCathodeElements().front()->GetSoc();
                double storedChargeAnode = anodeSoc.GetValue< state::SocGetFormat::AS >();
                double storedChargeCathode = cathodeSoc.GetValue< state::SocGetFormat::AS >();
                double initialCharge = cellSoc.GetValue< state::SocGetFormat::AS >();
                double stepsize = cellSoc.GetActualCapacity< state::SocGetFormat::AS >() / nSteps;
                anodeSoc.SetStoredEnergy< state::SocSetFormat::DELTA >( -initialCharge - stepsize );
                cathodeSoc.SetStoredEnergy< state::SocSetFormat::DELTA >( -initialCharge - stepsize );
                for ( size_t k = 0; k <= nSteps; ++k )
                {
                    anodeSoc.SetStoredEnergy< state::SocSetFormat::DELTA >( stepsize );
                    cathodeSoc.SetStoredEnergy< state::SocSetFormat::DELTA >( stepsize );
                    ocvVector[k] = cell->GetOcvVoltageValue();
                }
                anodeSoc.SetStoredEnergy< state::SocSetFormat::ABSOLUT >( storedChargeAnode );
                cathodeSoc.SetStoredEnergy< state::SocSetFormat::ABSOLUT >( storedChargeCathode );
            }
        }
    }
    MatlabFilter< T, aging::AgingTwoPort, PreparationType< T > >::ProcessData( data, t );
}

template < typename T >
void MatlabFilterAging< T >::WriteToDisk()
{
    MatlabFilterT::WriteToDisk();
    if ( mOCVOutput )
    {
        this->mOCVMatrix.erase( std::remove_if( this->mOCVMatrix.begin(), this->mOCVMatrix.end(),
                                                []( std::vector< std::vector< double > > &v ) { return v.empty(); } ),
                                this->mOCVMatrix.end() );
        matlab::MatioData data;
        data.LoadFromVector< 3 >( this->mOCVMatrix, this->mDataLocation + "OCV" );
        *this->mMatFile << data;
    }
}

template < typename T >
void MatlabFilterAging< T >::Visit( aging::CalendarianAging &calendarianAging )
{
    this->mMatlabMatrices["calendric.CapacityFactor"][mCellNumber].push_back( calendarianAging.GetCapacityFactor() );
    this->mMatlabMatrices["calendric.ResistanceFactor"][mCellNumber].push_back( calendarianAging.GetResistanceFactor() );
    this->mMatlabMatrices["calendric.AlphaCapacity"][mCellNumber].push_back( calendarianAging.GetStressFactorCapacity() );
    this->mMatlabMatrices["calendric.AlphaResistance"][mCellNumber].push_back( calendarianAging.GetStressFactorResistance() );
    this->mMatlabMatrices["calendric.SocOffset"][mCellNumber].push_back( calendarianAging.GetSocOffset() );
}

template < typename T >
void MatlabFilterAging< T >::Visit( aging::CalendarianAging2 &calendarianAging2 )
{
    this->mMatlabMatrices["calendric2.CapacityFactor"][mCellNumber].push_back( calendarianAging2.GetCapacityFactor() );
    this->mMatlabMatrices["calendric2.ResistanceFactor"][mCellNumber].push_back( calendarianAging2.GetResistanceFactor() );
    this->mMatlabMatrices["calendric2.AlphaCapacity"][mCellNumber].push_back( calendarianAging2.GetStressFactorCapacity() );
    this->mMatlabMatrices["calendric2.AlphaResistance"][mCellNumber].push_back( calendarianAging2.GetStressFactorResistance() );
    this->mMatlabMatrices["calendric2.SocOffset"][mCellNumber].push_back( calendarianAging2.GetSocOffset() );
}

template < typename T >
void MatlabFilterAging< T >::Visit( aging::CyclicalAging &cyclicalAging )
{
    this->mMatlabMatrices["cyclic.CapacityFactor"][mCellNumber].push_back( cyclicalAging.GetCapacityFactor() );
    this->mMatlabMatrices["cyclic.ResistanceFactor"][mCellNumber].push_back( cyclicalAging.GetResistanceFactor() );
    this->mMatlabMatrices["cyclic.BetaCapacity"][mCellNumber].push_back( cyclicalAging.GetStressFactorCapacity() );
    this->mMatlabMatrices["cyclic.BetaResistance"][mCellNumber].push_back( cyclicalAging.GetStressFactorResistance() );
    this->mMatlabMatrices["cyclic.SocOffset"][mCellNumber].push_back( cyclicalAging.GetSocOffset() );
}

template < typename T >
void MatlabFilterAging< T >::Visit( aging::CyclicalAging2 &cyclicalAging2 )
{
    this->mMatlabMatrices["cyclic2.CapacityFactor"][mCellNumber].push_back( cyclicalAging2.GetCapacityFactor() );
    this->mMatlabMatrices["cyclic2.ResistanceFactor"][mCellNumber].push_back( cyclicalAging2.GetResistanceFactor() );
    this->mMatlabMatrices["cyclic2.BetaCapacity"][mCellNumber].push_back( cyclicalAging2.GetStressFactorCapacity() );
    this->mMatlabMatrices["cyclic2.BetaResistance"][mCellNumber].push_back( cyclicalAging2.GetStressFactorResistance() );
    this->mMatlabMatrices["cyclic2.SocOffset"][mCellNumber].push_back( cyclicalAging2.GetSocOffset() );
}

template < typename T >
void MatlabFilterAging< T >::Visit( aging::AnodeOverhang &anodeOverhang )
{
    this->mMatlabMatrices["anodeOverhang.AnodeOverhangSoc"][mCellNumber].push_back( anodeOverhang.GetSoc()->GetValue() );
    this->mMatlabMatrices["anodeOverhang.AnodeOverhangVoltage"][mCellNumber].push_back( anodeOverhang.GetVoltage() );
    this->mMatlabMatrices["anodeOverhang.AnodeSocOffset"][mCellNumber].push_back( anodeOverhang.GetSocOffset() );
}

template < typename T >
void MatlabFilterAging< T >::Visit( aging::GenericAging &genericAging )
{
    this->mMatlabMatrices["genericAging.CapacityFactor"][mCellNumber].push_back( genericAging.GetCapacityFactor() );
    this->mMatlabMatrices["genericAging.ResistanceFactor"][mCellNumber].push_back( genericAging.GetResistanceFactor() );
    this->mMatlabMatrices["genericAging.SocOffset"][mCellNumber].push_back( genericAging.GetSocOffset() );
}

template < typename T >
void MatlabFilterAging< T >::Visit( aging::AgingLAM &agingLam )
{
}

template < typename T >
void MatlabFilterAging< T >::Visit( aging::AgingLLI &agingLli )
{
}

}    // namespace observer

#endif /* _DS1006 */
#endif /* __NO_STRING__ */
#endif /* BUILD_AGING_SIMULATION */
#endif /* _MATLABFILTERAGING_ */
