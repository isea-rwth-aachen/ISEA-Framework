/*
 * armadilloMatrix.h
 *
 *  Created on: 22.11.2013
 *      Author: chris
 */

#ifndef ARMADILLOMATRIX_H_
#define ARMADILLOMATRIX_H_

#ifdef _ARMADILLO_
#include "../../misc/matrixInclude.h"

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>

#include <sstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>



namespace boost {
namespace serialization {

template<class Archive>
void save(Archive & ar, const arma::Mat<double> & t, unsigned int /* version */)
{
    std::stringstream decompressed;

    arma::SpMat<double> sparseMatrix(t);

    sparseMatrix.quiet_save(decompressed);

#ifdef USE_ZLIB
    std::stringstream compressed;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    out.push(boost::iostreams::zlib_compressor());
    out.push(decompressed);
    boost::iostreams::copy(out, compressed);

    std::string mystring(compressed.str());
#else
    std::string mystring(decompressed.str());
#endif

    ar << mystring;
}

template<class Archive>
void load(Archive & ar, arma::Mat<double> & t, unsigned int /* version */)
{
    std::string mystring;
    ar >> mystring;

#ifdef USE_ZLIB
    std::stringstream compressed(mystring);
    std::stringstream decompressed;

    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    out.push(boost::iostreams::zlib_decompressor());
    out.push(compressed);
    boost::iostreams::copy(out, decompressed);
#else
    std::stringstream decompressed(mystring);
#endif

    arma::SpMat<double> sparseMatrix;
    sparseMatrix.quiet_load(decompressed);

    t = sparseMatrix;
}

template<class Archive>
void save(Archive & ar, const arma::sp_mat & t, unsigned int /* version */)
{
    std::stringstream decompressed;

    t.quiet_save(decompressed);

#ifdef USE_ZLIB
    std::stringstream compressed;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    out.push(boost::iostreams::zlib_compressor());
    out.push(decompressed);
    boost::iostreams::copy(out, compressed);

    std::string mystring(compressed.str());
#else
    std::string mystring(decompressed.str());
#endif

    ar << mystring;
}

template<class Archive>
void load(Archive & ar, arma::sp_mat & t, unsigned int /* version */)
{
    std::string mystring;
    ar >> mystring;

#ifdef USE_ZLIB
    std::stringstream compressed(mystring);
    std::stringstream decompressed;

    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    out.push(boost::iostreams::zlib_decompressor());
    out.push(compressed);
    boost::iostreams::copy(out, decompressed);
#else
    std::stringstream decompressed(mystring);
#endif

    t.quiet_load(decompressed);
}

} }

BOOST_SERIALIZATION_SPLIT_FREE(arma::Mat<double>)
BOOST_SERIALIZATION_SPLIT_FREE(arma::sp_mat)


#endif

#endif //ARMADILLOMATRIX_H_
