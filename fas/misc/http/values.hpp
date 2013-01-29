#ifndef FAS_MISC_HTTP_VALUES_HPP
#define FAS_MISC_HTTP_VALUES_HPP

namespace fas{ namespace http{

struct empty_value { const char* value() const { return "";} };

struct null_value { const char* value() const { return 0;} };

struct root_uri_value { const char* value() const { return "/";} };

struct Chunked { const char* value() const { return "Chunked";}  };

struct Keep_Alive { const char *value() const { return "Keep-Alive";} };

struct POST { const char* value() const { return "POST";}  };

struct GET { const char* value() const { return "GET";}  };

struct HTTP11 { const char* value() const { return "HTTP/1.1";}  };

struct HTTP10 { const char* value() const { return "HTTP/1.0";}  };

struct application_xhtml_xml{ const char* value() const { return "application/xhtml+xml"; }};

/// Reason Phrase ( HTTP )

// struct OK { const char* value() const { return "OK";}  };



}}

#endif
