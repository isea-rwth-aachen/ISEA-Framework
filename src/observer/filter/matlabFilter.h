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
        , mFilenamePrefix( filenamePrefix )
        , mMatrixSizes( 0 ){};

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

        for ( auto &pair : mMatlabMatrices )
        {
            auto &mat = *pair.first;
            // remove empty rows
            mat.erase( std::remove_if( mat.begin(), mat.end(), []( std::vector< double > &v ) { return v.empty(); } ),
                       mat.end() );
            if ( !mat.empty() )
            {
                *this->mMatFile << matlab::MatioData( mat, this->mDataLocation + pair.second );
            }
        }

        for ( auto &pair : mMatlabVectors )
        {
            auto &vec = *pair.first;
            if ( !vec.empty() )
            {
                *this->mMatFile << matlab::MatioData( vec, this->mDataLocation + pair.second );
            }
        }
    };

    /// Set the matlab struct that all data is written to, e.g. diga.daten
    virtual void SetDataLocation( const std::string &location ) { this->mDataLocation = location + "."; }
    virtual void SetFilenamePrefix( const std::string &prefix ) { this->mFilenamePrefix = prefix; }

    virtual void MaxSampleReached( size_t nextNumber );
    virtual void PrepareFilter( ArgumentType /*&prepData */ )
    {
        this->mMatFile.reset( new matlab::MatFile( MakeFilename( mCurrentCycle ), MAT_ACC_RDWR ) );
    }

    protected:
    virtual void InitializeMatrices( const size_t vectorSizes );

    std::vector< std::pair< std::vector< std::vector< double > > *, std::string > > mMatlabMatrices;
    std::vector< std::pair< std::vector< double > *, std::string > > mMatlabVectors;
    std::string mFilename;
    std::vector< double > mTime;
    const size_t mMaxSize = 100000;
    size_t mSampleCounter = 0;
    size_t mCurrentCycle = 0;
    boost::scoped_ptr< matlab::MatFile > mMatFile;
    std::string mDataLocation;
    std::string mFilenamePrefix;
    size_t mMatrixSizes;

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
void MatlabFilter< T, TConcrete, ArgumentType >::InitializeMatrices( const size_t matrixSizes )
{
    for ( auto &pair : mMatlabMatrices )
    {
        pair.first->resize( matrixSizes );
    }
    this->mMatrixSizes = matrixSizes;
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
void MatlabFilter< T, TConcrete, ArgumentType >::MaxSampleReached( size_t nextNumber )
{
    WriteToDisk();
    this->mMatFile.reset( new matlab::MatFile( this->MakeFilename( nextNumber ), MAT_ACC_RDWR ) );

    for ( auto &pair : mMatlabMatrices )
    {
        for ( size_t i = 0; i < mMatrixSizes; ++i )
        {
            pair.first->at( i ).clear();
        }
    }

    for ( auto &pair : mMatlabVectors )
    {
        pair.first->clear();
    }
}

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
    MatlabFilterBase( std::string filename, const size_t maxSize = 100000, const std::string &filenamePrefix = "" );

    virtual ~MatlabFilterBase() { this->WriteToDisk(); }

    virtual void PrepareFilter( PreparationType< T > &prePareData )
    {
        MatlabFilter< T, electrical::TwoPort, PreparationType< T > >::PrepareFilter( prePareData );
        this->InitializeMatrices( prePareData.mNumberOfElements );
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
MatlabFilterBase< T, electrical::TwoPort, PreparationType< T > >::MatlabFilterBase( std::string filename, const size_t maxSize,
                                                                                    const std::string &filenamePrefix )
    : MatlabFilter< T, electrical::TwoPort, PreparationType< T > >( filename, maxSize, filenamePrefix )
{
    this->mMatlabMatrices = {{&mCurrentVec, "StromVec"},
                             {&mVoltageVec, "SpannungVec"},
                             {&mAnodeVoltageVec, "AnodeVoltageVec"},
                             {&mAnodePotentialVec, "AnodePotentialVec"},
                             {&mCathodeVoltageVec, "CathodeVoltageVec"},
                             {&mCathodePotentialVec, "CathodePotentialVec"},
                             {&mPowerVec, "ThermischLeistungVec"},
                             {&mSocVec, "SOCVec"},
                             {&mAnodeSocVec, "AnodeSOCVec"},
                             {&mCathodeSocVec, "CathodeSOCVec"},
                             {&mTemperatureVec, "TemperaturVec"}};

    this->mMatlabVectors = {{&mCurrent, "Strom"}, {&mVoltage, "Spannung"}, {&mPower, "ThermischLeistung"}};
};

template < typename T >
class MatlabFilterBase< T, thermal::ThermalElement, ThermalPreperation >
    : public MatlabFilter< T, thermal::ThermalElement, ThermalPreperation >
{

    typedef Filter< T, thermal::ThermalElement, ThermalPreperation > FilterT;

    public:
    MatlabFilterBase( std::string filename, const size_t maxSize = 100000, const std::string &filenamePrefix = "" );

    virtual ~MatlabFilterBase() { this->WriteToDisk(); }

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
MatlabFilterBase< T, thermal::ThermalElement, ThermalPreperation >::MatlabFilterBase( std::string filename, const size_t maxSize,
                                                                                      const std::string &filenamePrefix )
    : MatlabFilter< T, thermal::ThermalElement, ThermalPreperation >( filename, maxSize, filenamePrefix )
    , mFileNameVertices( "Patch_Vertices.csv" )
    , mFileNameAreas( "Patch_Areas.csv" )
    , mFileNameAreasElectrical( "Patch_AreasElectrical.csv" )
    , mFileNameElectricThermalMapping( "Patch_ElectricThermalMapping.csv" )
    , mFileNameVolumes( "Patch_Volumes.csv" )
    , mFileNameVolumeNames( "Patch_VolumeNames.csv" )
    , mFileNameVolumeMaterials( "Patch_VolumeMaterials.csv" )
{
    this->mMatlabMatrices = {{&mTemperature, "Temperature"},
                             {&mProbesTemperature, "ThermalProbe"},
                             {&mCoolingVector, "Cooling"}};
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
