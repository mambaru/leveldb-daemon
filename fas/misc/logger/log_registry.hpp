#ifndef FAS_MISC_LOGGER_LOG_REGISTRY_HPP
#define FAS_MISC_LOGGER_LOG_REGISTRY_HPP

#include "ilog_manager.hpp"

namespace fas{ namespace logger{

class log_registry
{
public:
  template<int Mn>
  static const char* head( const char* head = 0 )
  {
    static const char* s_head = 0;
    if ( head!=0 )
      s_head = head;
    return s_head;
  }


  template<typename Tg>
  static ilog_manager* manager( ilog_manager* manager = 0, bool reset = false )
  {
    static ilog_manager* s_manager = 0;
    if ( manager!=0 )
      s_manager = manager;
    if ( reset )
      s_manager = 0;
    return s_manager;
  }
};

}}

#endif

