//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_MEMCACHED_STATES_HPP
#define FAS_UTIL_MEMCACHED_STATES_HPP

namespace fas{ namespace util { namespace memcached{

enum states
{
  MC_ready,
  MC_command,
  MC_storage,
  MC_get,
  MC_error
};

enum state_mode
{
  mode_server,
  mode_client
};

}}}

#endif
