#ifdef _SYMBOLIC_

#include "CreateSimulinkModelFunction.h"
#include "../misc/matrixInclude.h"
#include "../electrical/electrical_simulation.h"
#include "../xmlparser/tinyxml2/xmlparserimpl.h"
#include "circuit_simulink_model.h"
#include "matrix_simulink_model.h"

void CreateSimulinkModelFunction( const char* xmlFileText, size_t modelType, const char* modelLocation, const char* initialStateStruct )
{
    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromMem( xmlFileText );
    if ( modelType == 0 )
    {
        simulation::ElectricalSimulation< myMatrixType, double, false > electricalSimualtion( parser.GetRoot(), 0.0,
                                                                                              0.0, 0.0, nullptr );
        visualizer::ElectricalSimulinkModel< myMatrixType, double > electricalSimulinkModel( electricalSimualtion.mRootTwoPort );
        electricalSimulinkModel.CreateModelWithAlgebraicLoops();
    }
    else
    {
        visualizer::MatrixSimulinkModel< myMatrixType, double > matrixModel( parser.GetRoot(), modelLocation, initialStateStruct );
        matrixModel.CreateMatrixModel();
    }
}

#endif /* _SYMBOLIC_ */