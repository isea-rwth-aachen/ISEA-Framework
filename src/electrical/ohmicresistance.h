/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : ohmicresistance.h
* Creation Date : 30-10-2012
* Last Modified : Di 08 Mär 2016 16:04:58 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _OHMICRESISTANCE_
#define _OHMICRESISTANCE_
// STD
#include <cmath>

// ETC
#include "electricalelement.h"

namespace electrical
{
/// This TwoPort represents an ohmic resistance which can be influenced through the object::Object behaviour
template < typename T = myMatrixType >
class OhmicResistance : public ElectricalElement< T >
{
    public:
    explicit OhmicResistance( boost::shared_ptr< object::Object< double > > obj, const bool observable = false,
                              typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    explicit OhmicResistance( object::Object< double >* obj, const bool observable = false,
                              typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    virtual ~OhmicResistance(){};

    virtual T* GetVoltage();    ///< Returns i*R
    virtual void CalculateStateDependentValues();

    virtual ScalarUnit GetValue() const;    ///< Get Value of Object
    virtual const char* GetName() const;
    virtual void SetResistanceFactor( const double factor );

    private:
    protected:
};

}    // namespace electrical
#endif /* _OHMICRESISTANCE_ */
