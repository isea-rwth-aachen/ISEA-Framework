/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestNumericExport.cpp
* Creation Date : 06-05-2016
* Last Modified : Di 20 Jun 2017 17:56:37 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestNumericExport.h"


// STD
#include <sstream>

// BOOST
#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

// ETC
#include "../../electrical/ohmicresistance.h"
#include "../../object/const_obj.h"

#include "../../export/numericExport.h"

#include "../../export/esbVisualizer.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

void TestNumericExport::testSingleElement()
{
#if !defined( __NO_STRING__ )

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromFile( "../../scenarios/internal_use_only/RC.xml" );
    auto root = parser->GetRoot();
    simulation::ElectricalSimulation< myMatrixType, ScalarUnit, false > sim( root, 0, 0, 0, nullptr );
    sim.mRootTwoPort->SetCurrent( 1 );
    sim.mRootTwoPort->UpdateStateSystemGroup();


    std::stringstream outstream;
    NumericExport<> exporter( &sim, &outstream );
    //    U = [ 0, 1, 0;1, 0, 0;0, 1, 0;];
    //    I = [ 0, 0, 1;0, -1, 1;0, 1, 0;];
    //    A_DAE = [ -0.05, -0.000205203;-1, 1;];
    //    C_DAE = [ 0.000205203;0;];
    //    A_ODE = [ -0.05, -0.000205203;-0.05, -0.000205203;];
    //    C_ODE = [ 0.000205203;0.000205203;];
    //    Eigen_Value = [  ( -0.0502052 + 0j ) ; ( 0 + 0j ) ;];

#ifndef _SYMBOLIC_
    std::vector< std::string > compareVector = {"U = [ 0, 1, 0;1, 0, 0;0, 1, 0;];",
                                                "I = [ 0, 0, 1;0, -1, 1;0, 1, 0;];",
                                                "A_DAE = [ -0.05, -0.000205203;-1, 1;];",
                                                "C_DAE = [ 0.000205203;0;];",
                                                "A_ODE = [ -0.05, -0.000205203;-0.05, -0.000205203;];",
                                                "C_ODE = [ 0.000205203;0.000205203;];",
                                                "Eigen_Value = [ ];"};
#else

    //    ID1_ObjBase = 1; ID2_ObjTau =  20; ID2_ObjBase = 0.00410406018886447320
    //    DIV = @(a,b) a./b;MUL = @(a,b) a.*b;SUB = @(a,b) a-b;ADD = @(a,b) a+b;NEG = @(a) -a;:
    std::vector< std::string > compareVector = {
     "U = [ 0, ID1_ObjR, 0;1.000000, 0, 0;0, ID1_ObjR, 0;];",
     "I = [ 0, 0, 1.000000;0, -1.000000, 1.000000;0, 1.000000, 0;];",
     "A_DAE = [ NEG(DIV(1.000000,ID2_ObjTau)), DIV(MUL(-1.000000,ID2_ObjR),ID2_ObjTau);NEG(1.000000), "
     "ID1_ObjR;];",
     "C_DAE = [ DIV(ID2_ObjR,ID2_ObjTau);0;];",
     "A_ODE = [ NEG(DIV(1.000000,ID2_ObjTau)), "
     "DIV(MUL(-1.000000,ID2_ObjR),ID2_ObjTau);DIV(MUL(MUL(NEG(1.000000),NEG(DIV(1.000000,ID2_ObjTau))),-1.000000),ID1_"
     "ObjR), DIV(MUL(MUL(NEG(1.000000),DIV(MUL(-1.000000,ID2_ObjR),ID2_ObjTau)),-1.000000),ID1_ObjR);];",
     "C_ODE = [ DIV( ID2_ObjR, ID2_ObjTau );DIV( MUL( MUL( NEG( 1.000000 ), DIV( ID2_ObjR, ID2_ObjTau ) ), -1.000000 "
     "), ID1_ObjR );];"};

#endif
    std::string tmpString;
    for ( size_t counter = 0; counter < compareVector.size(); ++counter )
    {
        std::getline( outstream, tmpString );
        TS_ASSERT_EQUALS( tmpString, compareVector.at( counter ) );
    }

#endif /* __NO_STRING__ */
}
