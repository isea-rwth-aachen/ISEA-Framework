/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : main.cpp
* Creation Date : 22-04-2015
* Last Modified : Mi 17 Jun 2015 15:10:30 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
// STD
#include <algorithm>
#include <fstream>
#include <streambuf>
#include <string>

// ETC
#include "../../src/electrical/twoport.h"

#include "../../src/export/svgExport.h"
#include "../../src/export/svgLibraryInterpreter.h"

#include "../../src/factory/factorybuilder.h"

#include "../../src/xmlparser/tinyxml2/xmlparserimpl.h"

template < class MatrixType >
boost::shared_ptr< electrical::TwoPort< MatrixType > > GenerateNetworkFromFile( const char *fileName )
{
    factory::FactoryBuilder< MatrixType, double > factoryBuilder;
    factory::Factory< electrical::TwoPort< MatrixType >, factory::ArgumentTypeElectrical > *electricalFactory =
     factoryBuilder.BuildElectricalFactory();

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromFile( fileName );

    boost::shared_ptr< xmlparser::XmlParameter > rootParam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    boost::shared_ptr< electrical::TwoPort< MatrixType > > rootPort( electricalFactory->CreateInstance( rootParam ) );
    return rootPort;
}

int main( int argc, char *argv[] )
{
    if ( argc < 3 || std::string( argv[2] ).find( ".xml" ) == std::string::npos )
    {
        std::cerr << "Please enter the following parameter: " << std::endl;
        std::cerr << "1.Description depth: 0=Cellelements, 1=Electricalelements, 2=All" << std::endl;
        std::cerr << "2.Input xml" << std::endl;

        return EXIT_FAILURE;
    }

    std::string svgLib;

    if ( argv[3] )
    {
        std::ifstream instream( argv[3] );
        svgLib = std::string( ( std::istreambuf_iterator< char >( instream ) ), std::istreambuf_iterator< char >() );
    }

    try
    {
        std::string file( argv[2] );
        boost::shared_ptr< electrical::TwoPort< myMatrixType > > tmprootPort(
         GenerateNetworkFromFile< myMatrixType >( file.c_str() ) );

        // std::string outputName = std::string( argv[2] ) + ".svg";
        // std::shared_ptr< std::ofstream > outPutFile ( new std::ofstream("outputName.svg") );

        if ( *argv[1] == '0' )
            SvgExport< myMatrixType, CELLLEVEL_DEPTH > test( tmprootPort.get(), &std::cout, svgLib );

        else if ( *argv[1] == '1' )
            SvgExport< myMatrixType, ELECTRICALELEMENTS_DEPTH > test( tmprootPort.get(), &std::cout, svgLib );

        else if ( *argv[1] == '2' )
            SvgExport< myMatrixType, ALL > test( tmprootPort.get(), &std::cout, svgLib );

        else
        {
            std::cerr << "Please use a valid description depth (0-2)." << std::endl;
            return EXIT_FAILURE;
        }
    }

    catch ( std::exception &e )
    {
        std::cerr << "Error in xml-file." << std::endl;
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch ( ... )
    {
        std::cerr << "Unknown error in xml-file." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
