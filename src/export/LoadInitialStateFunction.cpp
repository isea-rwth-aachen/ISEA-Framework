#ifdef _SYMBOLIC_

#include "../misc/matrixInclude.h"
#include "../xmlparser/tinyxml2/xmlparserimpl.h"
#include "matrix_simulink_model.h"

void LoadInitialStateFunction( const char* xmlContent, const char* initialStateStruct )
{
    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromMem( xmlContent );
    visualizer::MatrixSimulinkModel< myMatrixType, double > matrixModel( parser.GetRoot(), "BuildMatrixModel", initialStateStruct );
    matrixModel.LoadInitialState();
}

#endif /* _SYMBOLIC_ */
