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

}    // namespace electrical
#endif /* _TWOPORT_WITHCHILD_ */
