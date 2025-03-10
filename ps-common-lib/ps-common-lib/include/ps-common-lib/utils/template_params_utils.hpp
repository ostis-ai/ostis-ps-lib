#pragma once

#include <sc-memory/sc_memory.hpp>

namespace common
{
class TemplateParamsUtils
{
public:
  static ScTemplateParams CreateTemplateParamsFromReplacements(
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements);

  static ScTemplateParams CreateTemplateParamsFromReplacements(
      ScMemoryContext * context,
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
      ScAddr const & structure);
};
}  // namespace common
