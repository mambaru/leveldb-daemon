#ifndef FAS_MISC_COMMAND_COMMAND_BINDER_HPP
#define FAS_MISC_COMMAND_COMMAND_BINDER_HPP

#include <stdexcept>
#include <iostream>

#include <fas/pattern/type_list.hpp>

namespace fas{ namespace command{

namespace ap = ::fas::pattern;

/** Базовый класс для биндеров конкретных комманд
  */
template<bool B>
class command_binder_base
{
public:
  template<typename Tg, typename T, typename C>
  void bind(T& t, C& cmd)
  {
    typedef C command_type;
    typedef typename command_type::iprocessor iprocessor;
    iprocessor* ipc = t.get_aspect().template get<Tg>().get().template get<iprocessor>();
    if (ipc) cmd.push(ipc);
    else if (B) throw std::logic_error("command_binder: target dynamic interface is not set");
  }
};

template<typename C, typename L, bool B = true>
class command_binder
  : public command_binder_base<B>
  , public ap::type2type<C>
{
  typedef command_binder_base<B> super;
public:
  typedef C command_type;
  typedef typename ap::type_list_traits<L>::type target_tags_list;


  template<typename T>
  void operator()( T& t, C& cmd )
  {
    _bind( t, cmd, target_tags_list() );
  }

  template<typename T>
  void bind( T& t, C& cmd )
  {
    _bind( t, cmd, target_tags_list() );
  }

  template<typename Tg, typename T>
  void bind( T& t, C& cmd )
  {
     super::template bind<Tg>(t, cmd);
  }


  template<typename T, typename RL, typename LL >
  void _bind(T& t, C& cmd, ap::type_list<RL, LL> )
  {
    _bind(t, cmd, LL() );
    super::template bind<RL>(t, cmd);
  }

  template<typename T>
  void _bind(T& t, C& cmd, ap::empty_type ) {}
};


template<typename C, typename L>
class callback_binder
  : public command_binder<C, L>
{
public:
  typedef command_binder<C, L> super;
  typedef typename super::target_tags_list target_tags_list;
  template<typename T>
  void operator()( T& t, C& cmd )
  {
    if ( cmd.callback !=0 )
      cmd.push( cmd.callback );
    bind( t, cmd );
  }
};


}}

#endif

