/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : parallelrc.cpp
* Creation Date : 20-03-2013
* Last Modified : Mo 07 Okt 2013 11:10:44 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "paralleltwoport.h"

namespace electrical
{

template < typename T >
ParallelTwoPort< T >::ParallelTwoPort( const bool observable, typename TwoPort< T >::DataType dataValues )
    : TwoPortWithChild< T >( observable, dataValues )
{
}

template < typename T >
void ParallelTwoPort< T >::SetCurrent( const T current )
{
    TwoPort< T >::SetCurrent( current );

    if ( this->mChildren.empty() )
        return;

    T cur = current;
    for ( size_t i = 0; i < this->mChildren.size() - 1; ++i )
        cur.coeffRef( 0, this->mStateSystemGroup->mDglStateSystem.GetEquationCount() + mUIDs[i] ) = -1;

    ( *( this->mChildren.begin() ) )->SetCurrent( cur );


    for ( size_t i = 1; i < this->mChildren.size(); ++i )
    {
        cur.setZero();
        cur.coeffRef( 0, this->mStateSystemGroup->mDglStateSystem.GetEquationCount() + mUIDs[i - 1] ) = 1;
        this->mChildren[i]->SetCurrent( cur );
    }
}

template < typename T >
void ParallelTwoPort< T >::SetCurrent( const ScalarUnit currentval )
{
    TwoPort< T >::SetCurrent( currentval );
    if ( !this->mChildren.empty() )
        this->mChildren.front()->SetCurrent( currentval );
}

template < typename T >
size_t ParallelTwoPort< T >::GetParallelChildren() const
{
    return this->mChildren.size();
}

template < typename T >
void ParallelTwoPort< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{
    for ( size_t i = 0; i < this->mChildren.size(); ++i )
    {
        if ( i > 0 )
            mUIDs.push_back( stateSystemGroup->mAlgStateSystem.GetNewEquation() );
        this->mChildren[i]->SetSystem( stateSystemGroup );
    }
    TwoPort< T >::SetSystem( stateSystemGroup );
}

template < typename T >
T* ParallelTwoPort< T >::GetVoltage()
{
    if ( this->mChildren.size() == 0 )
        return TwoPort< T >::GetVoltage();
    this->mVoltage = *( this->mChildren.back()->GetVoltage() );
    for ( size_t i = 1; i < this->mChildren.size() - 1; ++i )
        this->mStateSystemGroup->mAlgStateSystem.AddEquations( mUIDs[i - 1],
                                                               this->mVoltage - *( this->mChildren[i]->GetVoltage() ) );
    if ( this->mChildren.size() > 1 )
        this->mStateSystemGroup->mAlgStateSystem.AddEquations( mUIDs.back(),
                                                               this->mVoltage - *( this->mChildren[0]->GetVoltage() ) );

    return TwoPort< T >::GetVoltage();
}

template < typename T >
double ParallelTwoPort< T >::GetTotalCapacity() const
{
    double returnCap = 0;
    for ( size_t i = 0; i < this->mChildren.size(); ++i )
    {
        returnCap += this->mChildren[i]->GetTotalCapacity();
    }
    return returnCap;
}

template < typename T >
const char* ParallelTwoPort< T >::GetName() const
{
    return "ParallelTwoPort";
}

}    // namespace electrical

template class electrical::ParallelTwoPort< myMatrixType >;
