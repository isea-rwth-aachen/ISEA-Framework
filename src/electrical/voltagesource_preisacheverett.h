#ifndef _VOLTAGESOURCEPREISACHEVERETT_
#define _VOLTAGESOURCEPREISACHEVERETT_

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../state/soc.h"
#include "../object/object.h"
#include "electricalelement.h"
#include <vector>

class TestSurfaceSoc;

namespace electrical
{

/// This TwoPort describes a VoltageSource with the hysteresis
template < typename T = myMatrixType >
class VoltageSource_PreisachEverett : public ElectricalElement< T >
{
    friend class ::TestSurfaceSoc;

    public:
    explicit VoltageSource_PreisachEverett(boost::shared_ptr< state::Soc >& socState, double minOCV, boost::shared_ptr< object::Object< double > > obj,
        const bool observable = false, typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    explicit VoltageSource_PreisachEverett(boost::shared_ptr< state::Soc >& socState, double minOCV, object::Object< double >* obj, const bool observable = false,
                            typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    virtual ~VoltageSource_PreisachEverett(){};

    virtual T* GetVoltage(); 

    void addMaxima( double ExtremePoint );

    void addMinima( double ExtremePoint );
    
    void UpdateExtremumPoints();

    virtual void CalculateStateDependentValues();

    virtual const char* GetName() const;

    private:
    protected:
    std::vector< double > ExtremumPoints;    // vector to store the diffrent SOCs  that has not been wiped out
    std::vector< double > Everett; //vector to store The Everett value of each alpha and beta couple stored in Extremum points (E(alpha, beta))
    double OCVmin, OCV;
    boost::shared_ptr< state::Soc > mSOC;
};


}    // namespace electrical
#endif /* _VOLTAGESOURCEPREISACHEVERETT_ */