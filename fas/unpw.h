//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UNPW_H
#define FAS_UNPW_H

#ifdef HAVE_CONFIG_H

#include <fas/fas-config_win.h>

#endif // HAVE_CONFIG_H

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <process.h>

typedef unsigned int uint;
typedef SOCKET socket_t;
typedef int descriptor_t;

#pragma warning(disable : 4996)

#ifndef HAVE_SSIZE_T
typedef int ssize_t;
#endif

#include <time.h>

typedef long suseconds_t;
#if !defined(_WINSOCK2API_) && !defined(_WINSOCKAPI_)
  struct timeval 
  {
     long tv_sec;
     long tv_usec;
  };
#endif 

#include <sys\timeb.h>

inline int gettimeofday(struct timeval* tp, void* tzp) 
{
  struct _timeb timebuffer; 
  _ftime( &timebuffer ); 
  tp->tv_sec = static_cast<long>(timebuffer.time); 
  tp->tv_usec = timebuffer.millitm; 
  return 0;
}

#endif // FAS_UNPW_H
