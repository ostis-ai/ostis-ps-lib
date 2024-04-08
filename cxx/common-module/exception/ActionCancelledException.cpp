#include "ActionCancelledException.hpp"

namespace commonModule
{
ActionCancelledException::ActionCancelledException(std::string const & description, std::string const & msg)
  : ScException(description, msg)
{
}
}  // namespace commonModule
