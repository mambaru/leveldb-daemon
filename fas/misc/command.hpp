#ifndef FAS_MISC_COMMAND_COMMAND_HPP
#define FAS_MISC_COMMAND_COMMAND_HPP

#include <stack>

namespace fas{ namespace command{

struct icommand
{
  virtual ~icommand() {}
  virtual bool execute() = 0;
};

template<typename C>
struct icommand_processor
{
  typedef C command_type;
  virtual void process(C& cmd) = 0;
  virtual ~icommand_processor(){}
};

template<typename C>
class command
  : public C
{

public:
  typedef command<C> command_type;
  typedef icommand_processor< command<C> > iprocessor;
  typedef std::stack<iprocessor*> processor_stack;

  iprocessor* processor;
  iprocessor* callback;

  command(): C(), processor(0), callback(0) {}

  void clear() { /*_stack.clear();*/ while( !_stack.empty() ) _stack.pop(); }
  void push( iprocessor* p ) { if (p!=0) _stack.push(p); }
  bool empty() const { return _stack.empty(); }
  typename processor_stack::size_type size() const { return _stack.size(); }

  bool rebind()
  {
    if ( _stack.empty() ) 
      return false;

    _stack.pop();
    return !_stack.empty();
  }

  virtual bool execute()
  {
    iprocessor* p = processor;

    if (  p == 0 )
    {
      if ( _stack.empty() ) 
        return false;
      p = _stack.top();
      _stack.pop();
    }

    processor = 0;
    p->process(*this);

    return true;
  };

private:

  std::stack<iprocessor*> _stack;
};


}}

#endif

