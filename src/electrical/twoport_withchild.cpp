#include "twoport_withchild.h"

namespace electrical
{

template < typename T >
TwoPortWithChild< T >::TwoPortWithChild( const bool observable, typename TwoPort< T >::DataType dataValues )
    : TwoPort< T >( observable, dataValues )
{
}

template < typename T >
void TwoPortWithChild< T >::CalculateStateDependentValues()
{
    if ( this->mObservable )
        TwoPort< T >::CalculateStateDependentValues();

    for ( size_t i = 0; i < this->mChildren.size(); ++i )
        this->mChildren[i]->CalculateStateDependentValues();

    this->ReCalculatePower();
}

template < typename T >
void TwoPortWithChild< T >::ReCalculatePower()
{
    this->mPowerValue = 0;
    for ( size_t i = 0; i < this->mChildren.size(); ++i )
        this->mPowerValue += this->mChildren[i]->GetPowerValue();
}

template < typename T >
void TwoPortWithChild< T >::SetCurrent( const T current )
{
    TwoPort< T >::SetCurrent( current );
    for ( size_t i = 0; i < this->mChildren.size(); ++i )
        this->mChildren[i]->SetCurrent( current );
}

template < typename T >
void TwoPortWithChild< T >::SetCurrent( const ScalarUnit currentval )
{
    TwoPort< T >::SetCurrent( currentval );
    for ( size_t i = 0; i < this->mChildren.size(); ++i )
        this->mChildren[i]->SetCurrent( currentval );
}

template < typename T >
size_t TwoPortWithChild< T >::size() const
{
    return mChildren.size();
}

template < typename T >
bool TwoPortWithChild< T >::HasChildren() const
{
    return !mChildren.empty();
}

template < typename T >
bool TwoPortWithChild< T >::CanHaveChildren() const
{
    return true;
}

template < typename T >
void TwoPortWithChild< T >::AddChild( TwoPort< T >* newport )
{
    boost::shared_ptr< TwoPort< T > > child( newport );
    mChildren.push_back( child );
}

template < typename T >
void TwoPortWithChild< T >::AddChild( boost::shared_ptr< TwoPort< T > > newport )
{
    mChildren.push_back( newport );
}

template < typename T >
const TwoPort< T >* TwoPortWithChild< T >::operator[]( const size_t index ) const
{
    return mChildren[index].get();
}

template < typename T >
TwoPort< T >* TwoPortWithChild< T >::operator[]( const size_t index )
{
    return mChildren[index].get();
}

template < typename T >
TwoPort< T >* TwoPortWithChild< T >::at( const size_t index )
{
    return mChildren.at( index ).get();
}

template < typename T >
const boost::shared_ptr< TwoPort< T > >& TwoPortWithChild< T >::shared_at( const size_t index ) const
{
    return mChildren.at( index );
}

template < typename T >
const TwoPort< T >* TwoPortWithChild< T >::at( const size_t index ) const
{
    return mChildren.at( index ).get();
}

template < typename T >
const typename std::vector< boost::shared_ptr< TwoPort< T > > >::const_iterator TwoPortWithChild< T >::begin() const
{
    return mChildren.begin();
}

template < typename T >
const typename std::vector< boost::shared_ptr< TwoPort< T > > >::const_iterator TwoPortWithChild< T >::end() const
{
    return mChildren.end();
}

template < typename T >
void TwoPortWithChild< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{
    for ( size_t i = 0; i < this->mChildren.size(); ++i )
        this->mChildren[i]->SetSystem( stateSystemGroup );
    TwoPort< T >::SetSystem( stateSystemGroup );
}

template < typename T >
bool TwoPortWithChild< T >::IsSerialTwoPort() const
{
    return false;
}

template < typename T >
const char* TwoPortWithChild< T >::GetName() const
{
    return "TwoPortWithChild";
}

template < typename T >
void TwoPortWithChild< T >::SetResistanceFactor( const double factor )
{
    BOOST_FOREACH ( boost::shared_ptr< TwoPort< T > > child, mChildren )
    {
        child->SetResistanceFactor( factor );
    }
}

}    // namespace electrical

template class electrical::TwoPortWithChild< myMatrixType >;