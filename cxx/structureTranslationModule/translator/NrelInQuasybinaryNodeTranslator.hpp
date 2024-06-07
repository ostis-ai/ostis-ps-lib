#pragma once

#include "StructureTranslator.hpp"

namespace structureTranslationModule
{
/**
 * node _=> _nrel_relation:: {_quasybinary_node};;
 */
class NrelInQuasybinaryNodeTranslator : public StructureTranslator
{
public:
  explicit NrelInQuasybinaryNodeTranslator(ScMemoryContext * context);

  std::vector<std::string> translate(ScAddr const & structAddr)
      const override;

private:
};


}  // namespace structureTranslationModule
