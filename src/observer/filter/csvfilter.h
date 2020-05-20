/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name :
* Creation Date : 18-11-2012
* Last Modified : Mo 15 Feb 2016 13:24:25 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CSV_FILTER_
#define _CSV_FILTER_

#ifndef __NO_STRING__
// BOOST
#include <boost/shared_ptr.hpp>

// STD
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>

#include "../observer.h"
#include "../observerException.h"
#include "filter.h"

#include "../../electrical/cellelement.h"
#include "../../thermal/blocks/elements/thermal_element.h"

namespace observer
{

/// CSV Filter for electrical TwoPorts. Puts Voltage, Current, Power and SOC if availabe into a file
template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
class CsvFilter : public Filter< T, TConcrete, ArgumentType >
{
    public:
    typedef Filter< T, TConcrete, ArgumentType > FilterT;
    CsvFilter( std::string filename );
    virtual void PrintHeader() = 0;
    virtual ~CsvFilter() { mFilestream.close(); };
    // virtual void ProcessData( const typename FilterT::Data_t& data, const double t );

    protected:
    std::vector< double > mTimes;
    std::ofstream mFilestream;
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
CsvFilter< T, TConcrete, ArgumentType >::CsvFilter( std::string filename )
    : FilterT()
{
    size_t found = filename.find_last_of( ".csv" );
    const unsigned offset = 4;
    if ( found == filename.size() - offset )    // make sure we have the ending csv
        filename += ".csv";

    mFilestream.open( filename.c_str(), std::fstream::in | std::fstream::trunc );

    if ( !mFilestream.is_open() )
        ErrorFunction< CantOpenFile >( __FUNCTION__, __LINE__, __FILE__, "FileNotWorking", filename.c_str() );

    mFilestream << std::fixed << std::setprecision( std::numeric_limits< double >::digits10 + 1 );
    mFilestream << std::fixed << std::setprecision( std::numeric_limits< double >::digits10 + 1 );
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
class CsvFilterBase : public CsvFilter< T, TConcrete, ArgumentType >
{
    public:
    CsvFilterBase( std::string filename, bool printHeader = true )
        : CsvFilter< T, TConcrete, ArgumentType >( filename ){};
};

template < typename T >
class CsvFilterBase< T, electrical::TwoPort, PreparationType< T > >
    : public CsvFilter< T, electrical::TwoPort, PreparationType< T > >
{
    private:
    electrical::TwoPort< T >* mRootPort;

    public:
    typedef Filter< T, electrical::TwoPort, PreparationType< T > > FilterT;

    CsvFilterBase( std::string filename, bool printHeader = true )
        : CsvFilter< T, electrical::TwoPort, PreparationType< T > >( filename )
    {
        if ( printHeader )
            PrintHeader();
    }

    virtual void PrepareFilter( PreparationType< T >& prePareData ) { mRootPort = prePareData.mRootPort; }

    virtual void PrintHeader()
    {
        this->mFilestream
         << "#Time, Elementnr, Voltage, Current, Power, SOC, Temperature, AnodeVoltage, CathodeVoltage\n";
        this->mFilestream << "#s, Number , Voltage / V, Current / A, Power / W, SOC / %, Temperature / °C, "
                             "AnodeVoltage / V, CathodeVoltage / V\n";
    }

    virtual void PrintData( electrical::TwoPort< T >* port )
    {
        if ( !port )
            return;

        this->mFilestream << "," << port->GetVoltageValue();
        this->mFilestream << "," << port->GetCurrentValue();
        this->mFilestream << "," << port->GetPowerValue();

        if ( port->IsCellelement() )
        {
            electrical::Cellelement< T >* cell = static_cast< electrical::Cellelement< T >* >( port );

            this->mFilestream << "," << cell->GetSocStateValue();
            this->mFilestream << "," << cell->GetThermalState()->GetValue();

            if ( cell->GetConfigurationType() == electrical::CellConfiguration::HALFCELL )
            {
                this->mFilestream << "," << cell->GetAnodeVoltageValue() * -1;
                this->mFilestream << "," << cell->GetCathodeVoltageValue();
            }

            else
            {
                this->mFilestream << "," << -100;
                this->mFilestream << "," << -100;
            }
        }

        else
        {
            this->mFilestream << "," << -1.0;
            this->mFilestream << "," << -273;
        }

        this->mFilestream << "\n";
    }

    virtual void ProcessData( const typename FilterT::Data_t& data, const double t )
    {
        if ( mRootPort )
        {
            this->mFilestream << "," << -1;
            PrintData( mRootPort );
        }


        for ( size_t i = 0; i < data.size(); ++i )
        {
            this->mFilestream << t << "," << i + 1;
            PrintData( data[i] );
        }

        FilterT::ProcessData( data, t );
    }
};

template < typename T >
class CsvFilterBase< T, thermal::ThermalElement, ThermalPreperation >
    : public CsvFilter< T, thermal::ThermalElement, ThermalPreperation >
{
    public:
    typedef Filter< T, thermal::ThermalElement, ThermalPreperation > FilterT;

    virtual ~CsvFilterBase() { mProbeFile.close(); };
    CsvFilterBase( std::string filename = "./Patch_Temperatures.csv", bool printHeader = true,
                   std::string fileNameVertices = "Patch_Vertices.csv", std::string fileNameAreas = "Patch_Areas.csv",
                   std::string fileNameVolumes = "Patch_Volumes.csv",
                   std::string fileNameVolumeNames = "Patch_VolumeNames.csv",
                   std::string fileNameProbes = "Patch_ThermalProbe.csv",
                   std::string fileConductivityName = "Patch_Conductivity.csv" )

        : CsvFilter< T, thermal::ThermalElement, ThermalPreperation >( filename )
        , mFileNameVertices( fileNameVertices )
        , mFileNameAreas( fileNameAreas )
        , mFileNameVolumes( fileNameVolumes )
        , mFileNameVolumeNames( fileNameVolumeNames )
        , mFileNameThermalProbes( fileNameProbes )
        , mConductivityName( fileConductivityName )

    {
        if ( printHeader )
        {
            PrintHeader();
        }

        size_t found = mFileNameThermalProbes.find_last_of( ".csv" );
        const unsigned offset = 4;
        if ( found == mFileNameThermalProbes.size() - offset )    // make sure we have the ending csv
            mFileNameThermalProbes += ".csv";

        mProbeFile.open( mFileNameThermalProbes.c_str(), std::fstream::in | std::fstream::trunc );

        if ( !mProbeFile.is_open() )
            ErrorFunction< CantOpenFile >( __FUNCTION__, __LINE__, __FILE__, "FileNotWorking", mFileNameThermalProbes.c_str() );

        mProbeFile << std::fixed << std::setprecision( std::numeric_limits< double >::digits10 + 1 );
        mProbeFile << std::fixed << std::setprecision( std::numeric_limits< double >::digits10 + 1 );
    }

    virtual void PrepareFilter( ThermalPreperation& prepData )
    {
        mThermalProbes = prepData.mProbes;

        std::ofstream fileVertices( mFileNameVertices );
        std::ofstream fileAreas( mFileNameAreas );
        std::ofstream fileVolumes( mFileNameVolumes );
        std::ofstream fileVolumeNames( mFileNameVolumeNames );
        std::ofstream fileConductivity( mConductivityName );

        for ( size_t i = 0; i < prepData.mConductivityMatrix->size(); ++i )
        {
            for ( size_t j = 0; j < ( *prepData.mConductivityMatrix )[i].size(); ++j )
            {
                fileConductivity << ( *prepData.mConductivityMatrix )[i][j];
                if ( ( *prepData.mConductivityMatrix )[i].size() > 1 && j != ( *prepData.mConductivityMatrix )[i].size() - 1 )
                {
                    fileConductivity << ", ";
                }
            }
            fileConductivity << std::endl;
        }
        fileConductivity.close();


        BOOST_FOREACH ( const geometry::Cartesian< T >& vertex, *prepData.mVertices )
            fileVertices << vertex.GetX() << ", " << vertex.GetY() << ", " << vertex.GetZ() << "\n";

        BOOST_FOREACH ( const std::vector< size_t >& area, *prepData.mAreas )
        {
            std::vector< size_t >::const_iterator it = area.begin();
            if ( it != area.end() )
            {
                fileAreas << *it;
                for ( ++it; it != area.end(); ++it )
                {
                    fileAreas << ", " << *it;
                }
                fileAreas << "\n";
            }
        }
        BOOST_FOREACH ( const std::vector< size_t >& volume, *prepData.mVolumes )
        {
            std::vector< size_t >::const_iterator it = volume.begin();
            if ( it != volume.end() )
            {
                fileVolumes << *it;
                for ( ++it; it != volume.end(); ++it )
                {
                    fileVolumes << ", " << *it;
                }
                fileVolumes << "\n";
            }
        }
        BOOST_FOREACH ( misc::StrCont& volumeName, *prepData.mVolumeNames )
        {
            fileVolumeNames << volumeName << "\n";
        }
        fileVertices.close();
        fileAreas.close();
        fileVolumes.close();
        fileVolumeNames.close();
    }

    virtual void PrintHeader()
    {
        this->mFilestream << "#Time, Temperature of Element 1...n\n";
        this->mFilestream << "#s, °C\n";
    }

    virtual void ProcessData( const typename FilterT::Data_t& data, const double t )
    {
        this->mFilestream << t;
        BOOST_FOREACH ( const thermal::ThermalElement< T >* thermalElement, data )
            this->mFilestream << ", " << thermalElement->GetTemperature();

        this->mFilestream << "\n";
        this->mFilestream << std::flush;

        for ( const auto& probe : ( *mThermalProbes ) )
        {
            this->mProbeFile << ", " << probe.GetTemperature();
        }

        this->mProbeFile << "\n";
        this->mProbeFile << std::flush;

        FilterT::ProcessData( data, t );
    }

    protected:
    std::string mFileNameVertices, mFileNameAreas, mFileNameVolumes, mFileNameVolumeNames, mFileNameThermalProbes, mConductivityName;
    boost::shared_ptr< std::vector< ::probe::ThermalProbe > > mThermalProbes;

    std::ofstream mProbeFile;
};

template < typename T >
using CsvFilterTwoPort = CsvFilterBase< T, electrical::TwoPort, PreparationType< T > >;

template < typename T >
using CsvFilterThermal = CsvFilterBase< T, thermal::ThermalElement, ThermalPreperation >;
}    // namespace observer

#endif /* __NO_STRING__ */
#endif /* _FILENAME_ */
