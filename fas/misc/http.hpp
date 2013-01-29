#ifndef FAS_MISC_HTTP_HPP
#define FAS_MISC_HTTP_HPP

#include <vector>
#include <exception>
#include <algorithm>

namespace fas{ namespace misc{ namespace http{

typedef std::vector<char> data_type;

class initial_line
  : public data_type
{
public:
  typedef data_type::const_iterator const_iterator;

  void operator=(const std::string& value)
  {
    this->assign(value.begin(), value.end());
  }

  std::string request_method() const
  {
    return _first();
  }

  std::string request_uri() const
  {
    return _second();
  }

  std::string request_version() const
  {
     return _thrid();
  }

  std::string response_version() const
  {
    return _first();
  }

  std::string response_code() const
  {
    return _second();
  }

  std::string reason_phrase() const
  {
     return _thrid();
  }

private:

  std::string _first() const
  {
    return std::string(begin(), std::find(begin(), end(), ' ') );
  }

  std::string _second() const
  {
    const_iterator itr = std::find(begin(), end(), ' ');
    if (itr!=end()) ++itr;
    if ( itr==end() ) return std::string();
    return std::string(itr, std::find(itr, end(), ' ') );
  }

  std::string _thrid() const
  {
     const_iterator itr = std::find(begin(), end(), ' ');
     if (itr!=end()) ++itr;
     if ( itr==end() ) return std::string();
     itr = std::find(itr, end(), ' ');
     if ( itr!=end()) ++itr;
     if ( itr==end() ) return std::string();
     const_iterator itr_rn = std::find(itr, end(), '\r');
     if ( itr_rn==end() ) return std::string();
     return std::string(itr, itr_rn);
  }
};

/*
class request_line:
  public data_type
{
public:
  typedef data_type::const_iterator const_iterator;

  const_iterator method_begin() const { return begin();}
  const_iterator method_end() const { return std::find(begin(), end(), ' ');}

  const_iterator uri_begin() const 
  {
    const_iterator itr = method_end();
    if (itr!=end()) ++itr;
    return itr;
  }

  const_iterator uri_end() const { return std::find(rbegin(), rend(), ' ').base();}

  const_iterator version_begin() const 
  {
    const_iterator itr = uri_end();
    if (itr!=end()) ++itr;
    return itr;
  }
  const_iterator version_end() const 
  {
    if (size()<2) return end();
    return end() - 2;
  }

  void operator=(const std::string& value)
  {
    this->assign(value.begin(), value.end());
  }

};

class status_line:
  public data_type
{

public:
  status_line()
  {
    const char* default_status="HTTP/1.1 200 OK\r\n";
    this->assign(default_status, default_status + 17);
  }
  void operator=(const std::string& value)
  {
    this->assign(value.begin(), value.end());
  }
};
*/

class field:
  public data_type
{
public:

  field()
  {
    this->push_back('\r');
    this->push_back('\n');
  }

  field(const std::string& name, const std::string& value)
  {
    std::copy(name.begin(), name.end(), std::back_inserter(*this));
    this->push_back(':');
    this->push_back(' ');
    std::copy(value.begin(), value.end(), std::back_inserter(*this));
    this->push_back('\r');
    this->push_back('\n');
  }

  template<typename I>
  field(I beg, I end): data_type(beg, end) {}

  std::string get_name() const 
  {
    data_type::const_iterator itr = std::find(begin(), end(), ':');
    if (itr==end()) return std::string();
    return std::string(begin(), itr);
  }

  std::string get_value() const 
  {
    data_type::const_iterator itr = std::find(begin(), end(), ':');
    if (itr==end()) return std::string();
    ++itr;
    if (itr==end()) return std::string();
    for (;itr!=end() && *itr==' '; ++itr);
    if (itr==end()) return std::string();
    data_type::const_reverse_iterator ritr = rbegin();
    if (ritr==rend() || *ritr!='\n') return std::string();
    ++ritr;
    if (ritr==rend() || *ritr!='\r') return std::string();
    ++ritr;
    return std::string(itr, ritr.base() );
  }

  void set_name(const std::string& name) 
  {
    this->erase(begin(), std::find(begin(), end(), ':'));
    if (empty()) {
      push_back(':'); push_back(' ');
      push_back('\r'); push_back('\n');
    }
    std::copy(name.begin(), name.end(), std::inserter(*this, begin()));
  }

  void set_value(const std::string& value)
  {
    data_type::iterator b = std::find(begin(), end(), ':');
    if (b==end())
      throw std::runtime_error("http::head::set_value: cannot set value without name");
    ++b;
    for (;b!=end() && *b==' ';++b);
    if (b==end())
      throw std::runtime_error("http::head::set_value: cannot set value without name (invalid header)");
    data_type::iterator e = std::find(begin(), end(), '\r');
    if (e==end())
      throw std::runtime_error("http::head::set_value: cannot set value without name (invalid header)");
    this->erase(b, e);
    std::copy(value.begin(), value.end(), std::inserter(*this, b));
  }
};

class header:
  public data_type
{
  bool _chunked;

public:
  header():_chunked(false) {}

  void clear()
  {
    _chunked = false;
    data_type::clear();
  }

  void remove(const std::string& name)
  {
    data_type::iterator b = _find(name, begin(), end());
    if ( b!=end() )
      this->erase(b, _next(b, end()));
  }

  void set(const std::string& name, const std::string& value)
  {
    data_type::iterator b = _find(name, begin(), end());
    if ( b!=end() )
      this->erase(b, _next(b, end()));
    std::copy(name.begin(), name.end(), std::back_inserter(*this));
    this->push_back(':');
    this->push_back(' ');
    std::copy(value.begin(), value.end(), std::back_inserter(*this));
    this->push_back('\r');
    this->push_back('\n');
  }

  int get_content_length() const
  {
    data_type::const_iterator b = _find("Content-Length", begin(), end());
    if (b==end())
      return -1;
    b = std::find(b, end(), ':');
    if (b!=end()) ++b;
    for (;b!=end() && *b == ' '; ++b);
    if (b==end()) return -1;
    return std::atoi(&*b);
  }

  bool is_chunked() const
  {
    // return _chunked;
    return _find("Transfer-Encoding", begin(), end())!=end();
  }

  bool is_keep_alive() const
  {

    char con_str[] = "Connection: ";
    const_iterator itr = _find(con_str, begin(), end());
    if ( itr == end()  )
      return false;


    for (size_t i = 0; (i < sizeof(con_str)-1) && itr!=end(); ++i, ++itr); // -1 т.к. учитываем 0 на конце

    if ( itr==end() ) 
      return false;

    if ( *itr=='k' || *itr=='K' ) return true;
    return false;
  }

  void set_chunked( bool chunked = true)
  {
    _chunked = chunked;
    if ( chunked )
    {
      this->remove("Content-Length");
      this->set("Transfer-Encoding", "chunked");
    }
    else
    {
      this->remove("Transfer-Encoding");
    }
  }

  void set_content_length(int v)
  {
    _chunked = false;
    this->remove("Transfer-Encoding");
    char buf[32];
    char *beg = buf;
    char *cur = buf;
    char *end = buf + 32;
    int del = 10;
    if (v==0)
      *(cur++)='0';
    else
      for( ; cur!=end && v ; ++cur, v/=10) 
        *cur = '0' + v%del;
    end = cur;
    for (;cur-beg>1;--cur, ++beg) 
    { *beg ^= *(cur-1); *(cur-1)^=*beg; *beg^=*(cur-1); }
    this->set("Content-Length", std::string(buf, end));
  }

  field get(const std::string& name) const
  {
    data_type::const_iterator b = _find(name, begin(), end());
    if ( b!=end() )
      return field(b, _next(b, end()));
    return field();
  }

  bool is_complete() const
  {
    if (size()<4) return false;
    data_type::const_reverse_iterator r = rbegin();
    return *(r)=='\n' && *(r+1)=='\r' && *(r+2)=='\n' && *(r+3)=='\r';
  }

  void complete()
  {
    if (!is_complete())
    {
      this->push_back('\r');
      this->push_back('\n');
    }
  }
private:
  template<typename T>
  static T _find(const std::string& name, T beg, T end)
  {
    while (beg!=end && end==_equal(name.c_str(), beg, end))
    {
      beg = _next(beg, end);
    }
    return beg;
  }

  template<typename T>
  static T _next(T first, T end)
  {
    T itr = std::find(first, end, '\r');
    if (itr==end) return end;
    ++itr;
    if (itr==end || *itr!='\n') return end;
    return itr+1;
  }

  

  template<typename T>
  static T _equal(const char* name, T beg, T end)
  {
    for ( ; *name!='\0' && beg!=end ; ++name, ++beg )
    {
      if ( (*name >= 'A' && *name <= 'Z') )
      {
        if (*name != *beg && *name + ('a' - 'A') != *beg  )
          return end;
      }
      else if (*name >= 'a' && *name <= 'z') 
      {
        if (*name != *beg && *name!= *beg + ('a' - 'A') )
          return end;
      }
      else
      {
        if (*name != *beg)
          return end;
      }
    }

    if ( *name!='\0' )
      return end;

    for (;beg!=end && *beg!=':';++beg);
    if ( beg!=end ) ++beg;
    for (;beg!=end && *beg==' ';++beg);
    return beg;
  }

};


}}}

#endif
