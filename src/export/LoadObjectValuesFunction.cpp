#ifdef _SYMBOLIC_

#include "../misc/matrixInclude.h"
#include "../xmlparser/tinyxml2/xmlparserimpl.h"
#include "matrix_simulink_model.h"

void LoadObjectValuesFunction( const char* xmlContent, const char* modelLocation )
{
    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromMem( xmlContent );
    visualizer::MatrixSimulinkModel< myMatrixType, double > matrixModel( parser.GetRoot(), modelLocation,
                                                                         "initialState" );
    matrixModel.LoadObjectValues();
}

#endif /* _SYMBOLIC_ */