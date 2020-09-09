/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : twoport_withchild.h
* Creation Date : 30-10-2012
* Last Modified : Di 08 Mär 2016 16:06:13 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TWOPORT_WITHCHILD_
#define _TWOPORT_WITHCHILD_
// STD
#include <vector>

// BOOST
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

// ETC
#include "twoport.h"

namespace electrical
{


/// This class is the basic class for a TwoPort that has children (Serial or Parallel). The offspring is beeing saved in
/// mChildren as shared pointer
template < typename T = myMatrixType >
class TwoPortWithChild : public TwoPort< T >
{
    friend class ::TestDaeSystem;

    public:
    explicit TwoPortWithChild( const bool observable = false,
                               typename TwoPort< T >::DataType dataValues =
                                typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    virtual ~TwoPortWithChild(){};
    virtual void AddChild( TwoPort< T >* newport );
    virtual void AddChild( boost::shared_ptr< TwoPort< T > > newport );

    virtual bool HasChildren() const;        ///< Return True if this TwoPort has children otherwise false
    virtual bool CanHaveChildren() const;    ///< Returns true

    virtual void SetCurrent( const T current );     ///< Set Current of all children to the same as this TwoPort
    virtual void SetCurrent( const ScalarUnit );    ///< Sets the current value

    virtual TwoPort< T >* operator[]( const size_t index );    ///< method to access the children without boundary check
    virtual TwoPort< T >* at( const size_t index );            ///< method to access the children with boundary check
    virtual const boost::shared_ptr< TwoPort< T > >& shared_at( const size_t index ) const;    ///< method to access the children with boundary check

    virtual const TwoPort< T >* operator[]( const size_t index ) const;    ///< method to access the children without boundary check
    virtual const TwoPort< T >* at( const size_t index ) const;    ///< method to access the children with boundary check

    /// return iterator to first child to enable iteration over the child vector
    const typename std::vector< boost::shared_ptr< TwoPort< T > > >::const_iterator begin() const;
    /// return iterator past the last child to enable iteration over the child vector
    const typename std::vector< boost::shared_ptr< TwoPort< T > > >::const_iterator end() const;

    size_t size() const;    ///< Return the amount of children

    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );

    virtual void CalculateStateDependentValues();

    virtual bool IsSerialTwoPort() const;

    virtual void ReCalculatePower();

    virtual const char* GetName() const;

    virtual void SetResistanceFactor( const double factor );

    private:
    protected:
    std::vector< boost::shared_ptr< TwoPort< T > > > mChildren;
};
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
#endif /* _TWOPORT_WITHCHILD_ */
