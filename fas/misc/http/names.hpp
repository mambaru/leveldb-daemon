#ifndef FAS_MISC_HTTP_NAMES_HPP
#define FAS_MISC_HTTP_NAMES_HPP

/** Классы имен полей являются таже их тегами */
namespace fas{ namespace http{

struct Content_Length { const char* name() const { return "Content-Length";}  };
struct Content_Type { const char* name() const { return "Content-Type";}  };
struct Transfer_Encoding { const char* name() const { return "Transfer-Encoding";} };
struct Connection { const char *name() const { return "Connection";} };
struct Host { const char *name() const { return "Host";} };

}}

#endif
