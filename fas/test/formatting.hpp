#ifndef FAS_TEST_FORMATTING_HPP
#define FAS_TEST_FORMATTING_HPP

namespace fas{ namespace test{

inline std::string red( const std::string& str )
{
  std::string res("\e[31m");
  res+=str;
  res+="\e[0m";
  return res;
}

inline std::string green( const std::string& str )
{
  std::string res("\e[32m");
  res+=str;
  res+="\e[0m";
  return res;
}


inline std::string yello( const std::string& str )
{
  std::string res("\e[33m");
  res+=str;
  res+="\e[0m";
  return res;
}

inline std::string blue( const std::string& str )
{
  std::string res("\e[34m");
  res+=str;
  res+="\e[0m";
  return res;
}

inline std::string light_blue( const std::string& str )
{
  std::string res("\e[36m");
  res+=str;
  res+="\e[0m";
  return res;
}


// suite без ошибок
struct _passed_{};
// unit без ошибок
struct _ok_{};
// ошибка unit'а
struct _fail_{};
// критическия ошибка unit'а, прекращаем все 
struct _fatal_{};
// пользовательсокое исключение в unit
struct _except_{};
// запуск unit
struct _run_{};
// пользовательский warning
struct _warning_{};
// пользовательское сообщение
struct _message_{};
// пользовательское сообщение об ошибке
struct _error_{};
//
struct _suite_beg_{};
//
struct _suite_end_{};

inline void PASSED(_passed_) {};
inline void OK(_ok_) {};
inline void FAIL(_fail_) {};
inline void EXCEPT(_except_) {};
inline void FATAL(_fatal_) {};
inline void RUN(_run_) {};
inline void WARNING(_warning_) {};
inline void MESSAGE(_message_) {};
inline void ERROR(_error_) {};

inline void SUITE_BEG(_suite_beg_) {};
inline void SUITE_END(_suite_end_) {};

}}

namespace std
{
  template<typename O> 
  O& operator<<(O& o, void (*)( ::fas::test::_passed_ ) )
  {
    return o << ::fas::test::green("[ PASSED  ] ");
  }

  template<typename O>
  O& operator<<(O& o, void (*)(::fas::test::_ok_) )
  {
    return o << ::fas::test::green("[      OK ] ");
  }

  template<typename O>
  O& operator<<(O& o, void (*)(::fas::test::_run_) )
  {
    return o << ::fas::test::green("[ RUN     ] ");
  }

  template<typename O>
  O& operator<<(O& o, void (*)(::fas::test::_fail_) )
  {
    return o << ::fas::test::red("[    FAIL ] ");
  }

  template<typename O>
  O& operator<<(O& o, void (*)(::fas::test::_except_) )
  {
    return o << ::fas::test::red("[  EXCEPT ] ");
  }

  template<typename O>
  O& operator<<(O& o, void (*)(::fas::test::_fatal_) )
  {
    return o << ::fas::test::red("[   FATAL ] ");
  }

  template<typename O>
  O& operator<<(O& o, void (*)(::fas::test::_warning_) )
  {
    return o << ::fas::test::yello("[ WARNING ] ");
  }

  template<typename O>
  O& operator<<(O& o, void (*)(::fas::test::_message_) )
  {
    return o << ::fas::test::blue("[ MESSAGE ] ");
  }

  template<typename O>
  O& operator<<(O& o, void (*)(::fas::test::_error_) )
  {
    return o << ::fas::test::red("[   ERROR ] ");
  }

  template<typename O>
  O& operator<<(O& o, void (*)(::fas::test::_suite_beg_) )
  {
    return o << ::fas::test::light_blue("[ --BEG-- ] ");
  }

  template<typename O>
  O& operator<<(O& o, void (*)(::fas::test::_suite_end_) )
  {
    return o << ::fas::test::light_blue("[ --END-- ] ");
  }

}

#endif
