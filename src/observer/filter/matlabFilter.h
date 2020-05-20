/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name :
* Creation Date : 18-11-2012
* Last Modified : Do 15 Jun 2017 14:19:41 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MATLABFILTER_
#define _MATLABFILTER_

#ifndef __NO_STRING__
#ifndef _DS1006
#ifndef _SYMBOLIC_
// BOOST
#include <boost/shared_ptr.hpp>

// STD
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>

#include "../../container/matio_data.h"
#include "../../container/matio_file.h"
#include "../../electrical/cellelement.h"
#include "../../thermal/blocks/elements/thermal_element.h"
#include "../observer.h"

namespace observer
{

template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
class MatlabFilter : public Filter< T, TConcrete, ArgumentType >
{
    public:
    typedef Filter< T, TConcrete, ArgumentType > FilterT;


    MatlabFilter( std::string filename, const size_t maxSize = 100000, const std::string &filenamePrefix = "" )
        : FilterT()
        , mFilename( filename )
        , mMaxSize( maxSize )
        , mDataLocation( "diga.daten." )
        , mFilenamePrefix( filenamePrefix ){};

    virtual ~MatlabFilter() { this->WriteToDisk(); }

    virtual void ProcessData( const typename FilterT::Data_t &data, const double t )
    {
        UNUSED( data );
        mTime.push_back( t );
        FilterT::ProcessData( data, t );
        mSampleCounter += data.size();
        if ( mSampleCounter >= mMaxSize )
        {
            MaxSampleReached( ++mCurrentCycle );
            mSampleCounter = 0;
        }
    };
    virtual void WriteToDisk()
    {
        if ( mTime.empty() )
            return;
        *mMatFile << matlab::MatioData( mTime, this->mDataLocation + "Programmdauer" );
        mTime.clear();
    };

    /// Set the matlab struct that all data is written to, e.g. diga.daten
    virtual void SetDataLocation( const std::string &location ) { this->mDataLocation = location + "."; }
    virtual void SetFilenamePrefix( const std::string &prefix ) { this->mFilenamePrefix = prefix; }

    virtual void MaxSampleReached( size_t nextNumber ) = 0;
    virtual void PrepareFilter( ArgumentType /*&prepData */ )
    {
        this->mMatFile.reset( new matlab::MatFile( MakeFilename( mCurrentCycle ), MAT_ACC_RDWR ) );
    }

    protected:
    std::string mFilename;
    std::vector< double > mTime;
    const size_t mMaxSize = 100000;
    size_t mSampleCounter = 0;
    size_t mCurrentCycle = 0;
    boost::scoped_ptr< matlab::MatFile > mMatFile;
    std::string mDataLocation;
    std::string mFilenamePrefix;

    std::string MakeFilename( size_t cycle )
    {
        size_t lastSlash = this->mFilename.find_last_of( '/' );
        size_t filenameStart;
        if ( lastSlash == std::string::npos )
            filenameStart = 0;
        else
            filenameStart = lastSlash + 1;
        size_t extensionStart = this->mFilename.find( '.', filenameStart );
        if ( extensionStart == std::string::npos )
            extensionStart = mFilename.length();
        std::string path = this->mFilename.substr( 0, filenameStart );
        std::string filenameWithoutExtension = this->mFilename.substr( filenameStart, extensionStart - filenameStart );
        std::string extension = this->mFilename.substr( extensionStart );
        return path + this->mFilenamePrefix + filenameWithoutExtension + "_" + std::to_string( cycle ) + extension;
    }
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
class MatlabFilterBase : public MatlabFilter< T, TConcrete, ArgumentType >
{

    public:
    MatlabFilterBase( std::string filename, const size_t maxSize = 100000, const std::string &filenamePrefix = "" )
        : MatlabFilter< T, TConcrete, ArgumentType >( filename, maxSize, filenamePrefix ){};
};

template < typename T >
class MatlabFilterBase< T, electrical::TwoPort, PreparationType< T > >
    : public MatlabFilter< T, electrical::TwoPort, PreparationType< T > >
{

    private:
    electrical::TwoPort< T > *mRootPort;

    typedef Filter< T, electrical::TwoPort, PreparationType< T > > FilterT;

    public:
    MatlabFilterBase( std::string filename, const size_t maxSize = 100000, const std::string &filenamePrefix = "" )
        : MatlabFilter< T, electrical::TwoPort, PreparationType< T > >( filename, maxSize, filenamePrefix ){};

    virtual ~MatlabFilterBase() { WriteToDisk(); }

    virtual void WriteToDisk()
    {
        MatlabFilter< T, electrical::TwoPort, PreparationType< T > >::WriteToDisk();
        if ( mCurrentVec.empty() || mVoltageVec.empty() || mPowerVec.empty() || mSocVec.empty() || mTemperatureVec.empty() )
            return;

        size_t resizer = 0;
        for ( size_t i = 0; i < mCurrentVec.size(); ++i )
        {
            if ( mCurrentVec[i].empty() )
            {
                resizer = i;
                break;
            }
        }
        if ( resizer )
        {
            mCurrentVec.resize( resizer );
            mVoltageVec.resize( resizer );
            mAnodeVoltageVec.resize( resizer );
            mAnodePotentialVec.resize( resizer );
            mCathodeVoltageVec.resize( resizer );
            mCathodePotentialVec.resize( resizer );
            mPowerVec.resize( resizer );
            mSocVec.resize( resizer );
            mAnodeSocVec.resize( resizer );
            mCathodeSocVec.resize( resizer );
            mTemperatureVec.resize( resizer );
        }

        *this->mMatFile << matlab::MatioData( mCurrentVec, this->mDataLocation + "StromVec" );
        *this->mMatFile << matlab::MatioData( mVoltageVec, this->mDataLocation + "SpannungVec" );
        *this->mMatFile << matlab::MatioData( mAnodeVoltageVec, this->mDataLocation + "AnodeVoltageVec" );
        *this->mMatFile << matlab::MatioData( mAnodePotentialVec, this->mDataLocation + "AnodePotentialVec" );
        *this->mMatFile << matlab::MatioData( mCathodeVoltageVec, this->mDataLocation + "CathodeVoltageVec" );
        *this->mMatFile << matlab::MatioData( mCathodePotentialVec, this->mDataLocation + "CathodePotentialVec" );
        *this->mMatFile << matlab::MatioData( mPowerVec, this->mDataLocation + "ThermischLeistungVec" );
        *this->mMatFile << matlab::MatioData( mSocVec, this->mDataLocation + "SOCVec" );
        *this->mMatFile << matlab::MatioData( mAnodeSocVec, this->mDataLocation + "AnodeSOCVec" );
        *this->mMatFile << matlab::MatioData( mCathodeSocVec, this->mDataLocation + "CathodeSOCVec" );
        *this->mMatFile << matlab::MatioData( mTemperatureVec, this->mDataLocation + "TemperaturVec" );

        if ( mCurrent.empty() || mVoltage.empty() || mPower.empty() )
            return;

        *this->mMatFile << matlab::MatioData( mCurrent, this->mDataLocation + "Strom" );
        *this->mMatFile << matlab::MatioData( mVoltage, this->mDataLocation + "Spannung" );
        *this->mMatFile << matlab::MatioData( mPower, this->mDataLocation + "ThermischLeistung" );
    }

    virtual void MaxSampleReached( size_t nextNumber )
    {
        WriteToDisk();
        this->mMatFile.reset( new matlab::MatFile( this->MakeFilename( nextNumber ), MAT_ACC_RDWR ) );

        for ( size_t i = 0; i < mCurrentVec.size(); ++i )
        {
            mCurrentVec.at( i ).clear();
            mVoltageVec.at( i ).clear();
            mAnodeVoltageVec.at( i ).clear();
            mAnodePotentialVec.at( i ).clear();
            mCathodeVoltageVec.at( i ).clear();
            mCathodePotentialVec.at( i ).clear();
            mPowerVec.at( i ).clear();
            mSocVec.at( i ).clear();
            mAnodeSocVec.at( i ).clear();
            mCathodeSocVec.at( i ).clear();
            mTemperatureVec.at( i ).clear();
        }
        mCurrent.clear();
        mVoltage.clear();
        mPower.clear();
    }

    virtual void PrepareFilter( PreparationType< T > &prePareData )
    {
        MatlabFilter< T, electrical::TwoPort, PreparationType< T > >::PrepareFilter( prePareData );
        InitializeVectors( prePareData.mNumberOfElements );
        mRootPort = prePareData.mRootPort;
        if ( mRootPort )
        {
            std::vector< double > capacity = {mRootPort->GetTotalCapacity()};
            *this->mMatFile << matlab::MatioData( capacity, this->mDataLocation + "totalCapacity" );
        }
    };

    virtual void ProcessData( const typename FilterT::Data_t &data, const double t )
    {

        for ( size_t i = 0; i < data.size(); ++i )
        {
            electrical::TwoPort< T > *port = data[i];
            mVoltageVec[i].push_back( port->GetVoltageValue() );
            mCurrentVec[i].push_back( port->GetCurrentValue() );
            mPowerVec[i].push_back( port->GetPowerValue() );

            if ( port->IsCellelement() )
            {
                electrical::Cellelement< T > *cell = static_cast< electrical::Cellelement< T > * >( port );
                mSocVec[i].push_back( cell->GetSocStateValue() );
                mTemperatureVec[i].push_back( cell->GetThermalState()->GetValue() );

                if ( cell->GetConfigurationType() == electrical::CellConfiguration::HALFCELL )
                {
                    mAnodeVoltageVec[i].push_back( cell->GetAnodeVoltageValue() * -1 );
                    mAnodePotentialVec[i].push_back( cell->GetAnodePotentialValue() * -1 );
                    mAnodeSocVec[i].push_back( cell->GetAnodeElements()[0]->GetSocValue() );

                    mCathodeVoltageVec[i].push_back( cell->GetCathodeVoltageValue() );
                    mCathodePotentialVec[i].push_back( cell->GetCathodePotentialValue() );
                    mCathodeSocVec[i].push_back( cell->GetCathodeElements()[0]->GetSocValue() );
                }
            }
            else
            {
                mSocVec[i].push_back( -1.0 );
                mTemperatureVec[i].push_back( -273 );
            }
        }

        if ( mRootPort )
        {
            electrical::TwoPort< T > *port = mRootPort;
            mVoltage.push_back( port->GetVoltageValue() );
            mCurrent.push_back( port->GetCurrentValue() );
            mPower.push_back( port->GetPowerValue() );
        }
        MatlabFilter< T, electrical::TwoPort, PreparationType< T > >::ProcessData( data, t );
    }

    private:
    void InitializeVectors( const size_t vectorSizes )
    {
        mCurrentVec.resize( vectorSizes );
        mVoltageVec.resize( vectorSizes );
        mAnodeVoltageVec.resize( vectorSizes );
        mAnodePotentialVec.resize( vectorSizes );
        mCathodeVoltageVec.resize( vectorSizes );
        mCathodePotentialVec.resize( vectorSizes );
        mSocVec.resize( vectorSizes );
        mAnodeSocVec.resize( vectorSizes );
        mCathodeSocVec.resize( vectorSizes );
        mPowerVec.resize( vectorSizes );
        mTemperatureVec.resize( vectorSizes );
    };

    std::vector< std::vector< double > > mCurrentVec;
    std::vector< std::vector< double > > mVoltageVec;
    std::vector< std::vector< double > > mAnodeVoltageVec;
    std::vector< std::vector< double > > mAnodePotentialVec;
    std::vector< std::vector< double > > mCathodeVoltageVec;
    std::vector< std::vector< double > > mCathodePotentialVec;
    std::vector< std::vector< double > > mSocVec;
    std::vector< std::vector< double > > mAnodeSocVec;
    std::vector< std::vector< double > > mCathodeSocVec;
    std::vector< std::vector< double > > mPowerVec;
    std::vector< std::vector< double > > mTemperatureVec;

    std::vector< double > mCurrent;
    std::vector< double > mVoltage;
    std::vector< double > mPower;
};

template < typename T >
class MatlabFilterBase< T, thermal::ThermalElement, ThermalPreperation >
    : public MatlabFilter< T, thermal::ThermalElement, ThermalPreperation >
{

    typedef Filter< T, thermal::ThermalElement, ThermalPreperation > FilterT;

    public:
    MatlabFilterBase( std::string filename, const size_t maxSize = 100000, const std::string &filenamePrefix = "" )
        : MatlabFilter< T, thermal::ThermalElement, ThermalPreperation >( filename, maxSize, filenamePrefix )
        , mFileNameVertices( "Patch_Vertices.csv" )
        , mFileNameAreas( "Patch_Areas.csv" )
        , mFileNameAreasElectrical( "Patch_AreasElectrical.csv" )
        , mFileNameElectricThermalMapping( "Patch_ElectricThermalMapping.csv" )
        , mFileNameVolumes( "Patch_Volumes.csv" )
        , mFileNameVolumeNames( "Patch_VolumeNames.csv" )
        , mFileNameVolumeMaterials( "Patch_VolumeMaterials.csv" ){};

    virtual ~MatlabFilterBase() { WriteToDisk(); }

    virtual void WriteToDisk()
    {
        MatlabFilter< T, thermal::ThermalElement, ThermalPreperation >::WriteToDisk();
        if ( !mTemperature.empty() )
            *this->mMatFile << matlab::MatioData( mTemperature, this->mDataLocation + "Temperature" );

        if ( !mProbesTemperature.empty() )
            *this->mMatFile << matlab::MatioData( mProbesTemperature, this->mDataLocation + "ThermalProbe" );

        if ( !mCoolingVector.empty() )
            *this->mMatFile << matlab::MatioData( mCoolingVector, this->mDataLocation + "Cooling" );
    }

    virtual void MaxSampleReached( size_t nextNumber )
    {
        WriteToDisk();
        // std::swap( this->mMatFile, matlab::MatFile( nextNumber + "_" + this->mFilename, MAT_ACC_RDWR ) );
        this->mMatFile.reset( new matlab::MatFile( this->MakeFilename( nextNumber ), MAT_ACC_RDWR ) );

        for ( size_t i = 0; i < mTemperature.size(); ++i )
        {
            mTemperature[i].clear();
            mCoolingVector[i].clear();
        }
        for ( size_t i = 0; i < mProbesTemperature.size(); ++i )
        {
            mProbesTemperature[i].clear();
        }
    }

    virtual void PrepareFilter( ThermalPreperation &prepData )
    {
        MatlabFilter< T, thermal::ThermalElement, ThermalPreperation >::PrepareFilter( prepData );
        InitializeVectors( prepData.mAreas->size() );

        *this->mMatFile << matlab::MatioData( *prepData.mConductivityMatrix, this->mDataLocation + "Conductivity" );
        std::vector< double > surf = {prepData.mSurfaceArea};
        *this->mMatFile << matlab::MatioData( surf, this->mDataLocation + "SurfaceArea" );
        std::vector< double > capacity = {prepData.mTotalHeatCapacity};
        *this->mMatFile << matlab::MatioData( capacity, this->mDataLocation + "TotalHeatCapacity" );
        mThermalProbes = prepData.mProbes;
        mProbesTemperature.resize( prepData.mProbes->size() );

        std::ofstream fileVertices( mFileNameVertices );
        std::ofstream fileAreas( mFileNameAreas );
        std::ofstream fileVolumes( mFileNameVolumes );
        std::ofstream fileVolumeNames( mFileNameVolumeNames );

        BOOST_FOREACH ( const geometry::Cartesian< T > &vertex, *prepData.mVertices )
            fileVertices << vertex.GetX() << ", " << vertex.GetY() << ", " << vertex.GetZ() << "\n";

        BOOST_FOREACH ( const std::vector< size_t > &area, *prepData.mAreas )
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


        BOOST_FOREACH ( const std::vector< size_t > &volume, *prepData.mVolumes )
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

        BOOST_FOREACH ( misc::StrCont &volumeName, *prepData.mVolumeNames )
        {
            fileVolumeNames << volumeName << "\n";
        }

        fileVertices.close();
        fileAreas.close();
        fileVolumes.close();
        fileVolumeNames.close();
    }


    virtual void ProcessData( const typename FilterT::Data_t &data, const double t )
    {
        size_t element = 0;
        BOOST_FOREACH ( const thermal::ThermalElement< T > *thermalElement, data )
        {
            mTemperature[element].push_back( thermalElement->GetTemperature() );
            mCoolingVector[element].push_back( thermalElement->GetCoolingValue() );
            ++element;
        }

        element = 0;
        for ( const auto &probe : ( *mThermalProbes ) )    // must exist as passed by reference, so no pointer check is necessary
        {
            mProbesTemperature[element].push_back( probe.GetTemperature() );
            ++element;
        }


        MatlabFilter< T, thermal::ThermalElement, ThermalPreperation >::ProcessData( data, t );
    }

    void InitializeVectors( const size_t vectorSizes )
    {
        mTemperature.resize( vectorSizes );
        mCoolingVector.resize( vectorSizes );
    }

    protected:
    std::string mFileNameVertices, mFileNameAreas, mFileNameAreasElectrical, mFileNameElectricThermalMapping,
     mFileNameVolumes, mFileNameVolumeNames, mFileNameVolumeMaterials;

    private:
    boost::shared_ptr< std::vector< ::probe::ThermalProbe > > mThermalProbes;
    std::vector< std::vector< double > > mTemperature;
    std::vector< std::vector< double > > mCoolingVector;
    std::vector< std::vector< double > > mProbesTemperature;
};

template < typename T >
class MatlabFilterTwoPort : public MatlabFilterBase< T, electrical::TwoPort, PreparationType< T > >
{
    public:
    MatlabFilterTwoPort( std::string filename, const size_t maxSize = 100000, const std::string &filenamePrefix = "" )
        : MatlabFilterBase< T, electrical::TwoPort, PreparationType< T > >( filename, maxSize, filenamePrefix ){};
};

template < typename T >
class MatlabFilterThermal : public MatlabFilterBase< T, thermal::ThermalElement, ThermalPreperation >
{
    public:
    MatlabFilterThermal( std::string filename, const size_t maxSize = 100000, const std::string &filenamePrefix = "" )
        : MatlabFilterBase< T, thermal::ThermalElement, ThermalPreperation >( filename, maxSize, filenamePrefix ){};
};
}    // namespace observer


#endif /* SYMBOLIC */
#endif /* _DS1006 */
#endif /* __NO_STRING__ */
#endif /* _MATLABFILTER_ */
