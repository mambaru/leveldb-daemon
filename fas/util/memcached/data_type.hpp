//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_MEMCACHED_DATA_TYPE_HPP
#define FAS_UTIL_MEMCACHED_DATA_TYPE_HPP

namespace fas{ namespace util { namespace memcached{

// <command name> <key> <flags> <exptime> <bytes>\r\n
struct storage
{
  enum {set, add, replace} command;
  std::string key;
  std::vector<char> data;
  unsigned short flags;
  int exptime;
  int bytes;
};

/*VALUE <key> <flags> <bytes>\r\n
<data block>\r\n*/

struct value
{
  int bytes;
  unsigned short flags;
  std::string key;
  std::vector<char> data;
};

}}}

#endif
