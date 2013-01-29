#ifndef FAS_MISC_HTTP_READER_HTTP_PARSER_HPP
#define FAS_MISC_HTTP_READER_HTTP_PARSER_HPP

namespace fas{ namespace http{ namespace reader{

/** Паресер не выкидывает исключения, а возвращает 0 в случае неуспеха
  * В случае успеха, возвращает указатель на конец сущности
  */
class http_parser
{
public:
  inline static const char* parse_space( const char* beg, const char* end )
  {
    for (;beg!=end; ++beg)
      if (*beg!=' ') return beg;
    return beg;
  }

  inline static const char* parse_header_line( const char* beg, const char* end )
  {
    for (;beg!=end;)
    {
      if (*beg == '\r')
      {
        if ( ++beg == end) return 0;

        if (*beg == '\n')
          return ++beg;
      }
      else
        ++beg;
    }
    return 0;
  }

  inline static const char* parse_header( const char* beg, const char* end )
  {
    return parse_header_line(beg, end);
  }

  inline static const char* parse_initial( const char* beg, const char* end )
  {
    return parse_header_line(beg, end);
  }

  inline static const char* parse_end_head( const char* beg, const char* end )
  {
    // if (beg==end) return 0;
    if ( beg==end || *beg!='\r') return 0;
    ++beg;
    if ( beg==end || *beg!='\n') return 0;
    ++beg;
    return beg;
  }


  inline static const char* parse_value( const char* name, const char* beg, const char* end )
  {
    while( *name!='\0' && beg!=end )
    {
      if (*beg!=*name)
      {
        if ( (*name >= 'A' && *name <= 'Z') )
        {
          if (*name != *beg && *name + ('a' - 'A') != *beg  )
            return 0;
        }
        else if (*name >= 'a' && *name <= 'z') 
        {
          if (*name != *beg && *name!= *beg + ('a' - 'A') )
            return 0;
        }
        else
         return 0;
      }
      ++name;
      ++beg;
    }

    /*if ( beg == end ) 
      return 0;*/

    if ( *name != '\0' /*&& beg!=end*/ )
      return 0;

    return beg;

    /*// Допускаем наличие пробелов между именем и ;
    if ( *beg == ' ' || *beg == ':')
      return beg;

    return 0;*/
  }

  inline static const char* check_value( const char* name, const char* beg, const char* end )
  {
    beg = parse_value(name, beg, end);
    if (beg == 0 ) return 0;
    if ( beg == end ) return  beg;

    if ( *beg == ' ' || *beg == '\r')
      return beg;

    return 0;
  }

  inline static const char* check_name( const char* name, const char* beg, const char* end )
  {
    beg = parse_value(name, beg, end);
    if (beg == 0 ) return 0;

    if ( beg == end ) 
      return 0;

    // Допускаем наличие пробелов между именем и ;
    if ( *beg == ' ' || *beg == ':')
      return beg;

    return 0;
  }
};


}}}

#endif
