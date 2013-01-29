//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_AMF_TAGS_H
#define FAS_UTIL_AMF_TAGS_H

namespace fas{ namespace util{ namespace amf{

struct _server_handshake_{};
struct _process_first_handshake_{};
struct _process_second_handshake_{};
struct _chunk_splitter_{};
struct _process_content_{};
struct _chunk_collector_{};
struct _proxy_invocation_{};
struct _stub_invocation_{};
struct _chunk_prepare_{};
struct _invoke_connect_{};
struct _cleaner_{};
struct _write_{};

struct _client_handshake_{};
struct _process_client_handshake_{};

}}}

#endif
