#include "ps-common-lib/action_cancelled_exception.hpp"

namespace common
{
ActionCancelledException::ActionCancelledException(std::string const & description, std::string const & msg)
  : ScException(description, msg)
{
}
}  // namespace common
