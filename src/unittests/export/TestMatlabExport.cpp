/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestMatlabExport.cpp
* Creation Date : 23-06-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestMatlabExport.h"
#ifdef _SYMBOLIC_
#include <boost/scoped_ptr.hpp>
#include <boost/spirit/include/qi_parse.hpp>

#include "../../export/exportStructs/matlabExport.h"
#include "../../export/matlabExport.h"
#include "../../export/symbolicExport.h"

#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

#include "../../export/exportStructs/matlabExport.h"
#include "../../export/matlabExport.h"
#include "../../export/symbolicExport.h"
#include "../../misc/symbolicParser.h"
#include "../../electrical/electrical_simulation.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

#include <sstream>
#include <string>

typedef std::string::const_iterator iterator_type;
typedef symbolic::SymbolicExport< iterator_type, symbolic::MatlabExportStruct > sp;

extern template struct symbolic::SymbolicParser< std::string::const_iterator >;
extern template class symbolic::SymbolicExport< std::string::const_iterator, symbolic::MatlabExportStruct >;

extern template class symbolic::MatlabExport< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator, symbolic::MatlabExportStruct >,
                                              symbolic::SymbolicParser< std::string::const_iterator > >;


extern template class symbolic::SymbolicExport< std::string::const_iterator, symbolic::MatlabExportStruct >;

typedef MatlabExport< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator, symbolic::MatlabExportStruct >,
                      symbolic::SymbolicParser< std::string::const_iterator > >
 ES;
#endif

void TestMatlabExport::TestCreation()
{
#ifdef _SYMBOLIC_
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"ADD(3.0,MUL(1.0,ID10_ObjR))", "SUB(3.0,DIV(1.0,ID10_ObjR))",
                                        "DIV(3.0,POW(1.0,ID10_ObjR))"};
    std::vector< std::string > result = {"(3.0) + ((1.0) * (R10))", "(3.0) - ( (1.0) / (R10) )",
                                         " (3.0) / ( (1.0)^(R10) ) "};


    TS_ASSERT_EQUALS( input.size(), result.size() );

    for ( size_t i = 0; i < input.size(); ++i )
    {
        sp g;

        std::string str = input.at( i );

        iterator_type iter = str.begin();
        iterator_type end = str.end();
        std::string emp;
        bool r = phrase_parse( iter, end, g, space, emp );
        TS_ASSERT( r );
        TS_ASSERT_EQUALS( result.at( i ), emp );
    }
#endif
}
void TestMatlabExport::TestCreationFaulty()
{
#ifdef _SYMBOLIC_
    using namespace symbolic;
    using boost::spirit::ascii::space;

    std::vector< std::string > input = {
     "ADD(State1,MUL(InputCurrent,SUB(MUL(POW(MUL(ID1_ObjCT,ID1_ObjMP),0.5),DIV(1,TANH(POW(DIV(ID1_ObjMP,ID1_ObjCT),0."
     "5)))),ID1_ObjCT)))",
     "MUL(InputCurrent,SUB(MUL(POW(MUL(ID1_ObjCT,ID1_ObjMP),0.5),DIV(1,TANH(POW(DIV(ID1_ObjMP,ID1_ObjCT),0.5)))),ID1_"
     "ObjCT))",
     "SUB(MUL(POW(MUL(ID1_ObjCT,ID1_ObjMP),0.5),DIV(1,TANH(POW(DIV(ID1_ObjMP,ID1_ObjCT),0.5)))),ID1_ObjCT)",
     "MUL(POW(MUL(ID1_ObjCT,ID1_ObjMP),0.5),DIV(1,TANH(POW(DIV(ID1_ObjMP,ID1_ObjCT),0.5))))",
     "POW(MUL(ID1_ObjCT,ID1_ObjMP),0.5)",
     "MUL(ID1_ObjCT,ID1_ObjMP)",
     "ID1_ObjCT",
    };
    std::vector< std::string > result = input;


    TS_ASSERT_EQUALS( input.size(), result.size() );

    for ( size_t i = 0; i < input.size(); ++i )
    {
        sp g;

        std::string str = input.at( i );

        iterator_type iter = str.begin();
        iterator_type end = str.end();
        std::string emp;
        bool r = phrase_parse( iter, end, g, space, emp );
        if ( !r || iter != end )
        {
            std::cerr << "Error transforming string to tex:" << std::endl;
            while ( iter != end )
            {
                std::cerr << *iter;
                ++iter;
            }
            std::cerr << std::endl;

            TS_ASSERT( false );
        }
        // TS_ASSERT_EQUALS( result.at( i ), emp );
    }
#endif
}

void TestMatlabExport::TestSystems()
{
#ifdef _SYMBOLIC_
    std::vector< std::string > input = {
     "<?xml version='1.0'?>\\n\
     <Configuration>\\n\
     <Options/>\\n\
     <CustomDefinitions>\\n\
     <Cell class='CellElement'>\\n\
        <ThermalState class='ThermalState' cache='True'>\\n\
            <InitialTemperature>23</InitialTemperature>\\n\
        </ThermalState>\\n\
        <Soc class='Soc' cache='True'>\\n\
            <InitialCapacity>2.25</InitialCapacity>\\n\
            <InitialSoc>20</InitialSoc>\\n\
            <MeasurementPoints>5,10,20,23,35,50,65,80,90</MeasurementPoints>\\n\
        </Soc>\\n\
        <Children>\\n\
        <Ohm class='OhmicResistance'>\\n\
            <Object class='ConstObj'>\\n\
                <Value>10.0</Value>\\n\
            </Object>\\n\
        </Ohm>\\n\
        </Children>\\n\
     </Cell >\\n\
     <Cell2 class='CellElement'>\\n\
        <ThermalState class='ThermalState' cache='True'>\\n\
            <InitialTemperature>66</InitialTemperature>\\n\
        </ThermalState>\\n\
        <Soc class='Soc' cache='True'>\\n\
            <InitialCapacity>1.05</InitialCapacity>\\n\
            <InitialSoc>100</InitialSoc>\\n\
            <MeasurementPoints>5,10,20,23,35,50,65,80,90</MeasurementPoints>\\n\
        </Soc>\\n\
        <Children>\\n\
        <Ohm class='OhmicResistance'>\\n\
            <Object class='ConstObj'>\\n\
                <Value>10.0</Value>\\n\
            </Object>\\n\
        </Ohm>\\n\
        <Capacity class='Capacity'>\\n\
            <Object class='ConstObj'>\\n\
                <Value>10.0</Value>\\n\
            </Object>\\n\
        </Capacity>\\n\
        </Children>\\n\
     </Cell2 >\\n\
     </CustomDefinitions>\\n\
    <RootElement class='ParallelTwoPort'>\\n\
    <Children>\
        <cell1 ref='Cell'/>\
        <cell2 ref='Cell2'/>\
    </Children>\
    </RootElement>\
     </Configuration>"};
    std::vector< std::string > result = {
     "SOC = [ 20, 100 ]; \nCAP = [ 8100, 3780 ]; \nCAP_CUR = [ 1620, 3780 ]; \nT = [ 23, 66 ]; \n"};

    std::vector< std::string > stateVector = {"x = [ 0; 0 ]; \n"};
    std::vector< std::string > stateVectorUpdate = {"x = [ 0; 0 ]; \n"};


    for ( size_t i = 0; i < input.size(); ++i )
    {
        boost::scoped_ptr< xmlparser::XmlParser > parser;
        parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromMem( input[i].c_str() );

        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();
        boost::scoped_ptr< simulation::ElectricalSimulation< myMatrixType, double > > electricalSimulation;

        electricalSimulation.reset( new simulation::ElectricalSimulation< myMatrixType, double >( rootXmlNode, 0.01, 0.001, 0.1, 0 ) );

        std::stringstream test;
        ES exporter( electricalSimulation.get(), &test );
        test.clear();
        test.str( "" );
        exporter.CreateStates();
        TS_ASSERT_EQUALS( test.str(), result[i] );
        test.clear();
        test.str( "" );
        exporter.CreateStateVector();
        TS_ASSERT_EQUALS( test.str(), stateVector[i] );


        /*
            test.clear();
            test.str( "" );
            exporter.CreateLookupTable();
            std::cout << test.str() << std::endl;
            test.clear();
            test.str( "" );
            exporter.UpdateStates();
            std::cout << test.str() << std::endl;

            test.clear();
            test.str( "" );
            exporter.DefineDXFunction();
            std::cout << test.str() << std::endl;

            exporter.PrintBaseCurrent();
            exporter.PrintVoltageMatrix();

            test.clear();
            test.str( "" );
            exporter.CreateOdeFunction();
            std::cout << test.str() << std::endl;
            */
    }
#endif /* _SYMBOLIC_ */
}
