/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : inductance.h
* Creation Date : 22-04-2015
* Last Modified : Di 08 Mär 2016 15:49:47 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _INDUCTANCE_
#define _INDUCTANCE_
#include "electricalelement.h"

namespace electrical
{
/// This class defines the basic behaviour of an Inductance  and does not work! It is just for display
template < typename T = myMatrixType >
class Inductance : public ElectricalElement< T >
{
    public:
    explicit Inductance( boost::shared_ptr< object::Object< double > > obj, const bool observable = false,
                         typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType(new ElectricalDataStruct< ScalarUnit >));
    explicit Inductance( object::Object< double >* obj, const bool observable = false,
                         typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType(new ElectricalDataStruct< ScalarUnit >));
    virtual ~Inductance(){};
    virtual T* GetVoltage();    ///< Abort the simulation

    virtual const char* GetName() const;
    virtual void SetResistanceFactor( const double factor );

    private:
    protected:
};

template < typename T >
Inductance< T >::Inductance( boost::shared_ptr< object::Object< double > > obj, const bool observable,
                             typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( obj, observable, dataValues )
{
}

template < typename T >
Inductance< T >::Inductance( object::Object< double >* obj, const bool observable, typename TwoPort< T >::DataType dataValues )
    : Inductance< T >( boost::shared_ptr< object::Object< double > >( obj ), observable, dataValues )
{
}

template < typename T >
T* Inductance< T >::GetVoltage()
{
    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoSimulationPossible", GetName() );
    return 0;
}

template < typename T >
const char* Inductance< T >::GetName() const
{
    return "Inductance";
}

template < typename T >
void Inductance< T >::SetResistanceFactor( const double factor )
{
    this->mObject->SetFactor( factor );
}
}
#endif /* _INDUCTANCE_ */
