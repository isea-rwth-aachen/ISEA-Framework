/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : system2Tex.cpp
* Creation Date : 20-06-2017
* Last Modified : 11.07.2018 21:10:16
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "../src/export/exportSystem.h"
#include "../src/export/symbolicExport.h"
#include "../src/misc/symbolicParser.h"
#include "../src/electrical/electrical_simulation.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include "standalone/standalone.h"
#include <cstdlib>
#include <iostream>
#include <string>

/*
extern template struct symbolic::SymbolicParser< std::string::const_iterator >;
extern template class symbolic::SymbolicExport< std::string::const_iterator, symbolic::StandardExportStruct >;

extern template class symbolic::ExportSystem< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator,
symbolic::StandardExportStruct >, symbolic::SymbolicParser< std::string::const_iterator > >;
                                              */

int main( int argc, char *argv[] )
{
    std::string xmlfile;
    standalone::Standalone app( "ISEA-Framework Dot Export Standalone" );
    app.mApp.add_option( "xml-file", xmlfile, "XML configuration file" )->required();
    if ( !app.ParseCommandLine( argc, argv ) )
        return EXIT_FAILURE;

    typedef ExportSystem< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator, symbolic::StandardExportStruct >,
                          symbolic::SymbolicParser< std::string::const_iterator > >
     ES;
    boost::scoped_ptr< xmlparser::XmlParser > parser;
    try
    {
        parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromFile( xmlfile.c_str() );
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
        parser.reset();
    }
    catch ( std::exception &e )
    {
        const char *message = e.what();
        printf( "Error while creating the equation systems\n" );
        printf( "%s", message );
        return EXIT_FAILURE;
    }
    electricalSimulation->mRootTwoPort->SetCurrent( ::ScalarUnit( "InputCurrent" ) );
    electricalSimulation->UpdateSystem();
    electricalSimulation->UpdateSystemValues();

    try
    {
        long timeout = 5;
        if ( argc == 3 )
        {
            timeout = boost::lexical_cast< long >( argv[2] );
        }
        ES exporter( electricalSimulation.get(), &std::cout, timeout );
        exporter.PrintVoltageMatrix();
    }
    catch ( std::exception &e )
    {
        std::cerr << "Error in system export." << std::endl;
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
