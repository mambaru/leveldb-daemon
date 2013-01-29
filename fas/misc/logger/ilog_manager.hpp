#ifndef FAS_MISC_LOGGER_ILOG_MANAGER_HPP
#define FAS_MISC_LOGGER_ILOG_MANAGER_HPP

#include <string>
#include <ostream>
#include <list>

namespace fas{ namespace logger{

struct ilog_manager
{
  virtual ~ilog_manager() {};
  virtual bool open(const std::string& path, std::ostream* os = 0) = 0;
  virtual void close() = 0;
  virtual void write( const char* head, const std::list< std::string >& fields) = 0;
  virtual void flush() = 0;
};

}}

#endif

