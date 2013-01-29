#ifndef FAS_MISC_LOGGER_LOG_WRITER_HPP
#define FAS_MISC_LOGGER_LOG_WRITER_HPP

#include "ilog_manager.hpp"

#include <sstream>
#include <string>
#include <list>

namespace fas{ namespace logger{

class log_writer
{

public:

  log_writer(const char* head , ilog_manager* manager )
    : _head(head)
    , _manager( manager )
  {
  }

  ~log_writer() 
  {
    if ( !_fields.empty() )
    {
      if (_manager!=0 )
        _manager->write(_head, _fields);
    }
  }

  template<typename V>
  log_writer& operator + ( V v)
  {
    if ( _manager != 0 )
    {
      std::stringstream ss;
      ss<<v;
      if ( _fields.empty() )
        _fields.push_back( ss.str() );
      else
        _fields.back() += ss.str();
    }
    return *this;
  }

  template<typename V>
  log_writer& operator , ( V v)
  {
    if ( _manager != 0 )
    {
      std::stringstream ss;
      ss<<v;
      if ( _fields.empty() )
        _fields.push_back( ss.str() );
      else
        _fields.back() += ss.str();
    }
    return *this;
  }


  template<typename V>
  log_writer& operator << ( V v)
  {
    if ( _manager != 0 )
    {
      std::stringstream ss;
      ss<<v;
      _fields.push_back( ss.str() );
    }
    return *this;
  }

  // template<typename OS>
  log_writer& operator << ( std::ostream& ( *pf )(std::ostream&) )
  {
    if ( _manager != 0 )
    {
      std::stringstream ss;
      ss<<pf;
      _fields.push_back( ss.str() );
    }
    return *this;
  }

// ostream& ( *pf )(ostream&)
private:
  typedef std::list< std::string > data_type;
  data_type _fields;
  const char* _head;
  ilog_manager* _manager;
};


}}

#endif

