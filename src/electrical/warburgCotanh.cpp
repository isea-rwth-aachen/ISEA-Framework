/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : warburgCotanh.cpp
* Creation Date : 25-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "warburgCotanh.h"

namespace electrical
{

template < typename T >
WarburgCotanh< T >::WarburgCotanh( const bool observable, typename TwoPort< T >::DataType dataValues )
    : SerialTwoPort< T >( observable, dataValues )
{
}


template < typename T >
const char* WarburgCotanh< T >::GetName() const
{
    return "WarburgCotanh";
}

}    // namespace electrical

template class electrical::WarburgCotanh< myMatrixType >;
