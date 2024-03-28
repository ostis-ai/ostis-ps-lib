#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_memory.hpp"

namespace nonAtomicActionInterpreterModule
{
class TemplateParamsUtils
{
public:
  static ScTemplateParams createTemplateParamsFromReplacements(
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
      std::map<ScAddr, std::string, ScAddrLessFunc> const & resolvedVariableIdentifiers);

  static ScTemplateParams createTemplateParamsFromReplacements(
      ScMemoryContext * context,
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
      std::map<ScAddr, std::string, ScAddrLessFunc> const & resolvedVariableIdentifiers,
      ScAddr const & structure);
};
}  // namespace nonAtomicActionInterpreterModule
