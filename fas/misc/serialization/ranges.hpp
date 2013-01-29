#ifndef FAS_MISC_SEREALIZETION_RANGES_HPP
#define FAS_MISC_SEREALIZETION_RANGES_HPP

#include <iterator>
#include <cassert>
namespace fas{ namespace serialization{

template<typename T>
class trivial_range
{
public:
   typedef typename std::iterator_traits<T>::iterator_category iterator_category;
   typedef typename std::iterator_traits<T>::value_type        value_type;
  typedef typename std::iterator_traits<T>::difference_type   difference_type;
  typedef typename std::iterator_traits<T>::pointer           pointer;
  typedef typename std::iterator_traits<T>::reference         reference;

  explicit trivial_range(T b)
    : b(b)
  {};

  const T begin() const { return b; }

  operator bool () const { return true; }

  bool operator == (const trivial_range<T>& r ) const { return b == r.b;  }

  bool operator != (const trivial_range<T>& r ) const { return !(*this == r); }

  const reference operator*() const { return *b; }

  reference operator*() { return *b; }

  pointer operator ->() { return &(*b);}

  const pointer operator ->() const  { return &(*b);}

protected:
  T b;
};

template<typename T>
class output_range
{

public:

   typedef typename std::iterator_traits<T>::iterator_category iterator_category;
   typedef typename std::iterator_traits<T>::value_type        value_type;
  typedef typename std::iterator_traits<T>::difference_type   difference_type;
  typedef typename std::iterator_traits<T>::pointer           pointer;
  typedef typename std::iterator_traits<T>::reference         reference;


  explicit output_range(T b)
    : b(b)
  {};

  const T begin() const { return b; }

  operator bool () const { return true; }

  const reference operator*() const { return *b; }

  reference operator*() { return *b; }

  pointer operator ->() { return &(*b);}

  const pointer operator ->() const  { return &(*b);}

  bool operator == (const output_range<T>& r ) const { return b == r.b;  }

  bool operator != (const output_range<T>& r ) const { return !(*this == r); }

  output_range<T>& operator++() 
  {
    ++b; 
    return *this; 
  }

  output_range<T> operator++(int) 
  {
    output_range<T> ans = *this;   
    b++; 
    return ans; 
  }

protected:
  T b;

};

template<typename T>
class input_range
{
public:
   typedef typename std::iterator_traits<T>::iterator_category iterator_category;
   typedef typename std::iterator_traits<T>::value_type        value_type;
  typedef typename std::iterator_traits<T>::difference_type   difference_type;
  typedef typename std::iterator_traits<T>::pointer           pointer;
  typedef typename std::iterator_traits<T>::reference         reference;

  explicit input_range(T b, T e)
    : b(b), e(e)
  {};

  operator bool () const { return b!=e; }

  const reference operator*() const { return *b; }

  reference operator*() { return *b; }

  pointer operator ->() { return &(*b);}

  const pointer operator ->() const  { return &(*b);}

  const T begin() const { return b; }
  
  const T end() const { return e; }

  input_range<T>& operator++() 
  {
    assert(b!=e);
    ++b; 
    return *this; 
  }

  input_range<T> operator++(int) 
  {
    assert(b!=e);
    input_range<T> ans = *this;   
    b++; 
    return ans; 
  }

  difference_type distance() const { return std::distance(b, e); }
  
  input_range<T>& advance(difference_type s)  { std::advance(b, s); return *this;}
  
  bool operator == (const input_range<T>& r ) const { return b == r.b && e==r.e;  }

  bool operator != (const input_range<T>& r ) const { return !(*this == r); }

protected:
  T b;
  T e;
};

template<typename T>
class forward_range
{
public:
   typedef typename std::iterator_traits<T>::iterator_category iterator_category;
   typedef typename std::iterator_traits<T>::value_type        value_type;
  typedef typename std::iterator_traits<T>::difference_type   difference_type;
  typedef typename std::iterator_traits<T>::pointer           pointer;
  typedef typename std::iterator_traits<T>::reference         reference;

  explicit forward_range(T b, T e)
    : b(b), e(e)
  {};

  operator bool () const { return b!=e; }

  const reference operator*() const { return *b; }

  reference operator*() { return *b; }

  pointer operator ->() { return &(*b);}

  const pointer operator ->() const  { return &(*b);}

  const T begin() const { return b; }
  
  const T end() const { return e; }

  difference_type distance() const { return std::distance(b, e); }
  
  forward_range<T>& advance(difference_type s)  { std::advance(b, s); return *this;}

  forward_range<T>& operator++() 
  {
    assert(b!=e);
    ++b; 
    return *this; 
  }

  forward_range<T> operator++(int) 
  {
    assert(b!=e);
    forward_range<T> ans = *this;   
    b++; 
    return ans; 
  }

  bool operator == (const forward_range<T>& r ) const { return b == r.b && e==r.e;  }

  bool operator != (const forward_range<T>& r ) const { return !(*this == r); }

protected:
  T b;
  T e;

};

template<typename T>
class bidirectional_range
{
public:
   typedef typename std::iterator_traits<T>::iterator_category iterator_category;
   typedef typename std::iterator_traits<T>::value_type        value_type;
  typedef typename std::iterator_traits<T>::difference_type   difference_type;
  typedef typename std::iterator_traits<T>::pointer           pointer;
  typedef typename std::iterator_traits<T>::reference         reference;

  explicit bidirectional_range(T b, T e)
    : b(b), e(e), s(b)
  {};

  operator bool () const { return b!=e; }

  const reference operator*() const { return *b; }

  reference operator*() { return *b; }

  pointer operator ->() { return &(*b);}

  const pointer operator ->() const  { return &(*b);}

  const T begin() const { return b; }
  
  const T end() const { return e; }

  difference_type distance() const { return std::distance(b, e); }
  
  bidirectional_range<T>& advance(difference_type s)  { std::advance(b, s); return *this; }

  bidirectional_range<T>& operator++() 
  {
    assert(b!=e);
    ++b; 
    return *this; 
  }

  bidirectional_range<T> operator++(int) 
  {
    assert(b!=e);
    bidirectional_range<T> ans = *this;   
    b++; 
    return ans; 
  }

  bidirectional_range<T>& operator--() 
  {
    assert(b!=s);
    --b; 
    return *this; 
  }

  bidirectional_range<T> operator--(int) 
  {
    assert(b!=s);
    bidirectional_range<T> ans = *this;   
    b--; 
    return ans; 
  }


  bool operator == (const bidirectional_range<T>& r ) const { return b == r.b && e==r.e;  }

  bool operator != (const bidirectional_range<T>& r ) const { return !(*this == r); }

protected:
  T b;
  T e;
  T s; // start
};


template<typename T>
class random_access_range
{
public:
   typedef typename std::iterator_traits<T>::iterator_category iterator_category;
   typedef typename std::iterator_traits<T>::value_type        value_type;
  typedef typename std::iterator_traits<T>::difference_type   difference_type;
  typedef typename std::iterator_traits<T>::pointer           pointer;
  typedef typename std::iterator_traits<T>::reference         reference;

  explicit random_access_range(T b, T e)
    : b(b), e(e), s(b)
  {};

  operator bool () const { return b!=e; }

  const reference operator*() const { return *b; }

  reference operator*() { return *b; }

  pointer operator ->() { return &(*b);}

  const pointer operator ->() const  { return &(*b);}

  const T begin() const { return b; }
  
  const T end() const { return e; }

  difference_type distance() const { return std::distance(b, e); }
  
  random_access_range<T>& advance(difference_type c)  { std::advance(b, c); return *this; }

  random_access_range<T>& operator++() 
  {
    assert(b!=e);
    ++b; 
    return *this; 
  }

  random_access_range<T> operator++(int) 
  {
    assert(b!=e);
    random_access_range<T> ans = *this;   
    b++; 
    return ans; 
  }

  random_access_range<T>& operator--() 
  {
    assert(b!=s);
    --b; 
    return *this; 
  }

  random_access_range<T> operator--(int) 
  {
    assert(b!=s);
    random_access_range<T> ans = *this;   
    b--; 
    return ans; 
  }

  bool operator == (const random_access_range<T>& r ) const { return b == r.b && e==r.e;  }

  bool operator != (const random_access_range<T>& r ) const { return !(*this == r); }

// TODO: random access range operation
protected:
  T b;
  T e;
  T s; // start
};

template<typename T>
class string_range
{
public:
  typedef typename std::iterator_traits<T>::iterator_category iterator_category;
  typedef typename std::iterator_traits<T>::value_type        value_type;
  typedef typename std::iterator_traits<T>::difference_type   difference_type;
  typedef typename std::iterator_traits<T>::pointer           pointer;
  typedef typename std::iterator_traits<T>::reference         reference;

  explicit string_range(T b)
    : b(b), e(0), s(b)
  {};

  operator bool () const { return *b!='\0'; }

  const reference operator*() const { return *b; }

  reference operator*() { return *b; }

  pointer operator ->() { return &(*b);}

  const pointer operator ->() const  { return &(*b);}

  const T begin() const { return b; }
  
  const T end() const { return _end(); }

  difference_type distance() const { return std::distance(b, _end() ); }
  
  string_range<T>& advance(difference_type c)  { std::advance(b, c); return *this; }

  string_range<T>& operator++() 
  {
    assert(*b!='\0');
    ++b; 
    return *this; 
  }

  string_range<T> operator++(int) 
  {
    assert(*b!='\0');
    string_range<T> ans = *this;   
    b++; 
    return ans; 
  }

  string_range<T>& operator--() 
  {
    assert(b!=s);
    --b; 
    return *this; 
  }

  string_range<T> operator--(int) 
  {
    assert(b!=s);
    string_range<T> ans = *this;   
    b--; 
    return ans; 
  }

  bool operator == (const string_range<T>& r ) const { return b == r.b;  }

  bool operator != (const string_range<T>& r ) const { return !(*this == r); }

// TODO: random access range operation
protected:

  T _end() const
  {
    if ( !e) { e = b; for ( ;*e!='\0';++e); }
    return e; 
  }

  T b;
  mutable T e;
  T s; // start
};

/*
template<typename T>
struct forward_range
{
public:

   typedef typename std::iterator_traits<T>::iterator_category iterator_category;
   typedef typename std::iterator_traits<T>::value_type        value_type;
  typedef typename std::iterator_traits<T>::difference_type   difference_type;
  typedef typename std::iterator_traits<T>::pointer           pointer;
  typedef typename std::iterator_traits<T>::reference         reference;

  forward_range()
    : b()
    , e() 
  {};
  
  explicit forward_range(T beg)
    : b(beg)
    , e() 
  {};

  forward_range(T beg, T end)
    : b(beg), e(end) 
  {};

  const T begin() const { return b; }
  const T end() const { return e; }

  difference_type distance() const 
  {
    return std::distance(b, e);
  }
  
  void advance(difference_type s)
  {
    std::advance(b, s);
  }

  bool operator == (const forward_range<T>& r ) const
  {
    return b == r.b && e == r.e;
  }

  bool operator != (const forward_range<T>& r ) const
  {
    return !(*this == r);
  }

  operator bool () const 
  {
    return b!=e; 
  };

  forward_range<T>& operator++() 
  {
    assert(*this);
    ++b; 
    return *this; 
  }

  forward_range<T> operator++(int) 
  {
    assert(*this);
    forward_range<T> ans = *this; 
    b++; 
    return ans; 
  }

  
  const value_type& operator*() const 
  {
    assert(*this);
    return *b;
  }

private:
  T b;
  T e;
};
*/
namespace detail
{
  template<typename T, typename ItrTag>
  struct range_traits_helper;

  template<typename T>
  struct range_traits_helper<T, std::input_iterator_tag>
  {
    typedef input_range<T> range_type;
  };

  template<typename T>
  struct range_traits_helper<T, std::output_iterator_tag>
  {
    // #warinig DODO
    typedef output_range<T> range_type;
  };

  template<typename T>
  struct range_traits_helper<T, std::forward_iterator_tag>
  {
    // #warinig DODO
    typedef forward_range<T> range_type;
  };

  template<typename T>
  struct range_traits_helper<T, std::bidirectional_iterator_tag>
  {
    // #warinig DODO
    typedef bidirectional_range<T> range_type;
  };

  template<typename T>
  struct range_traits_helper<T, std::random_access_iterator_tag>
  {
    // #warinig DODO
    typedef random_access_range<T> range_type;
  };
};

template<typename T>
struct range_traits
{
  typedef typename detail::range_traits_helper<
    T, 
    typename std::iterator_traits<T>::iterator_category 
  >::range_type type;

};


template<typename T>
typename range_traits<T>::type make_range(T beg, T end)
{
  typedef typename range_traits<T>::type range_type;
  return range_type(beg, end);
};

template<typename T>
string_range<T> make_range(T beg)
{
  typedef string_range<T> range_type;
  return range_type(beg);
};


}}

#endif
