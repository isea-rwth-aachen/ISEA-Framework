/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : stdoutfilter.h
* Creation Date : 18-11-2012
* Last Modified : Fr 04 Sep 2015 10:35:07 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _STDOUTFILTER_
#define _STDOUTFILTER_

#ifdef __EXCEPTIONS__
// BOOST
#include <boost/shared_ptr.hpp>

// STD
#include <iostream>

#include "filter.h"

#include "../../aging/aging_twoport.h"
#include "../../aging/anode_overhang.h"
#include "../../electrical/cellelement.h"
#include "../../state/soc.h"

namespace observer
{

template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
class StdoutFilter : public Filter< T, TConcrete, ArgumentType >
{
    public:
    typedef Filter< T, TConcrete, ArgumentType > FilterT;
    StdoutFilter( std::ostream& outputStream )
        : FilterT()
        , mOutputStream( outputStream ){};
    virtual ~StdoutFilter(){};

    virtual void ProcessData( const typename FilterT::Data_t& data, const double t )
    {
        this->mOutputStream << "Time: " << t << std::endl;
        for ( size_t i = 0; i < data.size(); ++i )
        {
            this->mOutputStream << data[i] << std::endl;
        }
    };

    private:
    protected:
    std::ostream& mOutputStream;
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
class StdoutFilterBase : public StdoutFilter< T, TConcrete, ArgumentType >
{
    public:
    StdoutFilterBase()
        : StdoutFilter< T, TConcrete, ArgumentType >(){};
};


template < typename T >
class StdoutFilterBase< T, electrical::TwoPort, PreparationType< T > >
    : public StdoutFilter< T, electrical::TwoPort, PreparationType< T > >
{
    private:
    electrical::TwoPort< T >* mRootPort;

    virtual void PrintData( electrical::TwoPort< T >* port )
    {
        this->mOutputStream << "\n";

        if ( !port )
            return;

        this->mOutputStream << "Voltage: " << port->GetVoltageValue();
        this->mOutputStream << " Current: " << port->GetCurrentValue();
        this->mOutputStream << " Power: " << port->GetCurrentValue() * port->GetVoltageValue();
        this->mOutputStream << " Dissipation Power: " << port->GetPowerValue();

        if ( !port->IsCellelement() )
        {
            this->mOutputStream << "\n";
            return;
        }

        electrical::Cellelement< T >* cell = static_cast< electrical::Cellelement< T >* >( port );
        this->mOutputStream << " Soc: " << cell->GetSocStateValue();
        this->mOutputStream << " Temperature: " << cell->GetThermalState()->GetValue();

        if ( cell->GetConfigurationType() == electrical::CellConfiguration::HALFCELL )
        {
            this->mOutputStream << " AnodeVoltage: " << cell->GetAnodeVoltageValue() * -1;
            this->mOutputStream << " CathodeVoltage: " << cell->GetCathodeVoltageValue();
            this->mOutputStream << " AnodeVoltage: " << cell->GetAnodeVoltageValue() * -1;
            this->mOutputStream << " CathodeVoltage: " << cell->GetCathodeVoltageValue();
            const std::vector< boost::shared_ptr< electrical::AnodeElement< T > > > anodeElements = cell->GetAnodeElements();
            const std::vector< boost::shared_ptr< electrical::CathodeElement< T > > > cathodeElements =
             cell->GetCathodeElements();

            for ( size_t i = 0; i < anodeElements.size(); ++i )
            {
                this->mOutputStream << "\n";
                this->mOutputStream << "  AnodeElement " << i + 1 << ":";
                this->mOutputStream << "  Current: " << anodeElements[i]->GetCurrentValue();
                this->mOutputStream << "  Potential: " << anodeElements[i]->GetElectrodePotentialValue() * -1;
                this->mOutputStream << "  SOC: " << anodeElements[i]->GetSocValue();
            }

            for ( size_t i = 0; i < cathodeElements.size(); ++i )
            {
                this->mOutputStream << "\n";
                this->mOutputStream << "  CathodeElement " << i + 1 << ":";
                this->mOutputStream << "  Current: " << cathodeElements[i]->GetCurrentValue();
                this->mOutputStream << "  Potential: " << cathodeElements[i]->GetElectrodePotentialValue();
                this->mOutputStream << "  SOC: " << cathodeElements[i]->GetSocValue();
            }
        }

        this->mOutputStream << "\n";
    }

    public:
    StdoutFilterBase( std::ostream& outputStream = std::cout )
        : StdoutFilter< T, electrical::TwoPort, PreparationType< T > >( outputStream ){};

    typedef Filter< T, electrical::TwoPort, PreparationType< T > > FilterT;

    virtual void PrepareFilter( PreparationType< T >& prePareData )
    {
        mRootPort = prePareData.mRootPort;
        if ( mRootPort )
        {
            this->mOutputStream << "Maximum capacity in Ah: " << mRootPort->GetTotalCapacity() << std::endl;
        }
    }

    virtual void ProcessData( const typename FilterT::Data_t& data, const double t )
    {
        this->mOutputStream << "Time: " << t << "\n";

        for ( size_t i = 0; i < data.size(); ++i )
        {
            electrical::TwoPort< T >* port = data[i];

            if ( port->IsCellelement() )
                this->mOutputStream << " Cellelement " << i + 1;
            else if ( port == mRootPort )
                this->mOutputStream << " RootPort";
            else
                this->mOutputStream << " TwoPort " << i + 1;

            PrintData( port );
        }

        this->mOutputStream << std::endl;

        FilterT::ProcessData( data, t );
    }
};

template < typename T >
class StdoutFilterBase< T, thermal::ThermalElement, ThermalPreperation >
    : public StdoutFilter< T, thermal::ThermalElement, ThermalPreperation >
{

    typedef Filter< T, thermal::ThermalElement, ThermalPreperation > FilterT;

    public:
    StdoutFilterBase( std::ostream& outputStream = std::cout )
        : StdoutFilter< T, thermal::ThermalElement, ThermalPreperation >( outputStream ){};

    virtual void PrepareFilter( ThermalPreperation& prepData )
    {
        mThermalProbes = prepData.mProbes;

        this->mOutputStream << "Surface area: " << prepData.mSurfaceArea << std::endl;
        this->mOutputStream << "Total heat capacity: " << prepData.mTotalHeatCapacity << std::endl;
    }

    virtual void ProcessData( const typename FilterT::Data_t& data, const double t )
    {
        if ( mThermalProbes->empty() )
        {
            FilterT::ProcessData( data, t );
            return;
        }
        this->mOutputStream << "Time: " << t << "\n";

        for ( const auto& probe : ( *mThermalProbes ) )    // must exist as passed by reference, so no pointer check is necessary
        {
            this->mOutputStream << " " << probe.GetTemperature() << ",";
        }
        this->mOutputStream << std::endl;

        FilterT::ProcessData( data, t );
    }


    protected:
    boost::shared_ptr< std::vector< ::probe::ThermalProbe > > mThermalProbes;
};

#ifdef BUILD_AGING_SIMULATION

template < typename T >
class StdoutFilterAging : public StdoutFilter< T, aging::AgingTwoPort, PreparationType< T > >
{
    typedef Filter< T, aging::AgingTwoPort, PreparationType< T > > FilterT;

    public:
    StdoutFilterAging( std::ostream& outputStream = std::cout )
        : StdoutFilter< T, aging::AgingTwoPort, PreparationType< T > >( outputStream ){};

    virtual void ProcessData( const typename FilterT::Data_t& data, const double t )
    {
        for ( size_t i = 0; i < data.size(); i++ )
        {
            aging::AgingTwoPort< T >* agingTwoPort = data[i];
            const auto& agingState = *agingTwoPort->GetAgingState();
            if ( auto cell = boost::dynamic_pointer_cast< electrical::Cellelement< T > >( agingTwoPort->GetTwoPort() ) )
                this->mOutputStream << "Cellelement " << cell->mCellNumber + 1 << std::endl;
            else if ( boost::dynamic_pointer_cast< electrical::AnodeElement< T > >( agingTwoPort->GetTwoPort() ) )
                this->mOutputStream << "AnodeElement " << std::endl;
            else if ( boost::dynamic_pointer_cast< electrical::CathodeElement< T > >( agingTwoPort->GetTwoPort() ) )
                this->mOutputStream << "CathodeElement " << std::endl;

            this->mOutputStream << " C/C0: " << agingState.GetCapacityFactor() << std::endl;
            this->mOutputStream << " R/R0: " << agingState.GetResistanceFactor() << std::endl;

            this->mOutputStream << " CellAge: " << agingState.GetCellAge() / 3600 / 24 << " days" << std::endl;
            this->mOutputStream << " ChargeThroughput: " << agingState.GetChargeThroughput() / 3600 << " Ah, ";
            this->mOutputStream
             << " Fullcycles: "
             << agingState.GetChargeThroughput() /
                 agingTwoPort->GetTwoPort()->GetSoc()->template GetInitialCapacity< state::SocGetFormat::AS >()
             << std::endl;

            // TODO: Make this less terrible
            for ( const boost::shared_ptr< aging::AgingBase >& agingEffect : agingTwoPort->GetAgingEffects() )
            {
                if ( const auto& overhang = dynamic_cast< aging::AnodeOverhang* >( agingEffect.get() ) )
                {
                    this->mOutputStream << " Anode Overhang SOC: " << overhang->GetSoc()->GetValue() << std::endl;
                }
            }

            this->mOutputStream << std::endl;
        }
        FilterT::ProcessData( data, t );
    }
};

#endif

template < typename T >
using StdoutFilterTwoPort = StdoutFilterBase< T, electrical::TwoPort, PreparationType< T > >;

template < typename T >
using StdoutFilterThermal = StdoutFilterBase< T, thermal::ThermalElement, ThermalPreperation >;

}    // namespace observer
#endif /* __EXCEPTIONS__ */
#endif /* _STDOUTFILTER_ */
