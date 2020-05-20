/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : matlabExport.cpp
* Creation Date : 23-06-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "matlabExport.h"
#include "../symbolicExport.h"

namespace symbolic
{
template class SymbolicExport< std::string::const_iterator, MatlabExportStruct >;
}
