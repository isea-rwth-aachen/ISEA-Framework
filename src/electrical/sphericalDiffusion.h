/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : sphericalDiffusion.h
* Creation Date : 22-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SERICALDIFFUSION_
#define _SERICALDIFFUSION_
// STD
#include <numeric>

// ETC
#include "../object/object.h"
#include "twoport.h"

extern template class electrical::TwoPort< myMatrixType >;
extern template class object::Object< double >;

namespace electrical
{

/// This class defines the spherical diffusion derived by Heiko Witzenhausen in particles mostly used on the anode
template < typename T = myMatrixType >
class SphericalDiffusion : public TwoPort< T >
{
    public:
    explicit SphericalDiffusion(
     boost::shared_ptr< object::Object< double > > rObj, boost::shared_ptr< object::Object< double > > tauObj,
     size_t rcElements, int hasCapacity = 1, const bool observable = false,
     typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) )
        : TwoPort< T >( observable, dataValues )
        , mHasCapacity( hasCapacity )
        , mUIDCount( rcElements )
        , mResidualResistanceFactor( std::accumulate( mRFactor.begin() + rcElements, mRFactor.end(), 0.0 ) )
        , mR( rObj )
        , mTau( tauObj ){};

    explicit SphericalDiffusion( object::Object< double >* rObj, object::Object< double >* tauObj, size_t rcElements,
                                 int hasCapacity = 1, const bool observable = false,
                                 typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) )
        : SphericalDiffusion( boost::shared_ptr< object::Object< double > >( rObj ),
                              boost::shared_ptr< object::Object< double > >( tauObj ), rcElements, hasCapacity,
                              observable, dataValues ){};    // delegating constructor

    virtual ~SphericalDiffusion(){};

    virtual ScalarUnit GetValue() const
    {
#ifndef _SYMBOLIC_
        return mR->GetValue();
#else
        return ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjR" );
#endif

    };    ///< Get Value of Object

    virtual void CalculateStateDependentValues();

    const object::Object< double >* GetElementObject() const
    {
        return mR.get();    ///< Get Object if instance is const
    }
    virtual const char* GetName() const { return "SphericalDiffusion"; };

    void SetVoltage();
    virtual T* GetVoltage();    ///< Return a Voltage over the TwoPort including children

    virtual void SetCurrent( const T currentval );    ///< Sets the current value
    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );

    const static std::vector< double > mRFactor;
    const static std::vector< double > mTauFactor;
    size_t GetUIDCount() const { return mUIDCount; };
    bool HasCapacity() const { return mHasCapacity; };
    virtual void SetResistanceFactor( const double factor );


    std::vector< boost::shared_ptr< object::Object< double > > > GetObjects() const { return {mR, mTau}; };

    private:
    int mHasCapacity = 0;
    const size_t mUIDCount = 0;

    const ScalarUnit mResidualResistanceFactor = ScalarUnit();
    T mConstVoltageVector = T();
    T mDGLPart = T();
    ScalarUnit mRvalue = 0;
    std::vector< size_t > mUIDs;
    boost::shared_ptr< object::Object< double > > mR;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
    boost::shared_ptr< object::Object< double > > mTau;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
    std::vector< ScalarUnit > mPowerVec;
};

}    // namespace electrical

#endif /* _SERICALDIFFUSION_ */
