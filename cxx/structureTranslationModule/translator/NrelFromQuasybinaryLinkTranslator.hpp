#pragma once

#include "StructureTranslator.hpp"

namespace structureTranslationModule
{
/**
 * node _<= _nrel_relation:: {_[]};;
 */
class NrelFromQuasybinaryLinkTranslator : public StructureTranslator
{
public:

  explicit NrelFromQuasybinaryLinkTranslator(ScMemoryContext * context);
  std::vector<std::string> translate(ScAddr const & structAddr)
      const override;

private:
};

}  // namespace structureTranslationModule
