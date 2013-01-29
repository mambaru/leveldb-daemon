#ifndef FAS_MISC_COMMAND_WAIT_LIST_HPP
#define FAS_MISC_COMMAND_WAIT_LIST_HPP

#include <fas/misc/command/tags.hpp>
#include <fas/aop/aspect.hpp>
#include <fas/pattern/type_list.hpp>
#include <set>

namespace fas{ namespace command{

namespace aa = ::fas::aop;
namespace ap = ::fas::pattern;

struct wait_list_constants
{
  enum 
  {
    OrderedByMaxSize = 1024*1024*1024
  };
};

template<typename C, typename V, V C::*v>
struct less_command
{
  bool operator () ( const C& cmd1, const C& cmd2 ) const
  {
    return cmd1.*v < cmd2.*v;
  }
};

template<typename T>
struct command_wrapper: T
{
  time_t created;
  command_wrapper(const T& cmd): T(cmd), created(time(0)) {}
};

struct PRC_true
{
  template<typename T>
  bool operator()(const T& ) { return true;}
};


template<typename T, typename CB, typename V, V CB::*v, size_t MAXS>
class ordered_by_t
  : public std::multiset< command_wrapper<T>, less_command<CB, V, v> >
{
  typedef std::multiset< command_wrapper<T>, less_command<CB, V, v> > super;
public:
  typedef command_wrapper<T> wrapper;

  void insert(const T& cmd) 
  {
    while ( super::size() > MAXS )
      super::erase( super::begin() );
    
    super::insert( wrapper(cmd) );
  }

  template<typename C>
  void extract(const T& cmd, C& result_container )
  {
    std::pair< typename super::iterator, typename super::iterator > range = equal_range( cmd );
    typename super::iterator itr = range.first;
    for ( ;itr!=range.second; ++itr )
      result_container.push_back(*itr);
    erase(range.first, range.second);
  }

  template<typename C, typename CMP>
  void extract(const T& cmd, C& result_container, CMP cmp )
  {
    std::pair< typename super::iterator, typename super::iterator > range = equal_range( cmd );
    // typename super::iterator itr = range.first;
    while( range.first!=range.second)
    {
      if ( cmp(*range.first) )
      {
        result_container.push_back(*range.first);
        erase(range.first++);
      }
      else
        ++range.first;
    }
  }

  template<typename PRC>
  void process(T& cmd, PRC prc )
  {
    std::pair< typename super::iterator, typename super::iterator > range = equal_range( cmd );
    // typename super::iterator itr = range.first;
    while( range.first!=range.second)
    {
      // Ахтунг!! нельзя менять идекс в prc
      if ( prc( static_cast<const T&>(*range.first)) )
        erase(range.first++);
      else
        ++range.first;
    }
  }

  template<typename PRC>
  void process(PRC prc )
  {
    std::pair< typename super::iterator, typename super::iterator > range( this->begin(), this->end() );
    // typename super::iterator itr = range.first;
    while( range.first!=range.second)
    {
      // Ахтунг!! нельзя менять идекс в prc
      if ( prc( static_cast<const T&>(*range.first)) )
        erase(range.first++);
      else
        ++range.first;
    }
  }

  template<typename PRC>
  size_t process(time_t livetime, PRC prc )
  {
    size_t count = 0;
    time_t now = time(0);
    std::pair< typename super::iterator, typename super::iterator > range( this->begin(), this->end() );
    while( range.first!=range.second)
    {
      // Ахтунг!! нельзя менять идекс в prc
      if ( ( now - range.first->created >= livetime) &&  prc( static_cast<const T&>(*range.first)) )
      {
        ++count;
        erase(range.first++); 
      }
      else
        ++range.first;
    }
    return count;
  }


  bool probe( const T& cmd ) const { return super::find(cmd)!=super::end(); }

  template<typename CMP>
  bool probe( const T& cmd, CMP cmp ) const 
  {
     typename super::const_iterator itr = super::find(cmd);
     if ( itr == super::end()) return false;
     return cmp(*itr);
  }
};

template<typename Tg, typename CB, typename V, V CB::*v, size_t MAX_WL_SIZE = -1>
struct ordered_by
{
  template<typename T>
  struct make
  {
    typedef aa::advice< typename aa::tag_list_n< Tg, aa::gtag<_ordered_by_> >::type, ordered_by_t<T, CB, V, v, MAX_WL_SIZE > > type;
  };

};

template<typename T, typename L>
struct wait_list_bulder;

template<typename T, typename L, typename R>
struct wait_list_bulder< T, ap::type_list<L, R> >
{
  typedef ap::type_list< typename L::template make< T >::type, typename wait_list_bulder<T, R>::type > type;
};

template<typename T>
struct wait_list_bulder< T, ap::empty_type >
{
  typedef ap::empty_type type;
};

template<typename T, typename L>
struct wait_list_aspect_class_bulder
{
  typedef aa::aspect_class< 
    aa::aspect<
      typename wait_list_bulder<
        T,
        typename ap::type_list_traits<L>::type 
      >::type 
    >
  > type;
};

template<typename T, typename L>
class wait_list
  : public wait_list_aspect_class_bulder<T, L>::type
{
  typedef typename wait_list_aspect_class_bulder<T, L>::type super;
  typedef typename super::aspect aspect;
public:
  typedef T command_type;

  template<typename Tg>
  void insert(T& cmd)
  {
    this->get_aspect().template get<Tg>().insert(cmd);
  }

  template<typename C>
  void extract(const T& cmd, C& result_container  )
  {
    typedef typename super::template select<_ordered_by_>::type order_list;
    _extract(cmd, ap::type2type<order_list>(), result_container );
  }

  template<typename C, typename CMP>
  void extract(const T& cmd, C& result_container, CMP cmp )
  {
    typedef typename super::template select<_ordered_by_>::type order_list;
    _extract(cmd, ap::type2type<order_list>(), result_container, cmp );
  }

  void clear()
  {
    typedef typename super::template select<_ordered_by_>::type order_list;
    _clear(order_list());
  }

  template<typename LL, typename RR>
  void _clear(ap::type_list<LL, RR>)
  {
    this->get_aspect().template get< LL >().clear();
    _clear(RR());
  }

  void _clear(ap::empty_type) {};

  size_t size() const
  {
    typedef typename super::template select<_ordered_by_>::type order_list;
    return _size(order_list());
  }

  template<typename LL, typename RR>
  size_t _size(ap::type_list<LL, RR>) const
  {
    return this->get_aspect().template get< LL >().size()
           + _size(RR());
  }

  size_t _size(ap::empty_type) const { return 0; };

  template<typename PRC>
  void process(T& cmd, PRC prc )
  {
    typedef typename super::template select<_ordered_by_>::type order_list;
    _process(cmd, /*ap::type2type<*/order_list()/*>()*/, prc );
  }

  template<typename PRC>
  size_t process(time_t timelife, PRC prc = PRC_true() )
  {
    typedef typename super::template select<_ordered_by_>::type order_list;
    return _process(timelife , /*ap::type2type<*/order_list()/*>()*/, prc );
  }

  template<typename TT, typename C>
  void _extract(const T& cmd, ap::type2type<TT>, C& result_container  )
  {
    this->get_aspect().template get< typename TT::left_type>().extract(cmd, result_container );
    _extract(cmd, ap::type2type<typename TT::right_type>(), result_container );
  }

  template<typename TT, typename C, typename CMP>
  void _extract(const T& cmd, ap::type2type<TT>, C& result_container, CMP cmp  )
  {
    this->get_aspect().template get< typename TT::left_type>().extract(cmd, result_container, cmp );
    _extract(cmd, ap::type2type<typename TT::right_type>(), result_container, cmp );
  }

  template<typename LL, typename RR, typename PRC>
  void _process(T& cmd, ap::type_list<LL, RR>, PRC prc  )
  {
    this->get_aspect().template get< LL >().process(cmd, prc );
    _process(cmd, RR(), prc );
  }

  template<typename C, typename PRC>
  void _process(T&, ap::empty_type, PRC ) { }

  template<typename LL, typename RR, typename PRC>
  size_t _process(time_t lifetime, ap::type_list<LL, RR>, PRC prc  )
  {
    return this->get_aspect().template get< LL >().process(lifetime, prc )
           + _process(lifetime, RR(), prc );
  }

  template<typename PRC>
  size_t _process(time_t, ap::empty_type, PRC ) { return 0;}

  /*
  template<typename TT, typename PRC>
  void _process(T& cmd, ap::type2type<TT>, PRC prc  )
  {
    this->get_aspect().template get< typename TT::left_type>().extract(cmd, prc );
    _extract(cmd, ap::type2type<typename TT::right_type>(), prc );
  }

  template<typename C, typename PRC>
  void _process(T&, ap::type2type<ap::empty_type>, PRC ) { }
  */


  template<typename C>
  void _extract(const T&, ap::type2type<ap::empty_type>, C& ) { }

  template<typename C, typename CMP>
  void _extract(const T&, ap::type2type<ap::empty_type>, C&, CMP ) { }



  bool probe(const T& cmd) const
  {
    typedef typename super::template select<_ordered_by_>::type order_list;
    return _probe(cmd, ap::type2type<order_list>() );
  }

  template<typename CMP>
  bool probe(const T& cmd, CMP cmp) const
  {
    typedef typename super::template select<_ordered_by_>::type order_list;
    return _probe(cmd, ap::type2type<order_list>(), cmp );
  }

  template<typename TT>
  bool _probe(const T& cmd, ap::type2type<TT>) const
  {
    return
      this->get_aspect().template get< typename TT::left_type>().probe(cmd )
      || _probe(cmd, ap::type2type<typename TT::right_type>());
  }

  bool _probe(const T&, ap::type2type<ap::empty_type>) const { return false; }

  template<typename TT, typename CMP>
  bool _probe(const T& cmd, ap::type2type<TT>, CMP cmp  ) const
  {
    return
      this->get_aspect().template get< typename TT::left_type>().probe(cmd, cmp )
      || _probe(cmd, ap::type2type<typename TT::right_type>(), cmp );
  }

  template<typename CMP>
  bool _probe(const T&, ap::type2type<ap::empty_type>, CMP) const { return false; }

  template<typename Tg>
  struct index
  {
    typedef typename aa::advice_cast<Tg, aspect>::type type;
  };

  template<typename Tg>
  typename aa::advice_cast<Tg, aspect>::type& get()
  {
    return this->get_aspect().template get<Tg>();
  }

  template<typename Tg>
  const typename aa::advice_cast<Tg, aspect>::type& get() const
  {
    return this->get_aspect().template get<Tg>();
  }

  
};

/// //////////////////

template<typename L>
struct multi_wait_list_bulder;

template<typename T>
struct wait_holder
{
  T wait_list;
};

struct _holder_{};
template<typename L, typename R>
struct multi_wait_list_bulder< ap::type_list<L, R> >
{
  typedef ap::type_list< 
    aa::advice< 
      typename aa::tag_list_n<
        ap::type2type<typename L::command_type>,
        aa::gtag<_holder_>
      >::type,
      wait_holder<L>
    >,
    typename multi_wait_list_bulder<R>::type
  > type;
};

template<>
struct multi_wait_list_bulder<ap::empty_type >
{
  typedef ap::empty_type type;
};

template<typename L>
struct multi_wait_list_aspect_class_bulder
{
  typedef aa::aspect_class< 
    aa::aspect<
      typename multi_wait_list_bulder<
        typename ap::type_list_traits<L>::type 
      >::type 
    >
  > type;
};


template<typename L>
class multi_wait_list
  : public multi_wait_list_aspect_class_bulder<L>::type
{
public:
  typedef typename multi_wait_list_aspect_class_bulder<L>::type super;
  typedef typename super::aspect aspect;

  template<typename Tg>
  typename aa::advice_cast< ap::type2type<Tg>, aspect >::type&
  get() { return this->get_aspect().template get< ap::type2type<Tg> >(); }

  template<typename Tg>
  const typename aa::advice_cast< ap::type2type<Tg>, aspect >::type&
  get() const { return this->get_aspect().template get< ap::type2type<Tg> >(); }

  template<typename Tg, typename T>
  void insert(const T& cmd)
  {
    this->get_aspect().template get<ap::type2type<T> >().wait_list.get_aspect().template get<Tg>().insert(cmd);
  }

  template<typename T, typename C>
  void extract(const T& cmd, C& result_container  )
  {
    this->get_aspect().template get<ap::type2type<T> >().wait_list.extract(cmd, result_container);
  }

  template<typename T, typename C, typename CMP>
  void extract(const T& cmd, C& result_container, CMP cmp )
  {
    this->get_aspect().template get<ap::type2type<T> >().wait_list.extract(cmd, result_container, cmp);
  }

  template<typename T, typename PRC>
  void process(T& cmd, PRC prc )
  {
    this->get_aspect().template get<ap::type2type<T> >().wait_list.extract(cmd, prc);
  }

  template<typename PRC>
  size_t process(time_t timelife , PRC prc = PRC_true() )
  {
    typedef typename aspect::template select<_holder_>::type wait_list_type;
    return _process(wait_list_type(), timelife, prc );
  }

  size_t process(time_t timelife  )
  {
    typedef typename aspect::template select<_holder_>::type wait_list_type;
    return _process(wait_list_type(), timelife, PRC_true() );
  }

  template<typename LL, typename RR, typename PRC>
  size_t _process(ap::type_list<LL, RR>, time_t timelife , PRC prc )
  {
    return this->get_aspect().template get<LL>().wait_list.process(timelife, prc);
           + _process(RR(), timelife, prc);
  }

  template<typename PRC>
  size_t _process(ap::empty_type, time_t, PRC ) { return 0; };

  void clear()
  {
    typedef typename aspect::template select<_holder_>::type wait_list_type;
    return _clear(wait_list_type());
  }

  template<typename LL, typename RR>
  void _clear(ap::type_list<LL, RR>)
  {
    this->get_aspect().template get<LL>().wait_list.clear();
    _clear( RR());
  }

  void _clear(ap::empty_type) {};

  size_t size() const
  {
    typedef typename aspect::template select<_holder_>::type wait_list_type;
    return _size(wait_list_type());
  }

  template<typename LL, typename RR>
  size_t _size(ap::type_list<LL, RR>) const
  {
    return this->get_aspect().template get<LL>().wait_list.size()
           + _size( RR());
  }

  size_t _size(ap::empty_type) const { return 0; };

  template<typename T>
  bool probe(const T& cmd)
  {
    return this->get_aspect().template get<ap::type2type<T> >().wait_list.probe(cmd);
  }

  template<typename T, typename CMP>
  bool probe(const T& cmd, CMP cmp)
  {
    return this->get_aspect().template get<ap::type2type<T> >().wait_list.probe(cmd, cmp);
  }
};

}}

#endif

