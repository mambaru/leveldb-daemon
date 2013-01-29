#ifndef FAS_MISC_LOGGER_HPP
#define FAS_MISC_LOGGER_HPP

#include <fas/misc/logger/log_writer.hpp>
#include <fas/misc/logger/log_registry.hpp>

namespace fas{ namespace logger{

template<typename Tg, int Mn>
inline log_writer logger()
{
  return log_writer( log_registry::head<Mn>(), log_registry::manager<Tg>() );
}


}}

#endif

