#ifndef FAS_MISC_HTTP_AD_HTTP_WRITER_HPP
#define FAS_MISC_HTTP_AD_HTTP_WRITER_HPP

#include <fas/pattern/type_list.hpp>
#include <vector>
#include <algorithm>

#include "exceptions/server_error.hpp"
#include "writer/write_mode.hpp"


namespace fas{ namespace http {

namespace ap = ::fas::pattern;

template<typename H /* = writer::server_head_writer */, typename B/*  = writer::body_writer*/ >
class ad_writer
{
  typedef std::vector<char> data_type;
  typedef H headers_writer_type;
  typedef B body_writer_type;

  /*write_mode::type*/int _mode;
  bool _head_complete;
  bool _body_complete;

  bool _buffering;

  data_type _buffer;

public:

  ad_writer()
    : _mode( writer::write_mode::Content )
    , _head_complete(false)
    , _body_complete(false) 
    , _buffering(false)
  {}

  template<typename T>
  void clear(T&)
  {
    this->reset();
  }

  void reset() 
  {

    _mode = writer::write_mode::Content;
    _head_complete = false;
    _body_complete = false;
    _buffering  = false;
    _headers_writer.clear();
    _body_writer.clear();
    _buffer.clear();
  }

  void set_mode( int mode ) { _mode = mode; }

  int get_mode( ) const { return _mode;}

  headers_writer_type& headers() { return _headers_writer; }
  const headers_writer_type& headers() const { return _headers_writer; }

  body_writer_type& body() { return _body_writer; }
  const body_writer_type& body() const { return _body_writer; }

  bool is_buffering() const { return _buffering; }
  void start_buffering() {  _buffering = true; };
  void stop_buffering() {  _buffering = false; };
  void clear_buffer() { _buffer.clear(); }
  bool buffer_is_empty() const { return _buffer.empty(); }

  template<typename T>
  size_t flush(T& t)
  {

    if ( !_buffer.empty() )
      _write(t, &(_buffer[0]), _buffer.size() );
    else if (_buffering && _mode==writer::write_mode::Content )
      _write(t, 0, 0 );

    //else /*if (_buffering)*/ if (_mode==writer::write_mode::Content)
    //  _write(t, 0, 0 );

    size_t size = _buffer.size();
    _buffer.clear();
    return size;
  }

  template<typename T>
  void operator()(T& t, const char* d, size_t s)
  {
    if ( !_buffering )
    {
       _write(t, d, s);
    }
    else if (s > 0 )
    {
       std::copy(d, d + s, std::back_inserter(_buffer) );
    }
  }

private:
  template<typename T>
  void _write(T& t, const char* beg, size_t s)
  {
    std::vector<char> headers;
    std::vector<char> data;
    headers.reserve(256);
    data.reserve(256);

    // Сначала сериализуем тело (заодно правится заголовок, устанавливается content-length и пр.)
    _body_writer( t, _headers_writer, _mode, beg, s, std::back_inserter(data) );
    if ( !_headers_writer )
    {
      _headers_writer( t, std::back_inserter(headers) );
      if ( !headers.empty() )
        t.get_aspect().template get< _head_ready_ >()(t, &(headers[0]), headers.size()  );
      else
        t.get_aspect().template get< _head_ready_ >()(t, 0, 0 );
    }

    if ( !data.empty() )
      t.get_aspect().template get< _body_ready_ >()(t, &(data[0]), data.size()  );
    else
      t.get_aspect().template get< _body_ready_ >()(t, 0, 0 );
    if ( _body_writer )
      this->reset();
  }

private:
  data_type _head_data;
  data_type _body_data;
  headers_writer_type _headers_writer;
  body_writer_type _body_writer;
};

}}

#endif
