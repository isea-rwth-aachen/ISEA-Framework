/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : cpe.h
* Creation Date : 22-04-2015
* Last Modified : Di 08 Mär 2016 16:05:20 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ConstantPhaseElement_
#define _ConstantPhaseElement_
#include "electricalelement.h"

namespace electrical
{
/// This class defines the basic behaviour of a ConstantPhaseElement and does not work! It is just for display
template < typename T = myMatrixType >
class ConstantPhaseElement : public ElectricalElement< T >
{
    public:
    explicit ConstantPhaseElement( boost::shared_ptr< object::Object< double > > obj, const bool observable = false,
                                   typename TwoPort< T >::DataType dataValues =
                                    typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    explicit ConstantPhaseElement( object::Object< double >* obj, const bool observable = false,
                                   typename TwoPort< T >::DataType dataValues =
                                    typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    virtual ~ConstantPhaseElement(){};
    virtual T* GetVoltage();    ///< Abort the simulation
    virtual void SetResistanceFactor( const double factor );

    virtual const char* GetName() const;

    private:
    protected:
};

}    // namespace electrical
#endif /* _ConstantPhaseElement_ */
