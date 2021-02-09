/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : serialtwoport.cpp
* Creation Date : 30-10-2012
* Last Modified : Mi 02 Okt 2013 17:16:46 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "serialtwoport.h"

namespace electrical
{

template < typename T >
SerialTwoPort< T >::SerialTwoPort( const bool observable, typename TwoPort< T >::DataType dataValues )
    : TwoPortWithChild< T >( observable, dataValues )
{
}

template < typename T >
T* SerialTwoPort< T >::GetVoltage()
{
    this->mVoltage = *( this->mChildren.at( 0 )->GetVoltage() );
    for ( size_t i = 1; i < this->mChildren.size(); ++i )
        this->mVoltage += *( this->mChildren[i]->GetVoltage() );
    return TwoPort< T >::GetVoltage();
}

template < typename T >
bool SerialTwoPort< T >::IsSerialTwoPort() const
{
    return true;
}

template < typename T >
double SerialTwoPort< T >::GetTotalCapacity() const
{
    // if we have no children, we don't have any capacity --> 0
    if ( this->mChildren.size() == 0 )
        return 0;

    double returnCap = -1;

    // we determined that we have children. We now need to compare, which is the limiting child -->
    // find minimum capacity but larger than 0
    for ( size_t i = 0; i < this->mChildren.size(); ++i )
    {
        double comparison = this->mChildren[i]->GetTotalCapacity();
        if ( comparison > 0.0 && ( comparison < returnCap || returnCap < 0.0 ) )
            returnCap = comparison;
    }

    if ( returnCap > 0.0 )
        return returnCap;

    return 0.0;
}


template < typename T >
const char* SerialTwoPort< T >::GetName() const
{
    return "SerialTwoPort";
}
}    // namespace electrical

template class electrical::SerialTwoPort< myMatrixType >;
