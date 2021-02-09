/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : rmphn.h
* Creation Date : 24-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _RMN_
#define _RMN_

#include "../object/object.h"
#include "serialtwoport.h"
#include <cmath>
#include <numeric>

extern template class electrical::SerialTwoPort< myMatrixType >;
extern template class object::Object< double >;

namespace electrical
{

/// DESCRIPTION
template < typename T = myMatrixType >
class Rmphn : public SerialTwoPort< T >
{
    public:
    explicit Rmphn( boost::shared_ptr< object::Object< double > > rCtObj,
                    boost::shared_ptr< object::Object< double > > rMpObj, const bool observable = false,
                    typename SerialTwoPort< T >::DataType dataValues = typename SerialTwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) )
        : SerialTwoPort< T >( observable, dataValues )
        , mR_Ct( rCtObj )
        , mRMp( rMpObj ){};


    virtual T* GetVoltage();    ///< returns the voltage over the RC element  Urc  = 1

    virtual ~Rmphn(){};
    virtual void CalculateStateDependentValues();

    virtual double GetTotalCapacity() const
    {
        return 0;
    }    ///< If the element has a capacity (e.g. Cellelement) we return  its capacity


    std::vector< boost::shared_ptr< object::Object< double > > > GetObjects() const { return {mRMp, mR_Ct}; };
    virtual const char* GetName() const { return "Rmphn"; };

    private:
    boost::shared_ptr< object::Object< double > > mR_Ct;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
    boost::shared_ptr< object::Object< double > > mRMp;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
    double mRser = 0;

    protected:
};

}    // namespace electrical
#endif /* _RMN_ */
