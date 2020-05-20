#ifndef _CELL_DATA_STRUCT_
#define _CELL_DATA_STRUCT_

#include "electrical_data_struct.h"

/// this struct stores all information about a CellElement for the aging calculation
struct CellDataStruct
{
    const boost::shared_ptr< ElectricalDataStruct< double > > electricalData;
};
#endif /* _CELL_DATA_STRUCT_ */
