/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "StructureTranslator.hpp"

namespace structureTranslationModule
{
/**
 * _node _=> _nrel:: _[];;
 */
class NrelInLinkTranslator : public StructureTranslator
{
public:
  explicit NrelInLinkTranslator(ScMemoryContext * context);

  std::stringstream translate(ScAddr const & structAddr) const override;

private:
};

}  // namespace structureTranslationModule
