#ifndef FAS_MISC_COMMAND_AUTO_BINDER_HPP
#define FAS_MISC_COMMAND_AUTO_BINDER_HPP

#include "binder.hpp"
#include "detail/auto_binder.hpp"

namespace fas{ namespace command{

namespace aa = ::fas::aop;

/** Абстрактый класс. Автоматически строит динамический интерфес 
  * и аспект необходимый для класса binder на базе списка биндеров комманд.
  * Используется если для каждой связываемой цели не требуеться особого интерфейса
  * Требует реализации для каждой команды биндера вида:
  * virtual void process( my_command& cmd ) { bind(cmd); }
  * @param BL - список биндеров комманд
  */
template< typename BL, typename A = aa::aspect<> >
class auto_binder
  : public binder< typename detail::binder_aspect_bulder< typename detail::interface_builder<BL>::type, BL, A >::type >
  , public detail::interface_builder<BL>::type
{
};

}}

#endif

