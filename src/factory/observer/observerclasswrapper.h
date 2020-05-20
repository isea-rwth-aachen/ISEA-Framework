/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : observerclasswrapper.h
* Creation Date : 10-08-2015
* Last Modified : So 08 Mai 2016 12:06:53 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef _OBSERVERCLASSWRAPPER_H_
#define _OBSERVERCLASSWRAPPER_H_

// BOOST
#include <boost/make_shared.hpp>

// ETC
#include "../../observer/filter/benchmarkFilter.h"
#include "../../observer/filter/csvfilter.h"
#include "../../observer/filter/decimatefilter.h"
#include "../../observer/filter/elementCounterFilter.h"
#include "../../observer/filter/filter.h"
#include "../../observer/filter/matlabFilter.h"
#include "../../observer/filter/matlabFilterAging.h"
#include "../../observer/filter/sqlitefilter.h"
#include "../../observer/filter/stdoutfilter.h"
#include "../../observer/filter/vcpfilter.h"
#include "../../observer/filter/xmlFilter.h"


#include "../../misc/macros.h"
#include "../classwrapper.h"
#include "../factory.h"


namespace factory
{

struct ArgumentTypeObserver
{
    ArgumentTypeObserver(){};

    std::string mFilenamePrefix;
};

/// Classwrapper for ::observer namespace. This template class has to be specialized in order to create an instance of a
/// particular class.
template < typename MatrixT, template < typename > class TConcrete, typename ArgumentType >
class ObserverClassWrapperBase
    : public ClassWrapperBase< observer::Filter< MatrixT, TConcrete, ArgumentType >, ArgumentTypeObserver >
{
    public:
    ObserverClassWrapperBase(){};
};

template < typename MatrixT, template < typename > class FilterType >
class ObserverClassWrapperTwoPort
    : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};
};

template < typename MatrixT, template < typename > class FilterType >
class ObserverClassWrapperThermal
    : public ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >
{
    public:
    ObserverClassWrapperThermal()
        : ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >(){};
};

#ifdef BUILD_AGING_SIMULATION

template < typename MatrixT, template < typename > class FilterType >
class ObserverClassWrapperAging
    : public ObserverClassWrapperBase< MatrixT, electrical::Cellelement, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperAging()
        : ObserverClassWrapperBase< MatrixT, electrical::Cellelement, observer::PreparationType< MatrixT > >(){};
};

#endif /* BUILD_AGING_SIMULATION */

/// Classwrapper for observer::DecimateFilter
template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::DecimateFilterTwoPort >
    : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );

        return boost::make_shared< observer::DecimateFilterTwoPort< MatrixT > >(
         param->GetElementDoubleValue( "TimeDelay" ) );
    }
};

template < typename MatrixT >
class ObserverClassWrapperThermal< MatrixT, observer::DecimateFilterThermal >
    : public ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >
{
    public:
    ObserverClassWrapperThermal()
        : ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );

        return boost::make_shared< observer::DecimateFilterThermal< MatrixT > >(
         param->GetElementDoubleValue( "TimeDelay" ) );
    }
};


#ifndef _DS1006
#ifndef _SYMBOLIC_

/// Classwrapper for observer::CsvFilter
template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::CsvFilterTwoPort >
    : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );
        bool printHeader = true;
        if ( param->HasElementDirectChild( "printHeader" ) )
            printHeader = param->GetElementBoolValue( "printHeader" );

        return boost::make_shared< observer::CsvFilterTwoPort< MatrixT > >( param->GetElementStringValue( "Filename" ), printHeader );
    }
};

template < typename MatrixT >
class ObserverClassWrapperThermal< MatrixT, observer::CsvFilterThermal >
    : public ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >
{
    public:
    ObserverClassWrapperThermal()
        : ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );
        bool printHeader = true;
        if ( param->HasElementDirectChild( "printHeader" ) )
        {
            printHeader = param->GetElementBoolValue( "printHeader" );
        }

        std::string fileNameVertices( param->GetElementStringValue( "VerticesFile", "Patch_Vertices.csv" ) );
        std::string fileNameAreas( param->GetElementStringValue( "AreasFile", "Patch_Areas.csv" ) );
        std::string fileNameVolumes( param->GetElementStringValue( "VolumesFile", "Patch_Volumes.csv" ) );
        std::string fileNameVolumeNames( param->GetElementStringValue( "VolumesNameFile", "Patch_VolumeNames.csv" ) );
        std::string fileNameProbes( param->GetElementStringValue( "ProbesFile", "Patch_Probes.csv" ) );
        std::string fileNameConductivity(
         param->GetElementStringValue( "ConductivityFile", "Patch_Conductivity.csv" ) );
        return boost::make_shared< observer::CsvFilterThermal< MatrixT > >( param->GetElementStringValue( "Filename" ),
                                                                            printHeader, fileNameVertices, fileNameAreas,
                                                                            fileNameVolumes, fileNameVolumeNames,
                                                                            fileNameProbes, fileNameConductivity );
    }
};

/// Classwrapper for observer::ElementCounterFilterTwoPort
template < typename MatrixT >
class ObserverClassWrapperThermal< MatrixT, observer::ElementCounterFilterThermal >
    : public ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >
{
    public:
    ObserverClassWrapperThermal()
        : ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( param );
        UNUSED( arg );
        return boost::make_shared< observer::ElementCounterFilterThermal< MatrixT > >();
    }
};


/// Classwrapper for observer::ElementCounterFilterTwoPort
template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::ElementCounterFilterTwoPort >
    : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( param );
        UNUSED( arg );
        return boost::make_shared< observer::ElementCounterFilterTwoPort< MatrixT > >();
    }
};


/// Classwrapper for observer::BenchmarkFilterTwoPort
template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::BenchmarkFilterTwoPort >
    : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( param );
        UNUSED( arg );
        return boost::make_shared< observer::BenchmarkFilterTwoPort< MatrixT > >();
    }
};

/// Classwrapper for observer::BenchmarkFilterTwoPort
template < typename MatrixT >
class ObserverClassWrapperThermal< MatrixT, observer::BenchmarkFilterThermal >
    : public ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >
{
    public:
    ObserverClassWrapperThermal()
        : ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( param );
        UNUSED( arg );
        return boost::make_shared< observer::BenchmarkFilterThermal< MatrixT > >();
    }
};


/// Classwrapper for observer::StdoutFilter
template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::StdoutFilterTwoPort >
    : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( param );
        UNUSED( arg );
        return boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >(
         new observer::StdoutFilterTwoPort< MatrixT >() );
    }
};

/// Classwrapper for observer::MatlabFilter
template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::MatlabFilterTwoPort >
    : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );

        const int MAX_SIZE = 10E7;
        int maxSampleSize = MAX_SIZE;
        if ( param->HasElementDirectChild( "MaxSampleSize" ) )
        {
            maxSampleSize = param->GetElementIntValue( "MaxSampleSize", MAX_SIZE );
            if ( maxSampleSize < 0 )
            {
                maxSampleSize = MAX_SIZE;
            }
        }

        std::string filenamePrefix = "";
        if ( arg )
            filenamePrefix = arg->mFilenamePrefix;
        else if ( param->HasElementDirectChild( "FilenamePrefix" ) )
            filenamePrefix = param->GetElementStringValue( "FilenamePrefix" );

        auto filter =
         boost::make_shared< observer::MatlabFilterTwoPort< MatrixT > >( param->GetElementStringValue( "Filename" ),
                                                                         maxSampleSize, filenamePrefix );

        if ( param->HasElementDirectChild( "DataLocation" ) )
        {
            filter->SetDataLocation( param->GetElementStringValue( "DataLocation" ) );
        }

        return filter;
    }
};

template < typename MatrixT >
class ObserverClassWrapperThermal< MatrixT, observer::MatlabFilterThermal >
    : public ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >
{
    public:
    ObserverClassWrapperThermal()
        : ObserverClassWrapperBase< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {

        UNUSED( arg );
        const int MAX_SIZE = 10E7;
        int maxSampleSize = MAX_SIZE;
        if ( param->HasElementDirectChild( "MaxSampleSize" ) )
        {
            maxSampleSize = param->GetElementIntValue( "MaxSampleSize", MAX_SIZE );
            if ( maxSampleSize < 0 )
            {
                maxSampleSize = MAX_SIZE;
            }
        }

        std::string filenamePrefix = "";
        if ( arg )
            filenamePrefix = arg->mFilenamePrefix;
        else if ( param->HasElementDirectChild( "FilenamePrefix" ) )
            filenamePrefix = param->GetElementStringValue( "FilenamePrefix" );

        auto filter =
         boost::make_shared< observer::MatlabFilterThermal< MatrixT > >( param->GetElementStringValue( "Filename" ),
                                                                         maxSampleSize, filenamePrefix );

        if ( param->HasElementDirectChild( "DataLocation" ) )
        {
            filter->SetDataLocation( param->GetElementStringValue( "DataLocation" ) );
        }

        return filter;
    }
};

#ifdef BUILD_AGING_SIMULATION

template < typename MatrixT >
class ObserverClassWrapperAging< MatrixT, observer::MatlabFilterAging >
    : public ObserverClassWrapperBase< MatrixT, aging::AgingTwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperAging()
        : ObserverClassWrapperBase< MatrixT, aging::AgingTwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, aging::AgingTwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );

        const int MAX_SIZE = 10E7;
        int maxSampleSize = MAX_SIZE;
        if ( param->HasElementDirectChild( "MaxSampleSize" ) )
        {
            maxSampleSize = param->GetElementIntValue( "MaxSampleSize", MAX_SIZE );
            if ( maxSampleSize < 0 )
            {
                maxSampleSize = MAX_SIZE;
            }
        }

        std::string filenamePrefix = "";
        if ( arg )
            filenamePrefix = arg->mFilenamePrefix;
        else if ( param->HasElementDirectChild( "FilenamePrefix" ) )
            filenamePrefix = param->GetElementStringValue( "FilenamePrefix" );

        auto filter =
         boost::make_shared< observer::MatlabFilterAging< MatrixT > >( param->GetElementStringValue( "Filename" ),
                                                                       maxSampleSize, filenamePrefix );

        if ( param->HasElementDirectChild( "DataLocation" ) )
        {
            filter->SetDataLocation( param->GetElementStringValue( "DataLocation" ) );
        }

        return filter;
    }
};

template < typename MatrixT >
class ObserverClassWrapperAging< MatrixT, observer::XMLFilterAging >
    : public ObserverClassWrapperBase< MatrixT, aging::AgingTwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperAging()
        : ObserverClassWrapperBase< MatrixT, aging::AgingTwoPort, observer::PreparationType< MatrixT > >(){};

    virtual boost::shared_ptr< observer::Filter< MatrixT, aging::AgingTwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );

        return boost::make_shared< observer::XMLFilterAging< MatrixT > >( param->GetElementStringValue( "Filename" ),
                                                                          param->GetConfigurationRoot() );
    }
};

#endif /* BUILD_AGING_SIMULATION */

/// Classwrapper for observer::SqliteFilter
#ifdef _SQLITE_

template < typename MatrixT >
class ObserverClassWrapperTwoPort< MatrixT, observer::SqliteFilterTwoPort >
    : public ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >
{
    public:
    ObserverClassWrapperTwoPort()
        : ObserverClassWrapperBase< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >(){};
>>>>>>> master

    virtual boost::shared_ptr< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObserver* arg = 0 )
    {
        UNUSED( arg );

        return boost::make_shared< observer::SqliteFilterTwoPort< MatrixT > >(
         param->GetElementStringValue( "Filename" ) );
    }
};

#endif /* _SQLITE_ */

#endif /* _SYMBOLIC_ */
#endif /* _DS1006_ */

} /* namespace factory */
#endif /* STATECLASSWRAPPER_H_ */
