#pragma once

#include <sc-memory/sc_debug.hpp>

namespace common
{
class ActionCancelledException final : public utils::ScException
{
public:
  explicit ActionCancelledException(std::string const & description, std::string const & msg);
};

}  // namespace common
