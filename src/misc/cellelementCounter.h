/*
 * cellelementCounter.h
 *
 *  Created on: 14.01.2014
 *      Author: chris
 */

#ifndef CELLELEMENTCOUNTER_H_
#define CELLELEMENTCOUNTER_H_

#include "../electrical/twoport_withchild.h"

template < class MatrixType >
size_t CountCellelements( boost::shared_ptr< electrical::TwoPort< MatrixType > > tp )
{
    size_t count = 0;

    if ( tp->IsCellelement() )
        count = 1;

    if ( !tp->HasChildren() )
        return count;

    boost::shared_ptr< electrical::TwoPortWithChild< MatrixType > > tpwc(
     boost::static_pointer_cast< electrical::TwoPortWithChild< MatrixType > >( tp ) );

    for ( size_t i = 0; i < tpwc->size(); ++i )
        count += CountCellelements< MatrixType >( tpwc->shared_at( i ) );

    return count;
}

template < class MatrixType >
size_t CountCellelements( const electrical::TwoPort< MatrixType > *tp )
{
    size_t count = 0;

    if ( tp->IsCellelement() )
        count = 1;

    if ( !tp->HasChildren() )
        return count;

    auto tpwc = static_cast< const electrical::TwoPortWithChild< MatrixType > * >( tp );

    for ( size_t i = 0; i < tpwc->size(); ++i )
        count += CountCellelements< MatrixType >( tpwc->shared_at( i ).get() );

    return count;
}

#endif /* CELLELEMENTCOUNTER_H_ */
