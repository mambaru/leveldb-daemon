#ifndef FAS_MISC_LOGGER_ADVICES_HPP
#define FAS_MISC_LOGGER_ADVICES_HPP

#include <ctime>
#include <fstream>
#include "tags.hpp"

namespace fas{ namespace logger{

struct ad_buffer
{
  std::string buffer;
  void append( const char* buf )
  {
     buffer+=buf;
  }

  void append( char ch )
  {
     buffer+=ch;
  }

  template<typename T>
  const char* operator()(T& )
  {
    return buffer.c_str();
  }

  void clear()
  {
    buffer.clear();
  }

};

struct ad_file
{
  std::ofstream file;
  std::ostream* os;
  bool open(const std::string& path, std::ostream* os)
  {
    this->os = os;
    file.close();
    if ( !path.empty() )
       file.open(path.c_str(), std::ios_base::app);
    return file!=0;
  }

  void close()
  {
    os = 0;
    file.close();
  }

  void flush()
  {
    file.flush();
  }

  template<typename T>
  void operator()(T& t, const char* d)
  {
    if (file) file << d;
    if (os) *os << d;
    t.get_aspect().template get<_on_writed_>()(t, d);
  }
};

struct ad_date
{
  template<typename T>
  void operator()(T& t)
  {
    char buf[128];
    time_t tt = time(0);
    struct tm *tmp = localtime(&tt);
    strftime( buf, 128, "%F", tmp);
    t.get_aspect().template get< _buffer_ >().append(buf);
  }
};

struct ad_time
{
  template<typename T>
  void operator()(T& t)
  {
    char buf[128];
    time_t tt = time(0);
    struct tm *tmp = localtime(&tt);
    strftime( buf, 128, "%T", tmp);
    t.get_aspect().template get< _buffer_ >().append(buf);
  }
};

template<char sep>
struct ad_sep
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get< _buffer_ >().append(sep);
  }
};

struct ad_head
{
  template<typename T>
  void operator()(T& t, const char* head)
  {
    if ( head!=0)
      t.get_aspect().template get< _buffer_ >().append(head);
  }
};

struct ad_fields
{
  template<typename T>
  void operator()(T& t, const std::list< std::string >& fields)
  {
    typedef std::list< std::string > filed_list;
    for ( filed_list::const_iterator itr = fields.begin(); itr!=fields.end(); )
    {
      t.get_aspect().template get< _buffer_ >().append(itr->c_str());
      ++itr;
      if ( itr!=fields.end() )
        t.get_aspect().template get< _field_separator_>()(t);
    }
  }
};


struct ad_writer
{
  template<typename T>
  void operator()(T& t, const char* head, const std::list< std::string >& fields)
  {
    t.get_aspect().template get< _date_>()(t);
    t.get_aspect().template get< _head_separator_>()(t);
    t.get_aspect().template get< _time_>()(t);
    // t.get_aspect().template get< _head_separator_>()(t);
    t.get_aspect().template get< _field_separator_>()(t);
    t.get_aspect().template get< _head_>()(t, head);
    t.get_aspect().template get< _field_separator_>()(t);
    t.get_aspect().template get< _fields_>()(t, fields);
    t.get_aspect().template get< _line_separator_>()(t);
  }
};

struct ad_flush
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template get<_file_>()(t, t.get_aspect().template get<_buffer_>()(t));
    t.get_aspect().template get<_buffer_>().clear();
    t.get_aspect().template get<_file_>().flush();
  }
};


}}

#endif

