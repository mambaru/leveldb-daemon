#ifndef FAS_MISC_FAST_JSON_SPECIALIZATION_HPP
#define FAS_MISC_FAST_JSON_SPECIALIZATION_HPP

#include <cstdlib>

// #define USIND_STDLIB

namespace fas{ namespace misc{ namespace fast_json{


//TODO: ������� ������������ � �������������� ��� stdlib

template<>
class serializer< value<int> >
{

public:

  /** serialization:
    * end - back inserter iterator
    */
  template<typename P>
  P operator()( int v, P end)
  {

#ifdef USIND_STDLIB
    char buf[32];
    int s = std::sprintf( buf, "%d", v);
    return std::copy(buf, buf+s, end);
#else
    if ( v < 0)
    {
      *(end++)='-';
      v*=-1;
    }
    char buf[32];
    char *beg = buf;
    char *cur = beg;
    char *end2 = beg + 32;
    int del = 10;
    if (v==0)
      *(cur++) = '0';
    else
    for( ; cur!=end2 && v ; ++cur, v/=10) 
      *cur = '0' + v%del;
    end2 = cur;
    for (;cur-beg>1;--cur, ++beg) 
    { *beg ^= *(cur-1); *(cur-1)^=*beg; *beg^=*(cur-1); }
    return std::copy(buf, end2, end);
#endif

  }

  /** deserialization:
    * beg - forward iterator
    */
  template<typename P>
  P operator() ( int& v, P beg, P end )
  {
    for ( ; beg!=end && *beg<=' '; ++beg);
    std::string temp;
    temp.reserve(32);
    for (; (beg!=end)
      && ( (*beg>='0' && *beg<='9')
            || *beg=='-' || *beg=='+'
         ); ++beg) temp.push_back(*beg);
    if (
      (*beg>='A' && *beg<='F')
      || (*beg>='a' && *beg<='f')
      || *beg=='x' || *beg=='X'
      || *beg=='e' || *beg=='E'
      || *beg=='.'
    )
      throw not_supported();
#ifdef USIND_STDLIB
    v = std::atoi(temp.c_str());
    return beg;
#else
    v = std::atoi(temp.c_str());
    return beg;
#endif
  }
};

template<>
class serializer< value<bool> >
{

public:
  template<typename P>
  P operator()( bool v, P beg)
  {
    if ( v ) { *(beg++)='t'; *(beg++)='r';*(beg++)='u';*(beg++)='e'; }
    else { *(beg++)='f'; *(beg++)='a';*(beg++)='l';*(beg++)='s'; *(beg++)='e'; }
    return beg;
  }

  template<typename P>
  P operator() ( bool& v, P beg, P end )
  {
    static char s_true[]="true";
    static char s_false[]="false";
    for ( ; beg!=end && *beg<=' '; ++beg);
    /*if (*beg!='"') throw invalid_json();
    ++beg;*/
    int i = 0;
    for (;beg!=end && i < 3 ;++beg, ++i)
      if ( *beg!=s_true[i] && *beg!=s_false[i] )
      {
        throw invalid_json();
      }
    /*if ( *beg=='"' )
      v = true;
    else if ( *beg=='e' )*/
    if ( *beg=='e' )
    {
      ++beg;
      v = true;
    }
    else if ( *beg=='s' )
    {
      ++beg;
      if ( *beg!='e' ) throw invalid_json();
      ++beg;
      v = false;
    }
    else
      throw invalid_json();
    /*if ( *beg!='"' )
      throw invalid_json();
    ++beg;*/
    return beg;
  }

};


// TODO: ������ ������ \uNNNN �������������������
template<int N>
class serializer< value< char[N]> >
{
public:

  typedef char value_type[N];
  /** serialization:
   * end - back inserter iterator
   */
  template<typename P>
  P operator()( const value_type& v, P end)
  {
    const char* itr = &(v[0]);
    const char* vend = &(v[N]);
    *(end++)='"';
    for (;itr!=vend && *itr!='\0'; ++itr)
    {
      switch (*itr)
      {
        case '"':
        case '\\':
        case '/':  *(end++)='\\'; *(end++) = *itr; break;
        case '\t': *(end++)='\\'; *(end++) = 't'; break;
        case '\b': *(end++)='\\'; *(end++) = 'b'; break;
        case '\r': *(end++)='\\'; *(end++) = 'r'; break;
        case '\n': *(end++)='\\'; *(end++) = 'n'; break;
        case '\f': *(end++)='\\'; *(end++) = 'f'; break;
        /*case '\u':
          throw not_supported();*/
        default: *(end++) = *itr; break;
      }
    }
    *(end++)='"';
    return end;
    
  };

  /** deserialization:
    * beg, end - forward iterator
    * v.reserve() not call
    */
  template<typename P>
  P operator() ( value_type& v, P beg, P end )
  {
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) throw invalid_json();
    char* itr = &(v[0]);
    if (*beg!='"') throw invalid_json();
    ++beg;
    if (beg==end) throw invalid_json();
    int i = 0;
    for ( ; *beg!='"' && i < N - 1; ++beg, ++i)
    {
      if (beg==end) throw invalid_json();

      if (*beg=='\\' )
      {
        ++beg;
        if (beg==end) throw invalid_json();
        switch ( *beg )
        {
          case '"':
          case '\\':
          case '/': *(itr++) = *beg; break;
          case 't': *(itr++) = '\t'; break;
          case 'b': *(itr++) = '\b'; break;
          case 'n': *(itr++) = '\n'; break;
          case 'r': *(itr++) = '\r'; break;
          case 'f': *(itr++) = '\f'; break;
          case 'u': throw not_supported();
          default: 
            throw invalid_json();
            // *(bii++) = *beg; break;
        }
      }
      else
        *(itr++) = *beg;
    }
    if (*beg!='"') throw invalid_json();
    v[i] = '\0';
    ++beg;
    return beg;
    
  }
};

// TODO: ������ ������ \uNNNN �������������������
template<>
class serializer< value<std::string> >
{
public:

  /** serialization:
   * end - back inserter iterator
   */
  template<typename P>
  P operator()( const std::string& v, P end)
  {
    std::string::const_iterator itr = v.begin();
    *(end++)='"';
    for (;itr!=v.end(); ++itr)
    {
      switch (*itr)
      {
        case '"':
        case '\\':
        case '/':  *(end++)='\\'; *(end++) = *itr; break;
        case '\t': *(end++)='\\'; *(end++) = 't'; break;
        case '\b': *(end++)='\\'; *(end++) = 'b'; break;
        case '\r': *(end++)='\\'; *(end++) = 'r'; break;
        case '\n': *(end++)='\\'; *(end++) = 'n'; break;
        case '\f': *(end++)='\\'; *(end++) = 'f'; break;
        /*case '\u':  
          throw not_supported();*/
        default: *(end++) = *itr; break;
      }
    }
    *(end++)='"';
    return end;
  };

  /** deserialization:
    * beg, end - forward iterator
    * v.reserve() not call
    */
  template<typename P>
  P operator() ( std::string& v, P beg, P end )
  {
    v.clear();
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) throw invalid_json();
    std::back_insert_iterator<std::string> bii = std::back_inserter<std::string>(v);
    if (*beg!='"') throw invalid_json();
    ++beg;
    if (beg==end) throw invalid_json();
    for ( ; *beg!='"'; ++beg)
    {
      if (beg==end) throw invalid_json();

      if (*beg=='\\' )
      {
        ++beg;
        if (beg==end) throw invalid_json();
        switch ( *beg )
        {
          case '"':
          case '\\':
          case '/': *(bii++) = *beg; break;
          case 't': *(bii++) = '\t'; break;
          case 'b': *(bii++) = '\b'; break;
          case 'n': *(bii++) = '\n'; break;
          case 'r': *(bii++) = '\r'; break;
          case 'f': *(bii++) = '\f'; break;
          case 'u': throw not_supported();
          default: 
            throw invalid_json();
            // *(bii++) = *beg; break;
        }
      }
      else
        *(bii++) = *beg;
    }
    if (*beg!='"') throw invalid_json();
    ++beg;
    return beg;
  }
};

/// ////////////////////////////////////////////////////////////

template<typename T, typename L>
class serializer< object<T, L> >
{
  typedef object<T, L> object_type;
public:

  /** serialization:
   * end - back inserter iterator
   */
  template<typename P>
  P operator()( const T& t, P end)
  {
    *(end++)='{';
    end = serialize_members(t, end, L() );
    *(end++)='}';
    return end;
  }

  /** deserialization:
    * beg, end - forward iterator
    */
  template<typename P>
  P operator()( T& t, P beg, P end)
  {
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) throw invalid_json();
    if (*beg!='{') throw invalid_json();
    ++beg;
    for (;;)
    {
      beg = deserialize_members(t, beg, end, L() );
      for ( ; beg!=end && *beg<=' '; ++beg);
      if (beg==end) throw invalid_json();
      if (*beg=='}')  break;
      if (*beg!=',')  throw invalid_json();
      beg++;
      if (beg==end) throw invalid_json();
    }
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) throw invalid_json();
    if (*beg!='}') throw invalid_json();
    ++beg;
    return beg;
  }

private:

  template<typename P, typename C, typename R>
  P serialize_members( const T& t, P end, ap::type_list<C, R> )
  {
    end = serialize_member(t, end, C());
    *(end++)=',';
    return serialize_members(t, end, R() );
  }

  template<typename P, typename C>
  P serialize_members( const T& t, P end, ap::type_list<C, ap::empty_type> )
  {
    return serialize_member(t, end, C());
  }
  
  template<typename P, typename N, typename G, typename M, M G::* m, typename W >
  P serialize_member( const T& t, P end, const member<N, G, M, m, W>& memb )
  {
    const char* name = memb();
    *(end++)='"';
    for (;*name!='\0'; ++name) *(end++) = *name;
    *(end++)='"';
    *(end++) = ':';

    typedef typename member<N, G, M, m, W>::serializer_type serializer_type;

    return serializer_type()( memb.ref(t), end );
  }

private:

  // � � ��������������� ��������
  template<typename P, typename C, typename R>
  P deserialize_members( T& t, P beg, P end, ap::type_list<C, R> )
  {
    
    for ( ; beg!=end && *beg<=' '; ++beg);

    /// �������� ��������������� ������� ������
    beg = deserialize_member( t, beg, end, C() );
    
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) throw invalid_json();
    if (*beg=='}') return beg;
    if (*beg!=',') throw invalid_json();
    ++beg;
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) throw invalid_json();

    /// �������� ��������������� ���������� �������
    beg = deserialize_members( t, beg, end, R() );

    if (beg==end) throw invalid_json();
    
    return beg;
  }

  // ����� �� �����
  template<typename P>
  P deserialize_members( T& t, P beg, P end, ap::empty_type )
  {
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) throw invalid_json();
    return beg;
  }

  template<typename P, typename N, typename G, typename M, M G::* m, typename W >
  P deserialize_member( T& t, P beg, P end, member<N, G, M, m, W> memb )
  {
    
    P start = beg;
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) throw invalid_json();
    if (*beg!='"') throw invalid_json();
    beg++;

    const char* name = memb();
    
    for ( ; beg!=end && *name!='\0' && *beg==*name; ++name, ++beg) 
      if (*name!=*beg)
        return start;
    
    if (*beg!='"' || *name!='\0')
      return start;
    
    if (beg==end) throw invalid_json();
    if (*beg!='"') throw invalid_json();
    beg++;
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) throw invalid_json();
    if (*beg!=':') throw invalid_json();
    beg++;
    if (beg==end) throw invalid_json();
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) throw invalid_json();
    typedef typename member<N, G, M, m, W>::serializer_type serializer_type;
    
    return serializer_type()( memb.ref(t), beg, end);
  }
};


/// ////////////////////////////////////////////////////////////////

template<typename T, template<typename, typename> class C, template<typename> class A>
class serializer< array< T, C, A> >
{
  typedef typename T::serializer_type serializer_type;
  typedef typename T::target target;


public:
  template<typename P>
  P operator()( C<target, A<target> >& t,  P beg, P end)
  {
    t.clear();
    std::back_insert_iterator<C<target, A<target> > > bitr =  std::back_inserter(t);
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) throw invalid_json();
    if (*beg!='[') throw invalid_json();
    ++beg;
    for (;;)
    {
      for ( ; beg!=end && *beg<=' '; ++beg);
      if (*beg==']') break;
      if (beg==end) throw invalid_json();
      target tg;
      beg = serializer_type()( tg, beg, end);
      *(bitr++) = tg;

      if (beg==end) throw invalid_json();
      for ( ; beg!=end && *beg<=' '; ++beg);
      if (beg==end) throw invalid_json();
      if (*beg==']') break;
      if (*beg!=',') throw invalid_json();
       ++beg;
      if (beg==end) throw invalid_json();
    }
    ++beg;
    return beg;
  }


  template<typename P>
  P operator()( const C<target, A<target> >& t, P end)
  {
    *(end++)='[';
    typename C<target, A<target> >::const_iterator itr = t.begin();
    for (;itr!=t.end();)
    {
      end = serializer_type()(*itr, end);
      ++itr;
      if (itr!=t.end()) *(end++)=',';
    }
    *(end++)=']';
    return end;
  }
};


/// /////////////////////////////////////////////////////////////////

template<typename T, typename L>
class serializer< enumerator<T, L> >
{
  typedef typename enumerator<T, L>::enum_list enum_list;
public:
  template<typename P>
  P operator()( const T& v, P end)
  {
    *(end++)='"';
    end = this->serialize(v, enum_list(), end);
    *(end++)='"';
    return end;
  };

  template<typename LL, typename RR, typename P>
  P serialize( const T& v, ap::type_list<LL, RR>, P end)
  {
    if (LL::value == v)
    {
      const char* val = LL()();
      for ( ; *val!='\0' ; ++val)
        *(end++) = *val;
      return end;
    }
    else
    {
      return this->serialize(v, RR(), end);
    }
  }

  template<typename P>
  P serialize( const T& v, ap::empty_type, P end)
  {
    return end;
  }

  template<typename P>
  P operator() ( T& v, P beg, P end )
  {
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) throw invalid_json();
    if (*beg!='"') throw invalid_json();
    ++beg;
    if (beg==end) throw invalid_json();
    P first = beg;
    for ( ; beg!=end && *beg!='"'; ++beg);
    if (beg==end) throw invalid_json();
    if (*beg!='"') throw invalid_json();
    this->deserialize(v, enum_list(), first, beg);
    ++beg;
    return beg;
  }

  template<typename LL, typename RR, typename P>
  void deserialize( T& v, ap::type_list<LL, RR>, P beg, P end)
  {
    P first = beg;
    const char *pstr = LL()();
    for ( ; beg!=end && *pstr!='\0' && *pstr==*beg; ++beg, ++pstr);
    if ( beg==end && *pstr=='\0')
    {
      v = LL::value;
    }
    else
      deserialize(v, RR(), first, end);
  }

  template<typename P>
  void deserialize( T& , ap::empty_type,P,P)
  {
  }

};

}}}

#endif
