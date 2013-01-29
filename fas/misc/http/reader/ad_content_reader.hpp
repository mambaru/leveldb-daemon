#ifndef FAS_MISC_HTTP_READER_AD_CONTENT_READER_HPP
#define FAS_MISC_HTTP_READER_AD_CONTENT_READER_HPP


namespace fas{ namespace http{ namespace reader{

class ad_content_reader
{
public:
  template<typename T>
  const char* operator()(T& t, const char* beg, const char* end)
  {
    int content_length = t.get_content_length();
    if ( content_length < 0)
    {
      t._content_ready(beg, end, true);
      return end;
    }
    else if ( content_length == 0) 
    {
      t._content_ready(beg, beg, true);
      return beg;
    }
    else
    {
      if ( end - beg < content_length ) 
      {
        t._content_no_ready();
        return 0;
      }
      else
      {
        t._content_ready( beg, beg + content_length, true);
        return beg + content_length;
      }
    }
  }
};

}}}

#endif
