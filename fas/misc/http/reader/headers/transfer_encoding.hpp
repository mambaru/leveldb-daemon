#ifndef FAS_MISC_HTTP_READER_FIELDS_TRANSFER_ENCODING_HPP
#define FAS_MISC_HTTP_READER_FIELDS_TRANSFER_ENCODING_HPP

//#include "../ad_header.hpp"
#include "../header_equal_value.hpp"
#include "../../names.hpp"
#include "../../values.hpp"
#include "header.hpp"
/*#include "../../tags.hpp"

#include <fas/aop/advice.hpp>
*/

namespace fas{ namespace http{ namespace reader{

struct transfer_encoding: header< Transfer_Encoding, header_equal_value<Chunked> > {};
/*
namespace aa = ::fas::aop;

struct transfer_encoding_header
  : ad_header<Transfer_Encoding, header_equal_value<Chunked> >
{
};

struct transfer_encoding
  : aa::advice< transfer_encoding_header >
{
};
*/

}}}

#endif
