/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : exportSystem.cpp
* Creation Date : 19-06-2017
* Last Modified : Mo 19 Jun 2017 18:37:12 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifdef _SYMBOLIC_

#include "exportSystem.h"
#include "symbolicExport.h"
#include "exportStructs/texExport.h"
#include "../misc/symbolicParser.h"

namespace symbolic
{
	/*
extern template struct symbolic::SymbolicParser< std::string::const_iterator >;
extern template class symbolic::SymbolicExport< std::string::const_iterator, symbolic::TexExportStruct >;
extern template class symbolic::SymbolicExport< std::string::const_iterator, symbolic::StandardExportStruct >;

template class ExportSystem< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator, symbolic::TexExportStruct >,
                             symbolic::SymbolicParser< std::string::const_iterator > >;

template class ExportSystem< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator, symbolic::StandardExportStruct >,
                             symbolic::SymbolicParser< std::string::const_iterator > >;
							 */
}

#endif /* _SYMBOLIC_*/
