#ifndef FAS_MISC_COMMAND_BASIC_COMMAND_HPP
#define FAS_MISC_COMMAND_BASIC_COMMAND_HPP

/// Для json-rpc
namespace fas{ namespace command{

/// /////////////////////////////////////


struct command_type
{
  enum type_enum
  {
    undefined,
    request,
    response,
    notify
  };
};

struct command_state
{
  enum state_enum
  {
    unready,
    wait,
    ready
  };
};

struct command_status
{
  enum status_enum 
  {
    undefined,
    ok,
    error
  };
};

struct command_mode
{
  enum mode_enum
  {
    incoming,
    outgoing
  };
};

class property_type;
class property_state;
class property_status;
class property_mode;

class basic_command
{
  friend class property_type;
  friend class property_state;
  friend class property_status;
  friend class property_mode;

public:
  int id;
  typedef command_type::type_enum type_enum;
  typedef command_state::state_enum state_enum;
  typedef command_status::status_enum status_enum;
  typedef command_mode::mode_enum mode_enum;

  basic_command()
    : id(0), _type(), _state(), _status(), _mode()
  {}

// type
  type_enum type() const { return _type;}
  void type(type_enum type) { _type = type;}

  bool is_undefined( ) const { return _type == command_type::undefined;}
  bool is_request( ) const   { return _type == command_type::request;  }
  bool is_response( ) const  { return _type == command_type::response;  }
  bool is_notify( ) const    { return _type == command_type::notify;  }

  void as_request()   { _type = command_type::request;  this->wait();  }
  void as_response()  { _type = command_type::response; this->ready(); }
  void as_notify()    { _type = command_type::notify; if ( _state != command_state::wait )  _state = command_state::ready;  }
  void as_undefined() { _type = command_type::undefined; this->unready(); }

// state
  state_enum state() const { return _state;}
  void state(state_enum state) { _state = state;}

  bool is_unready() const { return _state == command_state::unready;}
  bool is_ready() const   { return _state == command_state::ready;  }
  bool is_wait() const    { return _state == command_state::wait;   }

  void unready() 
  {
    _state = command_state::unready; 
  }

  void ready()
  {
    _state = command_state::ready;
    if ( _status == command_status::undefined )
      _status = command_status::ok;
  }

  void wait()
  {
    _state = command_state::wait;
    _status = command_status::undefined;
  }

// status
  status_enum status() const { return _status; }
  void status(status_enum status) { _status = status; }

  void set_ok() { _status = command_status::ok; }
  void set_error() { _status = command_status::error; }

// mode
  mode_enum mode() const { return _mode;}
  void mode(mode_enum mode) { _mode = mode;}

  bool is_outgoing() const { return _mode == command_mode::outgoing;}
  bool is_incoming() const { return _mode == command_mode::incoming;}

  void as_outgoing() { _mode = command_mode::outgoing;}
  void as_incoming() { _mode = command_mode::incoming;}

  bool has_error() const { return _status == command_status::error;}

  void reset()
  {
    _type = type_enum();
    _state = state_enum();
    _status = status_enum();
    _mode = mode_enum();
  }

private:
   type_enum _type;
   state_enum _state;
   status_enum _status;
   mode_enum _mode;
};

class property_type
{
public:
  basic_command::type_enum operator() ( const basic_command& cmd) const { return cmd._type; }
  void operator()( basic_command& cmd, basic_command::type_enum v ) const { cmd._type = v; }
};

class property_state
{
public:
  basic_command::state_enum operator() ( const basic_command& cmd) const { return cmd._state; }
  void operator()( basic_command& cmd, basic_command::state_enum v ) const { cmd._state = v; }
};

class property_status
{
public:
  basic_command::status_enum operator() ( const basic_command& cmd) const { return cmd._status; }
  void operator()( basic_command& cmd, basic_command::status_enum v ) const { cmd._status = v; }
};

class property_mode
{
public:
  basic_command::mode_enum operator() ( const basic_command& cmd) const { return cmd._mode; }
  void operator()( basic_command& cmd, basic_command::mode_enum v ) const { cmd._mode = v; }
};


}}

#endif