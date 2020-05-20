/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : system2Tex.cpp
* Creation Date : 20-06-2017
* Last Modified : Di 20 Jun 2017 21:13:57 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "../src/export/exportStructs/matlabExport.h"
#include "../src/export/matlabExport.h"
#include "../src/export/symbolicExport.h"
#include "../src/misc/symbolicParser.h"
#include "../src/thermal/electrical_simulation.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include <cstdlib>
#include <iostream>
#include <string>

/*
extern template struct symbolic::SymbolicParser< std::string::const_iterator >;
extern template class symbolic::SymbolicExport< std::string::const_iterator, symbolic::MatlabExportStruct >;

extern template class symbolic::MatlabExport< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator,
symbolic::MatlabExportStruct >, symbolic::SymbolicParser< std::string::const_iterator >, 1 >;

extern template class symbolic::MatlabExport< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator,
symbolic::MatlabExportStruct >, symbolic::SymbolicParser< std::string::const_iterator >, 2 >;

extern template class symbolic::MatlabExport< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator,
symbolic::MatlabExportStruct >, symbolic::SymbolicParser< std::string::const_iterator >, 3 >;
                                              */
template < size_t DEPTH >
using ES = MatlabExport< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator, symbolic::MatlabExportStruct >,
                         symbolic::SymbolicParser< std::string::const_iterator >, DEPTH >;


int main( int argc, char *argv[] )
{

    if ( argc < 2 )
        return EXIT_FAILURE;

    boost::scoped_ptr< xmlparser::XmlParser > parser;
    try
    {
        parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromFile( argv[1] );
    }
    catch ( std::exception &e )
    {
        printf( "At 1. parameter: xml-file\n" );
        printf( "%s\n", e.what() );
        return EXIT_FAILURE;
    }

    boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();
    boost::scoped_ptr< simulation::ElectricalSimulation< myMatrixType, double, false > > electricalSimulation;

    double stepTime = 0.01;
    try
    {

        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();
        std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
        electricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, double, false >( rootXmlNode, stepTime, 0.1, 0.1, &cells ) );
    }
    catch ( std::exception &e )
    {
        const char *message = e.what();
        printf( "Error while creating the equation systems\n" );
        printf( "%s", message );
        return EXIT_FAILURE;
    }
    catch ( ... )
    {
        printf( "Unknown error while creating the equation systems\n" );
        return EXIT_FAILURE;
    }
    electricalSimulation->mRootTwoPort->SetCurrent( ScalarUnit( "InputCurrent" ) );
    electricalSimulation->UpdateSystem();
    electricalSimulation->UpdateSystemValues();

    try
    {

        long maxTime = 5;
        try
        {
            boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();
            if ( rootXmlNode->HasElementDirectChild( "Options" ) )
            {
                if ( rootXmlNode->GetElementChild( "Options" )->HasElementDirectChild( "MaxTimeConversion" ) )
                {
                    maxTime = rootXmlNode->GetElementChild( "Options" )->GetElementIntValue( "MaxTimeConversion", 5 );
                }
            }

            parser.reset();
        }
        catch ( ... )
        {
        }

        if ( argc == 3 )
        {
            if ( std::string( argv[2] ) == "2" )
            {
                ES< 2 > exporter( electricalSimulation.get(), &std::cout, maxTime );
            }
            else if ( std::string( argv[2] ) == "1" )
                ES< 1 > exporter( electricalSimulation.get(), &std::cout, maxTime );
            else
            {
                ES< 3 > exporter( electricalSimulation.get(), &std::cout, maxTime );
            }
        }
        else
        {
            ES< 3 > exporter( electricalSimulation.get(), &std::cout, maxTime );
        }


        // exporter.BaseFunction();    // exporter.PrintVoltageMatrix();
        // exporter.CreateOdeFunction();
        std::cout << std::endl;
    }
    catch ( std::exception &e )
    {
        std::cerr << "Error in system export." << std::endl;
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
