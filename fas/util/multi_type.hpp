//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_UTIL_MULTI_TYPE_HPP
#define FAS_UTIL_MULTI_TYPE_HPP

#include <map>
#include <string>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#define snprintf _snprintf
#endif
namespace fas{ namespace util {

enum mt_types { mt_bool, mt_char, mt_long, mt_double, mt_string, mt_nhash, mt_shash, mt_none };

template<mt_types t>
struct mt_traits;

class multi_type
{
  typedef std::map<int, multi_type> numeric_hash_type;
  typedef std::map<std::string, multi_type> string_hash_type;

  struct deleter 
  {
    template<typename T>
    void operator() (const T& t) {  /*delete t;*/ }

    template<typename F, typename S>
    void operator() (const std::pair<F,S>& t) {  /*delete t.second;*/ }
  };

  friend struct mt_traits<mt_bool>;
  friend struct mt_traits<mt_char>;
  friend struct mt_traits<mt_long>;
  friend struct mt_traits<mt_double>;
  friend struct mt_traits<mt_string>;
  friend struct mt_traits<mt_nhash>;
  friend struct mt_traits<mt_shash>;

public:

  ~multi_type() { this->clear(); }

  multi_type():_type(mt_none), _parent(0), _double(0.0){}

  explicit multi_type(multi_type* parent)
    : _type(mt_none)
    , _parent(parent)
    , _double(0.0)
  {}

  multi_type(const multi_type& at)
  {
    _type = mt_none;
    this->operator = (at);
  }

  multi_type(const multi_type& at, multi_type* parent)
  {
    _type = mt_none;
    this->operator = (at);
    _parent = parent;
  }

  mt_types get_type() const { return _type;}

  multi_type& operator = ( const multi_type& at)
  {
    this->clear();
    _type = at._type;
    _parent = 0;///at._parent;
    switch (_type)
    {
      case mt_bool: _bool = at._bool; break;
      case mt_char: _char = at._char; break;
      case mt_long: _long = at._long; break;
      case mt_double: _double = at._double; break;
      case mt_string:
        _string = at._string;
      break;
      case mt_nhash:
      {
        numeric_hash_type::const_iterator beg = at._nhash.begin();
        numeric_hash_type::const_iterator end = at._nhash.end();
        for (;beg!=end; ++beg)
          _nhash.insert( numeric_hash_type::value_type(beg->first, multi_type(beg->second, this) ) );
      }
      break;
      case mt_shash:
      {
//        _shash = new string_hash_type;
        string_hash_type::const_iterator beg = at._shash.begin();
        string_hash_type::const_iterator end = at._shash.end();
        for (;beg!=end; ++beg)
          _shash.insert( string_hash_type::value_type(beg->first, multi_type(beg->second, this) ) );
      }
      break;
      case mt_none: break;
    }
    return *this;
  }

  multi_type* parent() { return _parent;}
  const multi_type* parent() const { return _parent;}

  multi_type& operator= (bool value)
  {
    this->_check_clear_set(mt_bool);
    _bool = value;
    return *this;
  }

  multi_type& operator= (char value)
  {
    this->_check_clear_set(mt_char);
    _char = value;
    return *this;
  }

  multi_type& operator= (long value)
  {
    this->_check_clear_set(mt_long);
    _long = value;
    return *this;
  }

  template<typename T>
  multi_type& operator= (T value)
  {
    return this->operator= ( static_cast<long>(value) );
  }

  multi_type& operator= (double value)
  {
    this->_check_clear_set(mt_double);
    _double = value;
    return *this;
  }

  multi_type& operator = (float value)
  {
    return this->operator= ( static_cast<float>(value) );
  }

  multi_type& operator = (const std::string& value)
  {
    if (!this->_check_clear_set(mt_string))
      _string = value;
    return *this;
  }

  multi_type& operator = (const char* str)
  {
    if (!this->_check_clear_set(mt_string))
      _string = str;
    return *this;
  }

  void convert2(mt_types t) const
  {
    mt_types old_type = _type;
//    _type = t; основной тип не изменяем
    switch (t)
    {
      case mt_bool:
      {
        switch (old_type)
        {
          case mt_bool: return;
          case mt_none: _bool = false; return;
          case mt_char: _bool = _char!='\0' && _char!='0'; return;
          case mt_long: _bool = _long!=0L; return;
          case mt_double: _bool = _long!=0.0; return;
          case mt_string:
             _bool = !(_string.empty() || _string=="0" || _string=="false");
            return;
          default:
            throw std::runtime_error("bool conversion error");
        }
      }
      break;
      case mt_char:
      break;
      case mt_long:
        switch (old_type)
        {
          case mt_long: return;
          case mt_none: _long = 0L; return;
          case mt_bool: _long = _bool; return;
          case mt_char: _long = _char; return;
          case mt_double: _long = static_cast<long>(_double); return;
          case mt_string: _long = std::atoi(_string.c_str()); return;
          default:
            throw std::runtime_error("long conversion error");
        }
      break;
      case mt_double: 
        switch (old_type)
        {
          case mt_double: return;
          case mt_none: _double = 0.0L; return;
          case mt_bool: _double = _bool!=0; return;
          case mt_char: _double = _char; return;
          case mt_long: _double = _long; return;
          case mt_string: _double = std::atoi(_string.c_str()); return;
          default:
            throw std::runtime_error("double conversion error");
        }
      break;
      case mt_string: 
      {
        char ch[128]={0};
        switch (old_type)
        {
          case mt_string: return;
          case mt_none: _string.clear(); return;
          case mt_double: snprintf(ch, 128, "%f", _double); break;
          case mt_bool: snprintf(ch, 128, "%d", _bool); break;
          case mt_char: ch[0] = _char; break;
          case mt_long: snprintf(ch, 128, "%lu", _long); break;
          default:
            throw std::runtime_error("string conversion error");
        }
        _string = ch;
      }
      break;
      case mt_shash: break;
      case mt_nhash: break;
      default:;
    }
  }

  operator bool () const
  {
    if (_type != mt_bool)
      convert2(mt_bool);
      // throw std::runtime_error("multi_type is not bool");
    return _bool;
  }

  operator char () const
  {
    if (_type != mt_char)
      convert2(mt_char);
      // throw std::runtime_error("multi_type is not char");
    return _char;
  }

  operator short () const
  {
    if (_type != mt_long)
      convert2(mt_long);
      // throw std::runtime_error("multi_type is not long");
    return static_cast<short>(_long);
  }

  operator int () const
  {
    if (_type != mt_long)
      convert2(mt_long);
      // throw std::runtime_error("multi_type is not long");
    return static_cast<int>(_long);
  }

  operator unsigned int () const
  {
    if (_type != mt_long)
      convert2(mt_long);
      //throw std::runtime_error("multi_type is not long");
    return static_cast<unsigned int>(_long);
  }

  operator long () const
  {
    if (_type != mt_long)
      convert2(mt_long);
      // throw std::runtime_error("multi_type is not long");
    return static_cast<long>(_long);
  }

  operator double () const
  {
    if (_type != mt_double)
      convert2(mt_double);
      // throw std::runtime_error("multi_type is not double");
    return _double;
  }

  operator float () const
  {
    if (_type != mt_double)
      convert2(mt_double);
      // throw std::runtime_error("multi_type is not double");
    return static_cast<float>(_double);
  }

  operator std::string () const
  {
    if (_type != mt_string)
      convert2(mt_string);
      // throw std::runtime_error("multi_type is not string");
    return _string;
  }

  operator const std::string& () const
  {
    if (_type != mt_string)
      convert2(mt_string);
      // throw std::runtime_error("multi_type is not string");
    return _string;
  }

  operator std::string& ()
  {
    if (_type != mt_string)
      convert2(mt_string);
      // throw std::runtime_error("multi_type is not string");
    return _string;
  }


  multi_type& operator[](int i)
  {
    if ( _type == mt_shash)
    {
      char num[12];
      snprintf(num, 12, "%d", i);
      return this->operator[](num);
    }

    if ( !this->_check_clear_set( mt_nhash ) )
      /*_nhash = new numeric_hash_type*/;
    numeric_hash_type::iterator itr = _nhash.find(i);
    if ( itr == _nhash.end())
      itr = _nhash.insert( std::make_pair(i, multi_type(this)) ).first;
    _type = mt_nhash;
    return itr->second;
  }

  multi_type& operator[](const std::string& key)
  {
    if ( _type == mt_nhash)
      return this->operator[](std::atoi(key.c_str()));

    if ( !this->_check_clear_set( mt_shash ) )
      /*_shash = new string_hash_type*/;
    string_hash_type::iterator itr = _shash.find(key);
    if ( itr == _shash.end())
      itr = _shash.insert( std::make_pair(key, multi_type(this)) ).first;
    return itr->second;
  }

  multi_type& operator[](const char* key)
  {
    if ( _type == mt_nhash)
      return this->operator[](std::atoi(key));
    return this->operator[](std::string(key));
  }


  void clear()
  {
     switch (_type)
     {
       case mt_bool: break;
       case mt_char: break;
       case mt_long: break;
       case mt_double: break;
       case mt_string:
         //delete _string;
         break;
       case mt_nhash:
         std::for_each(_nhash.begin(), _nhash.end(), deleter() );
         //delete _nhash;
         break;
       case mt_shash:
         std::for_each(_shash.begin(), _shash.end(), deleter() );
         //delete _shash;
       break;
       case mt_none: break;
     }
  }



  template<mt_types t>
  typename mt_traits<t>::iterator begin()
  {
    return mt_traits<t>::get(*this).begin();
  };

  template<mt_types t>
  typename mt_traits<t>::const_iterator begin() const
  {
    return mt_traits<t>::get(*this).begin();
  };

  template<mt_types t>
  typename mt_traits<t>::iterator end()
  {
    return mt_traits<t>::get(*this).end();
  };

  template<mt_types t>
  typename mt_traits<t>::const_iterator end() const
  {
    return mt_traits<t>::get(*this).end();
  };

  template<mt_types t>
  typename mt_traits<t>::const_iterator find( const std::string& key  ) const
  {
    return mt_traits<t>::get(*this).find( key );
  };

  template<mt_types t>
  typename mt_traits<t>::iterator find( const std::string& key  )
  {
    return mt_traits<t>::get(*this).find( key );
  };

  template<mt_types t>
  typename mt_traits<t>::const_iterator find( int key  ) const
  {
    return mt_traits<t>::get(*this).find( key );
  };

  template<mt_types t>
  typename mt_traits<t>::iterator find( int key  )
  {
    return mt_traits<t>::get(*this).find( key );
  };

  void erase( const std::string& key  )
  {
    if (_type != mt_shash)
      throw std::runtime_error("multi_type is not string hash");
    _shash.erase(key);
  };

  void erase( int key  )
  {
    if (_type != mt_nhash)
      throw std::runtime_error("multi_type is not numeric hash");
    _nhash.erase(key);
  };


private:

  mt_types _type;
  multi_type* _parent;

  /*union
  {*/
  mutable bool _bool;
  mutable char _char;
  mutable long _long;
  mutable double _double;
  mutable std::string _string;
    numeric_hash_type _nhash;
    string_hash_type _shash;
  /*};*/

  bool _check_clear_set( mt_types t)
  {
    if (_type != t ) {
      if ( _type != mt_none )
        this->clear();
      _type = t;
      return false;
    }
    return true;
  }

};


template<>
struct mt_traits<mt_nhash>
{
  typedef multi_type::numeric_hash_type type;
  typedef multi_type::numeric_hash_type::iterator iterator;
  typedef multi_type::numeric_hash_type::const_iterator const_iterator;
  static multi_type::numeric_hash_type& get(multi_type& at)
  { return at._nhash;}
  static const multi_type::numeric_hash_type& get(const multi_type& at)
  { return at._nhash;}
};

template<>
struct mt_traits<mt_shash>
{
  typedef multi_type::string_hash_type type;
  typedef multi_type::string_hash_type::iterator iterator;
  typedef multi_type::string_hash_type::const_iterator const_iterator;
  static multi_type::string_hash_type& get(multi_type& at)
  { return at._shash;}
  static const multi_type::string_hash_type& get(const multi_type& at) 
  { return at._shash;}
};

static std::ostream& operator << ( std::ostream& o, const multi_type& mt)
{
  switch (mt.get_type() )
  {
    case mt_bool: o<<static_cast<bool>(mt); break;
    case mt_char: o<<static_cast<char>(mt); break;
    case mt_long: o<<static_cast<long>(mt); break;
    case mt_double: o<<static_cast<double>(mt); break;
    case mt_string: o<</*static_cast<std::string>*/(mt); break;
    case mt_nhash:
    {
      mt_traits<mt_nhash>::const_iterator beg = mt.begin<mt_nhash>();
      mt_traits<mt_nhash>::const_iterator end = mt.end<mt_nhash>();
      o<<'{';
      for (;beg!=end;++beg)
        o<<beg->first<<':'<<beg->second<<';';
      o<<'}';
    }
    break;
    case mt_shash:
    {
      mt_traits<mt_shash>::const_iterator beg = mt.begin<mt_shash>();
      mt_traits<mt_shash>::const_iterator end = mt.end<mt_shash>();
      o<<'{';
      for (;beg!=end;++beg)
        o<<beg->first<<':'<<beg->second<<';';
      o<<'}';
    }
    break;
    default: o<<"undefined"; break;
  }
  return o;
}


/*
enum mt_types { mt_bool, mt_char, mt_long, mt_double, mt_string, mt_nhash, mt_shash, mt_none };

template<mt_types t>
struct mt_traits;

class multi_type
{
  typedef std::map<int, multi_type*> numeric_hash_type;
  typedef std::map<std::string, multi_type*> string_hash_type;

  struct deleter 
  {
    template<typename T>
    void operator() (const T& t) {  delete t; }

    template<typename F, typename S>
    void operator() (const std::pair<F,S>& t) {  delete t.second; }
  };

  friend struct mt_traits<mt_bool>;
  friend struct mt_traits<mt_char>;
  friend struct mt_traits<mt_long>;
  friend struct mt_traits<mt_double>;
  friend struct mt_traits<mt_string>;
  friend struct mt_traits<mt_nhash>;
  friend struct mt_traits<mt_shash>;

public:

  ~multi_type() { this->clear(); }

  multi_type():_type(mt_none), _parent(0), _double(0.0){}

  multi_type(multi_type* parent)
    : _type(mt_none)
    , _parent(parent)
    , _double(0.0)
  {}

  multi_type(const multi_type& at)
  {
    _type = mt_none;
    this->operator = (at);
  }

  multi_type(const multi_type& at, multi_type* parent)
  {
    _type = mt_none;
    this->operator = (at);
    _parent = parent;
  }

  mt_types get_type() const { return _type;}

  multi_type& operator = ( const multi_type& at)
  {
    this->clear();
    _type = at._type;
    _parent = 0;///at._parent;
    switch (_type)
    {
      case mt_bool: _bool = at._bool; break;
      case mt_char: _char = at._char; break;
      case mt_long: _long = at._long; break;
      case mt_double: _double = at._double; break;
      case mt_string:
        _string = new std::string;
        *_string = *at._string;
      break;
      case mt_nhash:
      {
        _nhash = new numeric_hash_type;
        numeric_hash_type::const_iterator beg = at._nhash->begin();
        numeric_hash_type::const_iterator end = at._nhash->end();
        for (;beg!=end; ++beg)
          _nhash->insert( numeric_hash_type::value_type(beg->first, new multi_type(*beg->second, this) ) );
      }
      break;
      case mt_shash:
      {
        _shash = new string_hash_type;
        string_hash_type::const_iterator beg = at._shash->begin();
        string_hash_type::const_iterator end = at._shash->end();
        for (;beg!=end; ++beg)
          _shash->insert( string_hash_type::value_type(beg->first, new multi_type(*beg->second, this) ) );
      }
      break;
      case mt_none: break;
    }
    return *this;
  }

  multi_type* parent() { return _parent;}
  const multi_type* parent() const { return _parent;}

  multi_type& operator= (bool value)
  {
    this->_check_clear_set(mt_bool);
    _bool = value;
    return *this;
  }

  multi_type& operator= (char value)
  {
    this->_check_clear_set(mt_char);
    _char = value;
    return *this;
  }

  multi_type& operator= (long value)
  {
    this->_check_clear_set(mt_long);
    _long = value;
    return *this;
  }

  template<typename T>
  multi_type& operator= (T value)
  {
    return this->operator= ( static_cast<long>(value) );
  }

  multi_type& operator= (double value)
  {
    this->_check_clear_set(mt_double);
    _double = value;
    return *this;
  }

  multi_type& operator = (float value)
  {
    return this->operator= ( static_cast<float>(value) );
  }

  multi_type& operator = (const std::string& value)
  {
    if (!this->_check_clear_set(mt_string))
      _string = new std::string(value);
    return *this;
  }

  multi_type& operator = (const char* str)
  {
    if (!this->_check_clear_set(mt_string))
      _string = new std::string(str);
    return *this;
  }

  operator bool () const
  {
    if (_type != mt_bool)
      throw std::runtime_error("multi_type is not bool");
    return _bool;
  }

  operator char () const
  {
    if (_type != mt_char)
      throw std::runtime_error("multi_type is not char");
    return _char;
  }

  operator short () const
  {
    if (_type != mt_long)
      throw std::runtime_error("multi_type is not long");
    return static_cast<short>(_long);
  }

  operator int () const
  {
    if (_type != mt_long)
      throw std::runtime_error("multi_type is not long");
    return static_cast<int>(_long);
  }

  operator long () const
  {
    if (_type != mt_long)
      throw std::runtime_error("multi_type is not long");
    return static_cast<long>(_long);
  }

  operator double () const
  {
    if (_type != mt_double)
      throw std::runtime_error("multi_type is not double");
    return _double;
  }

  operator float () const
  {
    if (_type != mt_double)
      throw std::runtime_error("multi_type is not double");
    return static_cast<float>(_double);
  }

  multi_type& operator[](int i)
  {
    if ( _type == mt_shash)
    {
      char num[12];
      snprintf(num, 12, "%d", i);
      return this->operator[](num);
    }

    if ( !this->_check_clear_set( mt_nhash ) )
      _nhash = new numeric_hash_type;
    numeric_hash_type::iterator itr = _nhash->find(i);
    if ( itr == _nhash->end())
      itr = _nhash->insert( std::make_pair(i, new multi_type(this)) ).first;
    return *itr->second;
  }

  multi_type& operator[](const std::string& key)
  {
    if ( _type == mt_nhash)
      return this->operator[](std::atoi(key.c_str()));

    if ( !this->_check_clear_set( mt_shash ) )
      _shash = new string_hash_type;
    string_hash_type::iterator itr = _shash->find(key);
    if ( itr == _shash->end())
      itr = _shash->insert( std::make_pair(key, new multi_type(this)) ).first;
    return *itr->second;
  }

  multi_type& operator[](const char* key)
  {
    if ( _type == mt_nhash)
      return this->operator[](std::atoi(key));
    return this->operator[](std::string(key));
  }


  void clear()
  {
     switch (_type)
     {
       case mt_bool: break;
       case mt_char: break;
       case mt_long: break;
       case mt_double: break;
       case mt_string:
         delete _string;
         break;
       case mt_nhash:
         std::for_each(_nhash->begin(), _nhash->end(), deleter() );
         delete _nhash;
         break;
       case mt_shash:
         std::for_each(_shash->begin(), _shash->end(), deleter() );
         delete _shash;
       break;
       case mt_none: break;
     }
  }



  template<mt_types t>
  typename mt_traits<t>::iterator begin()
  {
    return mt_traits<t>::get(this).begin();
  };

  template<mt_types t>
  typename mt_traits<t>::const_iterator begin() const
  {
    return mt_traits<t>::get(this).begin();
  };

  template<mt_types t>
  typename mt_traits<t>::iterator end()
  {
    return mt_traits<t>::get(this).end();
  };

  template<mt_types t>
  typename mt_traits<t>::const_iterator end() const
  {
    return mt_traits<t>::get(this).end();
  };

  template<mt_types t>
  typename mt_traits<t>::const_iterator find( const std::string& key  ) const
  {
    return mt_traits<t>::get(this).find( key );
  };

  template<mt_types t>
  typename mt_traits<t>::iterator find( const std::string& key  )
  {
    return mt_traits<t>::get(this).find( key );
  };

  template<mt_types t>
  typename mt_traits<t>::const_iterator find( int key  ) const
  {
    return mt_traits<t>::get(this).find( key );
  };

  template<mt_types t>
  typename mt_traits<t>::iterator find( int key  )
  {
    return mt_traits<t>::get(this).find( key );
  };

  void erase( const std::string& key  )
  {
    if (_type != mt_shash)
      throw std::runtime_error("multi_type is not string hash");
    _shash->erase(key);
  };

  void erase( int key  )
  {
    if (_type != mt_nhash)
      throw std::runtime_error("multi_type is not numeric hash");
    _nhash->erase(key);
  };

  operator std::string () const
  {
    if (_type != mt_string)
      throw std::runtime_error("multi_type is not string");
    return *_string;
  }

  operator const std::string& () const
  {
    if (_type != mt_string)
      throw std::runtime_error("multi_type is not string");
    return *_string;
  }

  operator std::string& ()
  {
    if (_type != mt_string)
      throw std::runtime_error("multi_type is not string");
    return *_string;
  }

private:

  mt_types _type;
  multi_type* _parent;

  union
  {
    bool _bool;
    char _char;
    long _long;
    double _double;
    std::string* _string;
    numeric_hash_type* _nhash;
    string_hash_type* _shash;
  };

  bool _check_clear_set( mt_types t)
  {
    if (_type != t ) {
      if ( _type != mt_none )
        this->clear();
      _type = t;
      return false;
    }
    return true;
  }

};


template<>
struct mt_traits<mt_nhash>
{
  typedef multi_type::numeric_hash_type type;
  typedef multi_type::numeric_hash_type::iterator iterator;
  typedef multi_type::numeric_hash_type::const_iterator const_iterator;
  static multi_type::numeric_hash_type& get(multi_type* at)
  { return *at->_nhash;}
  static const multi_type::numeric_hash_type& get(const multi_type* at)
  { return *at->_nhash;}
};

template<>
struct mt_traits<mt_shash>
{
  typedef multi_type::string_hash_type type;
  typedef multi_type::string_hash_type::iterator iterator;
  typedef multi_type::string_hash_type::const_iterator const_iterator;
  static multi_type::string_hash_type& get(multi_type* at)
  { return *at->_shash;}
  static const multi_type::string_hash_type& get(const multi_type* at)
  { return *at->_shash;}
};

static std::ostream& operator << ( std::ostream& o, const multi_type& mt)
{
  switch (mt.get_type() )
  {
    case mt_bool: o<<static_cast<bool>(mt); break;
    case mt_char: o<<static_cast<char>(mt); break;
    case mt_long: o<<static_cast<long>(mt); break;
    case mt_double: o<<static_cast<double>(mt); break;
    case mt_string: o<<static_cast<std::string>(mt); break;
    case mt_nhash:
    {
      mt_traits<mt_nhash>::const_iterator beg = mt.begin<mt_nhash>();
      mt_traits<mt_nhash>::const_iterator end = mt.end<mt_nhash>();
      o<<'{';
      for (;beg!=end;++beg)
        o<<beg->first<<':'<<*beg->second<<';';
      o<<'}';
    }
    break;
    case mt_shash:
    {
      mt_traits<mt_shash>::const_iterator beg = mt.begin<mt_shash>();
      mt_traits<mt_shash>::const_iterator end = mt.end<mt_shash>();
      o<<'{';
      for (;beg!=end;++beg)
        o<<beg->first<<':'<<*beg->second<<';';
      o<<'}';
    }
    break;
    default: o<<"undefined"; break;
  }
  return o;
}
*/

}}

#endif
