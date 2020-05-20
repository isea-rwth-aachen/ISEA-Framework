/*
 * rawsparsedata.h
 *
 *  Created on: 11.01.2014
 *      Author: chris
 */

#ifndef RAWSPARSEDATA_H_
#define RAWSPARSEDATA_H_

#ifdef _ARMADILLO_

#include <armadillo>

namespace systm
{

///Data container for sparse data. It can be merged into a single sparse matrix.
class RawSparseData
{
public:
    ///Create an empty container
    RawSparseData();

    ///Merge several containers into one
    RawSparseData(const std::vector< RawSparseData >& vector);

    ///Clears the added data. (no time consumption)
    void Clear();

    ///Add some data
    void Add(const arma::SpMat<double>& matrix, size_t rowOffset);

    ///Add some data
    void Add(const arma::SpMat<double>::const_iterator& beginIt, const arma::SpMat<double>::const_iterator& endIt, size_t rowOffset);

    ///Add some data
    void Add(const size_t row, const size_t col, const double value);

    ///Add some data
    void FillMatrix(arma::SpMat<double>& matrix, const arma::uword rows, const arma::uword cols);

    ///Extends the underlying storage capacity if needed
    void CheckAndAdaptSize(size_t additionalNeededElements = 0);

    ///Cell locations
    arma::umat mLocations;

    ///Cell values
    arma::colvec mValues;

    ///Number of added Cells
    size_t mElements;
};

} /* namespace systm */

#endif /* _ARMADILLO_ */

#endif /* RAWSPARSEDATA_H_ */
