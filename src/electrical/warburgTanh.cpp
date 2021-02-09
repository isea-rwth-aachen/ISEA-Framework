/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : warburgTanh.cpp
* Creation Date : 25-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "warburgTanh.h"

namespace electrical
{

template < typename T >
WarburgTanh< T >::WarburgTanh( const bool observable, typename TwoPort< T >::DataType dataValues )
    : SerialTwoPort< T >( observable, dataValues )
{
}

template < typename T >
const char* WarburgTanh< T >::GetName() const
{
    return "WarburgTanh";
}

}    // namespace electrical

template class electrical::WarburgTanh< myMatrixType >;
