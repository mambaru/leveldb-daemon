//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_MISC_HTTP_HEAD_HPP
#define FAS_MISC_HTTP_HEAD_HPP

#include <string>
#include <cstdlib>
#include <iterator>
#include <vector>
#include <list>
#include <algorithm>

namespace fas{ namespace misc{ namespace http {

  namespace detail
  {
    class base_header: 
      public std::vector<char>
    {
    public:
      typedef std::vector<char> data_type;

      static const char* version() { return "HTTP/1.1"; }

      static const char* desc(int code)
      {
        return code == 201 ? "Created" 
               : code==202 ? "Accepted" 
               : code==203 ? "Provisional Information" 
               : code==204 ? "No Content" 
               : code==300 ? "Multiple Choices" 
               : code==301 ? "Moved Permanently" 
               : code==302 ? "Moved Temporarily" 
               : code==303 ? "Method" 
               : code==304 ? "Not Modified" 
               : code==400 ? "Bad Request" 
               : code==401 ? "Unauthorized" 
               : code==402 ? "Payment Required" 
               : code==403 ? "Forbidden" 
               : code==404 ? "Not Found" 
               : code==405 ? "Method Not Allowed" 
               : code==406 ? "None Acceptable" 
               : code==407 ? "Proxy Authentication Required" 
               : code==408 ? "Request Timeout" 
               : code==409 ? "Conflict" 
               : code==410 ? "Gone" 
               : code==500 ? "Internal Server Error" 
               : code==501 ? "Not Implemented" 
               : code==502 ? "Bad Gateway" 
               : code==503 ? "Service Unavailable" 
               : code==504 ? "Gateway Timeout" 
               : "";
      }

    public:
      base_header& operator << (const std::string& h)
      {
        std::copy(h.begin(), h.end(), std::back_inserter(*this));
      }

      base_header& operator << (const char* d)
      {
        std::back_insert_iterator<base_header> itr = std::back_inserter(*this);
        for (;*d!='\0'; ++d)
          *(itr++) = *d;
        return *this;
      }

      base_header& operator << (char c)
      {
        this->push_back(c);
        return *this;
      }

      /**
    char *cur = beg;
    int del = 10;
    if (v==0)
      *(cur++)='0';
    else
      for( ; cur!=end && v ; ++cur, v/=10) 
        *cur = '0' + v%del;
    end = cur;
    for (;cur-beg>1;--cur, ++beg) 
    { *beg ^= *(cur-1); *(cur-1)^=*beg; *beg^=*(cur-1); }
    return end;
      */
      base_header& operator << (int i)
      {
        if ( i==0 ) 
          this->push_back('0');
        else
        {
          char ch[50];
          sprintf(ch, "%d", i);
          (*this)<<ch;
        }
        return *this;
      }
    };
  } 

class request_status: 
  public detail::base_header
{
  typedef detail::base_header base;
public:
  // @param sl - Строка-Статуса = Метод SP URI-Запроса SP Версия-HTTP CRLF
  request_status(const char* sl)  { (*this)<<sl; }

  template<typename I>
  request_status(I beg, I end)
    : data_type(beg, end)
  {}

  request_status(const char* method, const char* uri)
  {
    (*this)<<method<<' '<<uri<<' '<<version()<<'\r'<<'\n';
  }

  request_status(const std::string& method, const std::string& uri)
  {
    (*this)<<method<<' '<<uri<<' '<<version()<<'\r'<<'\n';
  }

  request_status(const char* method, const char* uri, const char* ver)
  {
    (*this)<<method<<' '<<uri<<' '<<ver<<'\r'<<'\n';
  }

  request_status(const std::string& method, const std::string& uri, const std::string& ver)
  {
    (*this)<<method<<' '<<uri<<' '<<ver<<'\r'<<'\n';
  }
};

class responce_status: 
  public detail::base_header
{
public:
  // @param sl - Строка-Статуса = Метод SP URI-Запроса SP Версия-HTTP CRLF
  responce_status(const char* sl)  { (*this)<<sl; }

  template<typename I>
  responce_status(I beg, I end)
    : data_type(beg, end)
  {}

  responce_status(const char* ver, int code, const char* desc)
  {
    (*this)<<ver<<' '<<code<<' '<<desc<<'\r'<<'\n';
  }

  responce_status(const std::string& ver, int code, const std::string& desc)
  {
    (*this)<<ver<<' '<<code<<' '<<desc<<'\r'<<'\n';
  }

  responce_status(int code, const char* desc)
  {
    (*this)<<version()<<' '<<code<<' '<<desc<<'\r'<<'\n';
  }

  responce_status(int code, const std::string& desc)
  {
    (*this)<<version()<<' '<<code<<' '<<desc<<'\r'<<'\n';
  }

  responce_status(int code)
  {
    (*this)<<version()<<' '<<code<<' '<<desc(code)<<'\r'<<'\n';
  }

  responce_status()
  {
    (*this)<<version()<<' '<<200<<' '<<desc(200)<<'\r'<<'\n';
  }
};

class header:
  public detail::base_header
{
public:

  header() { (*this)<<'\r'<<'\n';}

  header(const char* szrn)  { (*this)<<szrn; }

  template<typename I>
  header(I beg, I end)
    : data_type(beg, end)
  {}

  header(const char* name, const char* value)
  {
    (*this)<<name<<':'<<' '<<value<<'\r'<<'\n';
  }

  header(const std::string& name, const std::string& value)
  {
    (*this)<<name<<':'<<' '<<value<<'\r'<<'\n';
  }

  header(const char* name, int value)
  {
    (*this)<<name<<':'<<' '<<value<<'\r'<<'\n';
  }

  header(const std::string& name, int value)
  {
    (*this)<<name<<':'<<' '<<value<<'\r'<<'\n';
  }

  bool isempty() const 
  { 
    return (*this).size()==2 && (*this)[0]=='\r' && (*this)[1]=='\n';
  }

  const_iterator value_begin() const
  {
    const_iterator itr = std::find(begin(), end(), ':');
    if ( itr == end() )
      return itr;
    for (++itr; itr!=end() && *itr==' '; ++itr);
    return itr;
  }

  const_iterator value_end() const
  {
    if (size() < 2)
      return end();
    return end() - 2;
  }

  iterator value_begin() 
  {
    iterator itr = std::find(begin(), end(), ':');
    if ( itr == end() )
      return itr;
    for (++itr; itr!=end() && *itr==' '; ++itr);
    return itr;
  }

  iterator value_end()
  {
    if (size() < 2)
      return end();
    return end() - 2;
  }

  void set_value(const char* value)
  {
    resize(std::distance( this->begin(), this->value_begin() ));
    (*this)<<value<<'\r'<<'\n';
  }

  void set_value(int value)
  {
    resize(std::distance( this->begin(), this->value_begin() ));
    (*this)<<value<<'\r'<<'\n';
  }
};

class headers
  : public std::list<header>
{
  typedef std::list<header> base;

public:

  void set(const char *name, const char *value)
  {
    iterator itr = this->find(name);
    if (itr!=this->end())
      itr->set_value(value);
    else
      this->push_back(header(name, value));
  }

  void set(const char *name, int value)
  {
    iterator itr = this->find(name);
    if (itr!=this->end())
      itr->set_value(value);
    else
      this->push_back(header(name, value));
  }

  void set(const std::string& name, const std::string& value)
  {
    this->set(name.c_str(), value.c_str());
  }

  void set(const std::string& name, int value)
  {
    this->set(name.c_str(), value);
  }

  iterator find(const char* name)
  {
    base::iterator itr = begin();
    for ( ; itr != end(); ++itr )
      if ( _equal(name, itr->begin(), itr->end() ) )
        return itr;
    return end();
  }

  const_iterator find(const char* name) const
  {
    base::const_iterator itr = begin();
    for ( ; itr != end(); ++itr )
      if ( _equal(name, itr->begin(), itr->end() ) )
        return itr;
    return end();
  }

  template<typename I>
  void copy_to(I beg) const
  {
    const_iterator itr = begin();
    for (;itr!=end(); ++itr)
      std::copy(itr->begin(), itr->end(), beg);
  }


private:

  static bool _equal(const char* name, header::const_iterator beg, header::const_iterator end)
  {
    for ( ; *name!='\0' && beg!=end ; ++name, ++beg )
    {
      if ( (*name >= 'A' && *name <= 'Z') )
      {
        if (*name != *beg && *name + ('a' - 'A') != *beg  )
          return false;
      }
      else if (*name >= 'a' && *name <= 'z') 
      {
        if (*name != *beg && *name!= *beg + ('a' - 'A') )
          return false;
      }
      else
      {
        if (*name != *beg)
          return false;
      }
    }
    return *name=='\0';
  }
};

class request_headers
  : headers
{
public:
  request_status status_line;
  template<typename I>
  void copy_to(I beg) const
  {
    std::copy(status_line.begin(), status_line.end(), beg);
    headers::copy_to(beg);
    *(beg++)='\r';
    *(beg++)='\n';
  }
};

class responce_headers
  : headers
{
public:
  responce_status status_line;
  template<typename I>
  void copy_to(I beg) const
  {
    std::copy(status_line.begin(), status_line.end(), beg);
    headers::copy_to(beg);
    *(beg++)='\r';
    *(beg++)='\n';
  }
};

}}}

#endif
