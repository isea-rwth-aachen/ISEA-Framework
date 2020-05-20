/*
 * chooseElectricSystemType.h
 *
 *  Created on: 14.09.2013
 *      Author: chris
 */

#ifndef _CHOOSEELECTRICSYSTEMTYPE_H
#define _CHOOSEELECTRICSYSTEMTYPE_H


//BOOST
#include <boost/shared_ptr.hpp>

//#include "stateSystemGroup.h"
#include "../../system/generalizedsystem.h"
#include "../../system/linAlg_sys.h"
#include "../../system/dae_sys.h"

namespace factory
{

template< typename MatrixT >
boost::shared_ptr< systm::GeneralizedSystem< MatrixT > > ChooseElectricEquationSystemType(systm::StateSystemGroup< MatrixT >* stateSystemGroup)
{
#ifndef _SYMBOLIC_
    if(stateSystemGroup->mDglStateSystem.GetEquationCount()<1)
    {
        return boost::shared_ptr< systm::GeneralizedSystem< MatrixT > >( new systm::LinAlgSystem< MatrixT >(stateSystemGroup) );
    }
#endif
    return boost::shared_ptr< systm::GeneralizedSystem< MatrixT > >( new systm::DifferentialAlgebraicSystem< MatrixT >(stateSystemGroup) );
}


} //namespace factory

#endif /*_CHOOSEELECTRICSYSTEMTYPE_H*/

