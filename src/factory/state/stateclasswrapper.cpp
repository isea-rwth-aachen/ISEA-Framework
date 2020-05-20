/*
 * stateclasswrapper.cpp
 *
 *  Created on: 17.09.2013
 *      Author: chris
 */

#include "stateclasswrapper.h"

#include "../factory.h"

namespace factory
{
template class Factory< state::State, ArgumentTypeState >;
} /* namespace factory */
