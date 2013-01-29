//
// Author: Migashko Vladimir <migashko@mamba.ru>, (C) 2007
//

#ifndef MAMBA_GATEWAY_DB_MYMYSQL_HPP
#define MAMBA_GATEWAY_DB_MYMYSQL_HPP

#include <string>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <mysql/mysql.h>

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <memory>
#endif

#ifndef MYMYSQL_DEBUG
#define MYMYSQL_DEBUG 0
#endif

namespace mamba{ namespace gateway{ namespace db{

class mysql_row
{
  typedef mysql_row self;
  int _pos;
  unsigned long _length;
  MYSQL_ROW _row;
public:
  mysql_row() {
	  _row = 0;
  }
  void init( const MYSQL_ROW& row, unsigned long length)
  {
	if (MYMYSQL_DEBUG>=2 && length>0) std::cerr << "reading row" << std::endl;
    _row = row;
    _pos = 0;
    _length = length;
  }

  /*template<typename T>
  void operator >> ( T& value)
  {
    if ( _row[_pos] !=0 ) 
      value = boost::lexical_cast<T>( _row[_pos] );
    else 
      value = 0;
    _pos++;
  }*/
  inline char* operator[] (unsigned long int index) {
//	assert(index<_length);
	if (MYMYSQL_DEBUG>=3) std::cerr << "reading cell: " << _row[index] << std::endl;
  	return _row[index];
  }

  template<typename T> inline T each() {
	  T val;
	  (*this) >> val;
	  return val;
  }

  self& operator >> ( bool& value)
  {
    if ( _row[_pos] !=0 )
      value = std::atoi((*this)[_pos])?true:false;
    else
      value = false;
    _pos++;
    return (*this);
  }

  self& operator >> ( unsigned char& value)
  {
    if ( _row[_pos] !=0 )
      value = std::atoi((*this)[_pos]);
    else
      value = 0;
    _pos++;
    return (*this);
  }

  self& operator >> ( unsigned short& value)
  {
    if ( _row[_pos] !=0 )
      value = std::atoi((*this)[_pos]);
    else
      value = 0;
    _pos++;
    return (*this);
  }

  self& operator >> ( unsigned long& value)
  {
    if ( _row[_pos] !=0 )
      value = std::atol((*this)[_pos]);
    else
      value = 0;
    _pos++;
    return (*this);
  }

  self& operator >> ( unsigned long long& value)
  {
    if ( _row[_pos] !=0 )
      value = std::atoll((*this)[_pos]);
    else
      value = 0;
    _pos++;
    return (*this);
  }

  self& operator >> ( unsigned int& value)
  {
    if ( _row[_pos] !=0 )
      value = (int)std::atol((*this)[_pos]);
    else
      value = 0;
    _pos++;
    return (*this);
  }

  self& operator >> ( int& value)
  {
    if ( _row[_pos] !=0 )
      value = std::atoi((*this)[_pos]);
    else
      value = 0;
    _pos++;
    return (*this);
  }

  self& operator >> ( time_t& value)
  {
    if ( _row[_pos] !=0 )
      value = std::atoi((*this)[_pos]);
    else
      value = 0;
    _pos++;
    return (*this);
  }

  self& operator >> ( std::string& value)
  {
    if ( _row[_pos] !=0 )
      value = (*this)[_pos];
    else
      value = "";
    _pos++;
    return (*this);
  }
};

class mysql_result
{
  mutable MYSQL_RES* _result;
private:

  /** @link dependency */
  /*# mysql_row lnkmysql_row; */
public:
  operator bool () const 
  {
    if (_result==0) return false;
    return true;
  }
  
  mysql_result():_result(0){}

  mysql_result(const mysql_result& mr)
  {
    if (_result)
      mysql_free_result(_result);
    _result = mr._result;
    mr._result = 0;
  }

  explicit mysql_result( MYSQL_RES* result)
  {
    _result = result;
  }

  void  operator = ( MYSQL_RES* result)
  {
    if (_result)
      mysql_free_result(_result);
    _result = result;
  }
  
  void  operator = ( const mysql_result& mr )
  {
    if (_result)
      mysql_free_result(_result);
    _result = mr._result;
    mr._result = 0;
  }

  bool operator >> ( mysql_row& row)
  {
    if ( _result == 0) return false;
    MYSQL_ROW r = mysql_fetch_row(_result);
    if (r!=0)
      row.init( r, *mysql_fetch_lengths(_result) );
    else
      row.init( 0, 0 );
    return r!=0;
  }

  ~mysql_result()
  {
    if (_result!=0)
      mysql_free_result(_result);
  }
};

class mysql_connection
{
protected:
  MYSQL _mysql;
private:

  mysql_connection(const mysql_connection&) {}

  /** @link dependency */
  /*# mysql_result lnkmysql_result; */
public:

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  mysql_connection(mysql_connection&& other) {
	  std::swap(_mysql, other._mysql);
  }

  std::string escape(const std::string& str) {
      std::unique_ptr<char[]> buf(new char[2*str.size() + 1]);

      mysql_real_escape_string(&_mysql, buf.get(), str.data(), str.size());

      return std::string(buf.get());
  }

#endif


  mysql_connection()
  {
    mysql_init(&_mysql);
  }

  ~mysql_connection()
  {

    this->close();
  }

  void close()
  {
    mysql_close(&_mysql);
  }

  bool connect (const std::string& addr, const std::string& user,
           const std::string& pass, const std::string& db, unsigned short port = 0)
  {
    if (!mysql_real_connect(&_mysql, addr.c_str(),
         user.c_str(), pass.c_str(), db.c_str(), port, NULL, 0))
    {
      throw std::runtime_error(mysql_error(&_mysql));
    }

    if (0!=mysql_set_character_set(&_mysql, "utf8"))
    {
      std::cerr<<mysql_error(&_mysql)<<std::endl;
     // mlog(error, mysql_error(&_mysql));
      throw std::runtime_error(mysql_error(&_mysql));
    }
    return true;
  }

  inline mysql_result query(const std::string& query_string )
  {
	if (MYMYSQL_DEBUG>=1) std::cerr << query_string << std::endl;
    int res = mysql_query(&_mysql, query_string.c_str() );
    if ( res == 0 )
      return mysql_result(mysql_store_result(&_mysql));
    else 
    {
      std::cerr<<mysql_error(&_mysql);
      throw std::runtime_error(mysql_error(&_mysql));
    }

    return mysql_result(0);
  };
  inline unsigned long long affected_rows() {
    return mysql_affected_rows(&_mysql);
  }

};

}}}

#endif

