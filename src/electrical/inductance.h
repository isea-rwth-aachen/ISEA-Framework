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
                         typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    explicit Inductance( object::Object< double >* obj, const bool observable = false,
                         typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    virtual ~Inductance(){};
    virtual T* GetVoltage();    ///< Abort the simulation

    virtual const char* GetName() const;
    virtual void SetResistanceFactor( const double factor );

    private:
    protected:
};

}    // namespace electrical
#endif /* _INDUCTANCE_ */
