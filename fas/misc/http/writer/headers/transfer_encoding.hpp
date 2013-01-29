#ifndef FAS_MISC_HTTP_WRITER_FIELDS_TRANSFER_ENCODING_HPP
#define FAS_MISC_HTTP_WRITER_FIELDS_TRANSFER_ENCODING_HPP


#include "../header_equal_value.hpp"
#include "../../names.hpp"
#include "../../values.hpp"
#include "header.hpp"

namespace fas{ namespace http{ namespace writer{

struct transfer_encoding: header< Transfer_Encoding, header_equal_value<Chunked> > {};

}}}

#endif
