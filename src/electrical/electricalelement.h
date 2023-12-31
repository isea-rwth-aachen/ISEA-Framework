/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : electricalelement.h
* Creation Date : 30-10-2012
* Last Modified : Do 10 Mär 2016 10:40:24 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ELECTRICALELEMENT_
#define _ELECTRICALELEMENT_

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../object/object.h"
#include "twoport.h"

// Friend
class TestElectricalFactory;

namespace electrical
{
/// This class defines the basic behaviour of Electrical element, e.g. having an Object that defines the the behaviour
/// (const, lookup). This is saved in a shared_ptr mObject
template < typename T = myMatrixType >
class ElectricalElement : public TwoPort< T >
{
    friend class ::TestElectricalFactory;


    public:
    explicit ElectricalElement( boost::shared_ptr< object::Object< double > > obj, const bool observable = false,
                                typename TwoPort< T >::DataType dataValues =
                                 typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    explicit ElectricalElement( object::Object< double >* obj, const bool observable = false,
                                typename TwoPort< T >::DataType dataValues =
                                 typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    virtual ~ElectricalElement(){};

    virtual ScalarUnit GetValue() const;    ///< Get Value of Object

    object::Object< double >* operator()();                      ///< Get Object
    const object::Object< double >* GetElementObject() const;    ///< Get Object if instance is const
    virtual const char* GetName() const;

    private:
    protected:
    boost::shared_ptr< object::Object< double > > mObject;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
};

}    // namespace electrical
#endif /* _ELECTRICALELEMENT_ */
