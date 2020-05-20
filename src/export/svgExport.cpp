/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : svgExport.cpp
* Creation Date : 24-03-2015
* Last Modified : 29.07.2015 19:34:51
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef _DS1006
#include "svgExport.h"
#ifndef __NO_STRING__

template class SvgExport< myMatrixType, CELLLEVEL_DEPTH >;
template class SvgExport< myMatrixType, ELECTRICALELEMENTS_DEPTH >;
template class SvgExport< myMatrixType, ALL >;

#endif /* __NO_STRING__ */
#endif /* _DS1006 */
