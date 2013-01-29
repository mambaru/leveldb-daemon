//
// Author: Vladimir Migashko <migashko@faslib.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef FAS_MISC_JSON_HPP
#define FAS_MISC_JSON_HPP

/** Классы fas::misc::json предназначенны для максимально быстрой сереализации и десериализации
  * объектов json. Подходы для сереализации и десереализации различаются, но в обоих случаях 
  * объектная модель JSON не выстраивается. 
  */
namespace fas{ namespace misc{ namespace json{

 
/** Используется для сереализации и десериализации объектов типа value (string, number, true, false, null).
  * Для сериализации исползуются статические методы json_value::pack, а для 
 */
class json_value
{
public:

  /** Не копирует данные.
    * @param beg - указатель в буффере, где находится сериализованое значение (перед значением могут быть пробелы)
    * @param end - конец буфера
    */
	json_value(const char* beg, const char* end)
    : _beg(beg), _end(end) 
	{
		for (;beg!=end && *beg<=' ';++beg);
		_vbeg = beg;

    if (beg==end)
      return;

		if ( *_vbeg == '"')
		{
			for (++beg;beg!=end && *beg!='"'; ++beg)
				if (*beg=='\\') ++beg;
      if (beg==end)
        return;
			_vend = ++beg;
		}
		else if (*_vbeg == 'n')
			_vend = _vbeg + 4;
		else if (*_vbeg == 't' )
			_vend = _vbeg + 4;
		else if (*_vbeg == 'f' )
			_vend = _vbeg + 5;
		else
		{
			for (++beg;beg!=end && ( 
				(*beg>='0' && *beg<='9') || *beg=='-' 
				|| *beg=='.' || *beg=='e' 
				|| *beg=='E' || *beg=='+'); ++beg);
			_vend = beg;
		}

		for (;beg!=end && *beg<=' ';++beg);
		_end = beg;
	}

  /** смещение на первую позицию value*/
 	const char* raw_begin() const {return _vbeg;}
  /** смещение за последнюю позицию value*/
	const char* raw_end() const {return _vend;}
  /** смещение на начало буфера*/
	const char* buffer_begin() const {return _beg;}
  /** смещение за последнюю позицию value + пробелные символы 
    * до конца буфера или следующее значение*/
	const char* buffer_end() const {return _end;}

  /** @return false - если укзаный буфер не содежит валидного value */
  bool isempty () const { return _vbeg==_end;}
	bool isnull () const { return *_vbeg=='n';}
  bool isbool () const { return *_vbeg=='t' || *_vbeg=='f';}
  bool isstring () const { return *_vbeg=='"';}
  bool isnumber() const 
  { 
    return ((*_vbeg>='0' && *_vbeg<='9') || *_vbeg=='-' || *_vbeg=='.' || *_vbeg=='e' || *_vbeg=='E' || *_vbeg=='+');
  }



  
  operator bool() const { return *_vbeg == 't'; }
  operator int() const { return *_vbeg!='"' ? std::atoi(_vbeg) : std::atoi(_vbeg+1); }
  operator long() const { return *_vbeg!='"' ? std::atol(_vbeg) : std::atol(_vbeg+1); }
  operator double() const 
  { 
    return *_vbeg!='"' ? std::atof(_vbeg) : std::atof(_vbeg+1); 
  }
  operator std::string() const 
  {
    std::string str;
    std::back_insert_iterator<std::string> bii = std::back_inserter<std::string>(str);
    str.reserve(std::distance(_vbeg, _vend));
    const char* itr = _vbeg;
    if (*itr=='"') ++itr;

    for ( ; itr!=_vend && *itr!='"'; ++itr)
    {
      if (*itr=='\\' && ++itr!=_vend)
      {
        switch ( *itr )
        {
        case '"':
        case '\\':
        case '//': *(bii++) = *itr; break;
        case 't': *(bii++) = '\t'; break;
        case 'b': *(bii++) = '\b'; break;
        case 'n': *(bii++) = '\n'; break;
        case 'r': *(bii++) = '\r'; break;
        case 'f': *(bii++) = '\f'; break;
        case 'u': 
        default: *(bii++) = *itr; break;
        }
      }
      else
        *(bii++) = *itr;
    }
    return str;
  }



  
  static char* pack(char* beg, char* end, const std::string& v)
  {
    return pack(beg, end, v.c_str());
  }

  static char* pack(char* beg, char* end, bool v)
  {
    if (v)
    { *(beg++)='t'; *(beg++)='r'; *(beg++)='u'; *(beg++)='e';}
    else
    { *(beg++)='f'; *(beg++)='a'; *(beg++)='l'; *(beg++)='s';*(beg++)='e';}
    return beg;
  }
  static char* pack(char* beg, char* end, int v)
  {
    char *cur = beg;
    int del = 10;
    for( ; cur!=end && v ; ++cur, v/=10) 
      *cur = '0' + v%del;
    end = cur;
    for (;cur-beg>1;--cur, ++beg) 
    { *beg ^= *(cur-1); *(cur-1)^=*beg; *beg^=*(cur-1); }
    return end;
  }

  static char* pack(char* beg, char* end, long v)
  {
    char *cur = beg;
    long del = 10;
    for( ; cur!=end && v ; ++cur, v/=10) 
      *cur = '0' + v%del;
    end = cur;
    for (;cur-beg>1;--cur, ++beg) 
    { *beg ^= *(cur-1); *(cur-1)^=*beg; *beg^=*(cur-1); }
    return end;
  }

  static char* pack(char* beg, char* end, double v)
  {
    if ( v > 0.000000001 && v < 1000000000.0
         || v > -1000000000.0 && v < -0.000000001)
    {
      long i = (long)v;
      beg = pack(beg, end, i);
      *(beg++) = '.';
      i = (v-i)*1000000000;
      for ( ; i%10==0; i/=10 );
      beg = pack(beg, end, i );
    }
    else
      beg += std::sprintf(beg, "%e", v);
    return beg;
  }

  static char* pack(char* beg, char* end, const char* v)
  {
    *(beg++)='"';
    for (;*v!='\0' && beg!=end;++beg, ++v)
    {
      switch (*v)
      {
        case '"':
        case '\\':
        case '/': *(beg++)='\\'; *beg = *v; break;
        case '\t': *(beg++)='\\'; *beg = 't'; break;
        case '\b': *(beg++)='\\'; *beg = 'b'; break;
        case '\r': *(beg++)='\\'; *beg = 'r'; break;
        case '\n': *(beg++)='\\'; *beg = 'n'; break;
        case '\f': *(beg++)='\\'; *beg = 'f'; break;
        default: *beg = *v; break;
      }
    }
    *(beg++)='"';
    return beg;
  }

private:

  const char* _beg;
	const char* _end;
	const char* _vbeg;
	const char* _vend;
};


/**
  *
  */
class json_member
{
	json_value _name;
	const char* _beg;
	mutable const char* _end; // определяеттся только после вызова value<>
	const char* _colon; // :
	const char* _mbeg; // member begin
	const char* _vbeg;
	mutable const char* _vend; // определяеттся только после вызова value<>
	const char* _last;
public:

  json_member(const char* beg, const char* end)
		: _name(beg, end), _beg(0), _end(0), _colon(0), _mbeg(0), _vbeg(0), _vend(0), _last(end)
	{
		_beg = _name.buffer_begin();
		_vbeg = _name.raw_begin();
		_colon = _name.buffer_end(); 
    if ( _colon==end )
      return;
		if (*_colon!=':')
		 	;
		_mbeg = _colon+1;
		for (;_mbeg!=end && *_mbeg<=' ';++_mbeg);
	}


  const char* raw_begin() const {return _vbeg;}

	const char* raw_end() const { 
		if ( _vend == 0) _init();
		return _vend;	
	}

	const char* buffer_begin() const {return _beg;}

	const char* buffer_end() const 	{ 
		if ( _vend == 0)
			_init();
		return _end;
	}

  std::string sname() const { return _name;}

	const json_value& name() const { return _name; }

  template<typename T>
  T value() const	{ return T(_colon + 1, _last); 	}

	bool isempty() const { return _mbeg==_end;}
	bool isnull() const { return *_mbeg=='n';}
	bool isarray() const { return *_mbeg=='[';}
	bool isobject() const { return *_mbeg=='{';}
  bool isvalue() const { return !isempty() && !isnull() && !isarray() && !isobject(); }

private:
	void _init() const;
};


/**
  *
  */
template<typename MT, char L, char R>
class json_iterator
{
public:
  typedef MT json_type;
private:
	mutable const char* _beg;
	mutable const char* _last;
	mutable json_type _member;
  typedef json_iterator<MT, L, R> self_type;
public:

	json_iterator(const char* beg, const char* end)
    : _beg(beg), _last(end)
    , _member(beg, end) 
  {}

	json_iterator(const char* beg)
    : _beg(beg), _last(beg)
    , _member(beg, beg) 
  {}

	const json_type& operator*() const { return _member; }

  const json_type* operator->() const { return &_member; }

  operator bool () const { return *_beg!=R && _beg!=_last; }

	const self_type& operator++() const
	{
		const char* next = _member.buffer_end();

		if ( *next == R )
    {
			_beg = next;
			_member = json_type(_beg+1,_beg+1);
			return *this;
    }

		if ( *next!=',')
			;
		
    _beg = next;
		_member = json_type( next + 1, _last);
		return *this;
  }
};


/**
  *
  */
template<typename MT, char L, char R>
class json_complex
{
	const char* _beg;
	const char* _vbeg;
	mutable const char* _end;
	mutable const char* _vend;
	const char* _last;

public:
  typedef MT json_type;

	typedef json_iterator<MT, L, R> iterator;
	typedef const json_iterator<MT, L, R> const_iterator;

	const char* raw_begin() const {return _vbeg;}

	const char* raw_end() const { 
		if ( _vend == 0) _init();
		return _vend;	
	}

	const char* buffer_begin() const {return _beg;}

	const char* buffer_end() const 	{ 
		if ( _vend == 0)
			_init();
		return _end;
	}

	json_complex(const char* beg, const char* end): _beg(beg), _vbeg(0), _end(0), _vend(0),_last(end) 
	{
		for (;beg!=end && *beg<=' ';++beg);
    _vbeg = beg;
    if (beg==end)
      return;

		if (*_vbeg!=L)
			;
	}

	iterator begin() const
	{
		return iterator(_vbeg+1, _last);
	}

private:

  void _init() const
	{
		iterator itr_beg = begin();
		for (;itr_beg;) 
    {
      _end = (*itr_beg).buffer_end();
      _vend = (*itr_beg).raw_end();
      ++itr_beg;
    }
    if (_vend==_last)
      ;
    if (*_vend==R)
      _vend++;
    if (*_end==R)
    _end++;
	}
};


/**
  *
  */
class json_object: public json_complex<json_member, '{', '}'> 
{
public:
  json_object(const char* beg, const char* end):  json_complex(beg, end) {}
};


/**
  *
  */
class json_array: public json_complex<json_value, '[', ']'> 
{
public:
  json_array(const char* beg, const char* end):  json_complex(beg, end) {}

};


class json_pack
{
  char* _beg;
  char* _end;
public:
  json_pack(char* beg, char* end):_beg(beg),_end(end){};
  
  json_pack& operator << (char sep)
  {
    *(_beg++) = sep;
    (*_beg) = '\0';
    return *this;
  }

  json_pack& operator << (bool v)
  {
    _beg = json_value::pack(_beg, _end, v);
    return *this;
  }

  json_pack& operator << (int v)
  {
    _beg = json_value::pack(_beg, _end, v);
    return *this;
  }

  json_pack& operator << (long v)
  {
    _beg = json_value::pack(_beg, _end, v);
    return *this;
  }

  json_pack& operator << (double v)
  {
    _beg = json_value::pack(_beg, _end, v);
    return *this;
  }

  json_pack& operator << (const std::string& str)
  {
    _beg = json_value::pack(_beg, _end, str);
    (*_beg) = '\0';
    return *this;
  }

  const char* data() const { return _beg;}

};

void json_member::_init() const
{
  if ( !isempty() )
  {
	  if ( isobject() )
	  {
		  json_object j = value<json_object>();
		  _vend = j.raw_end();
		  _end = j.buffer_end();
	  }
	  else if( isarray() )
	  {
		  json_array j = value<json_array>();
		  _vend = j.raw_end();
		  _end = j.buffer_end();
	  }
	  else{
		  json_value j = value<json_value>();
		  _vend = j.raw_end();
		  _end = j.buffer_end();
	  }
  }
  else
  {
    _end = _last;
    _vend = _vbeg;
  }
};


}}}

#endif
