#ifndef FAS_MISC_HTTP_READER_BODY_READER_HPP
#define FAS_MISC_HTTP_READER_BODY_READER_HPP


#include "body_reader_aspect.hpp"

#include <stdexcept>
namespace fas{ namespace http{ namespace reader{

template<typename A = aa::aspect<> >
class body_reader
  : public aa::aspect_class<body_reader_aspect, A>
{
  bool _complete;
  bool _chunked;
  bool _multipart;
  int _content_length;
  const char *_data_begin;
  const char *_data_end;
  std::string _boundary;

public:

  body_reader(): _complete(false), _chunked(false), _multipart(false), _content_length(0), _data_begin(0), _data_end(0) {}

  void clear()
  {
    _complete = false;
    _chunked = false;
    _multipart = false;
    _content_length = 0;
    _data_begin = 0;
    _data_end = 0;
    _boundary = std::string();
    this->get_aspect().template get< _make_clean_ >()(*this);
  }

  void set_content_length( int v) { _content_length = v; }
  int get_content_length( ) const { return _content_length; }

  void set_chunked( bool v) { _chunked = v; }
  void set_multipart( std::string v ) { _multipart = true; _boundary = v;}

  operator bool () const { return _complete;}

  size_t size() const { return _data_begin==0 ? 0 : std::distance(_data_begin, _data_end);  }
  const char *begin() const {  return _data_begin; }
  const char *end() const {  return _data_end; }

  template<typename T>
  const char* operator()(T&, const char* beg, const char* end)
  {
    if (_complete) return beg;

    if (_multipart)
      return _content_read( this->get_aspect().template get<_multipart_reader_>(), beg, end );
    else if (_chunked)
      return _content_read( this->get_aspect().template get<_chunked_reader_>(), beg, end );
    else
      return _content_read( this->get_aspect().template get<_content_reader_>(), beg, end );
    return 0;
  }

  template<typename D>
  const char* _content_read( D& d, const char* beg, const char* end)
  {
    return d(*this, beg, end);
  }

  const char* _content_read( ap::empty_type, const char*, const char*)
  {
    throw std::logic_error("chunked processor not found");
    return 0;
  }

  void _content_ready(const char* beg, const char* end, bool complete)
  {
    _data_begin = beg;
    _data_end = end;
    _complete = complete;
  }

  void _content_no_ready()
  {
    _data_begin = 0;
    _data_end = 0;
    _complete = false;
  }
};

}}}
#endif
