/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : matlabExport.cpp
* Creation Date : 26-06-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#ifdef _SYMBOLIC_
#include "matlabExport.h"
#include "exportStructs/matlabExport.h"
#include "../misc/symbolicParser.h"
#include "symbolicExport.h"
/*


#include "../electrical/twoport_withchild.h"
*/

namespace symbolic
{
extern template struct symbolic::SymbolicParser< std::string::const_iterator >;
extern template class symbolic::SymbolicExport< std::string::const_iterator, symbolic::MatlabExportStruct >;

template class symbolic::MatlabExport< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator, symbolic::MatlabExportStruct >,
                                       symbolic::SymbolicParser< std::string::const_iterator >, 1 >;

template class symbolic::MatlabExport< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator, symbolic::MatlabExportStruct >,
                                       symbolic::SymbolicParser< std::string::const_iterator >, 2 >;

template class symbolic::MatlabExport< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator, symbolic::MatlabExportStruct >,
                                       symbolic::SymbolicParser< std::string::const_iterator >, 3 >;
}

#endif /* _SYMBOLIC_*/
