#ifndef FAS_MISC_LOGGER_LOG_MANAGER_HPP
#define FAS_MISC_LOGGER_LOG_MANAGER_HPP

#include <fas/aop/aspect.hpp>
#include "ilog_manager.hpp"
#include "aspect.hpp"

#include <string>
#include <ostream>

namespace fas{ namespace logger{

namespace aa = ::fas::aop;

template<typename A = aa::aspect<> >
class log_manager
  : public ilog_manager
  , public aa::aspect_class<aspect, A >
{
public:
  virtual bool open(const std::string& path, std::ostream* os = 0) 
  {
    return this->get_aspect().template get<_file_>().open(path, os);
  };

  virtual void close() 
  {
    this->get_aspect().template get<_file_>().close();
  };

  virtual void write( const char* head, const std::list< std::string >& fields) 
  {
    this->get_aspect().template get<_writer_>()(*this, head, fields);
    this->get_aspect().template get<_flush_>()(*this);
  };

  virtual void flush() 
  {
    this->get_aspect().template get<_file_>().flush();
  };
private:
  
};

}}

#endif

