#include "ActionCancelledException.hpp"

namespace nonAtomicActionInterpreterModule
{
ActionCancelledException::ActionCancelledException(std::string const & description, std::string const & msg)
  : ScException(description, msg)
{
}
}  // namespace nonAtomicActionInterpreterModule
