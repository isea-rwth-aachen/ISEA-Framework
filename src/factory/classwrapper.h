/*
 * factorywrapper.h
 *
 *  Created on: 12.09.2013
 *      Author: chris
 */

#ifndef FACTORYWRAPPER_H_
#define FACTORYWRAPPER_H_

#include "../xmlparser/xmlparameter.h"
#include "../misc/cmp_str.h"
#include "../misc/StrCont.h"
#include "../states/thermal_state.h"
#include <map>
#include <boost/shared_ptr.hpp>

namespace factory
{
///Contains additional information created during run time for the factory
struct ArgumentType {} ;
typedef std::map<misc::StrCont, double, misc::cmp_str>::const_iterator DoubleMapConstIterator;

///This class is a base class for all classwrappers. A classwrapper is able to create an instance of a particular class which is derived from TBase.
template< typename TBase, typename Argument = ArgumentType >
class ClassWrapperBase
{
public:
    ClassWrapperBase()
    { }

    virtual ~ClassWrapperBase()
    { }

    ///Creates an instance of TBase.
    virtual boost::shared_ptr< TBase > CreateInstance( const xmlparser::XmlParameter* param, const Argument* arg = 0 ) = 0;

};

} /* namespace xmlfactory */
#endif /* FACTORYWRAPPER_H_ */
