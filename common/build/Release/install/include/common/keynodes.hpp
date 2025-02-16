#pragma once

#include <sc-memory/sc_keynodes.hpp>

namespace common
{
class Keynodes : public ScKeynodes
{
public:
  static inline ScKeynode const atomic_logical_formula{"atomic_logical_formula", ScType::ConstNodeClass};
};

}  // namespace common
