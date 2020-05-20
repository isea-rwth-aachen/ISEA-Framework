/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : chooseElectricSystemType.cpp
* Creation Date : 04-06-2017
* Last Modified : Do 15 Jun 2017 17:01:23 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "chooseElectricSystemType.h"

namespace factory
{
template boost::shared_ptr< systm::GeneralizedSystem< myMatrixType > >
ChooseElectricEquationSystemType( systm::StateSystemGroup< myMatrixType >* stateSystemGroup );
}
