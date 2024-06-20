
/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/kpm/sc_agent.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "StructureTranslationAgent.generated.hpp"

#include "translator/StructureTranslatorSet.hpp"

namespace structureTranslationModule
{
class StructureTranslationAgent : public ScAgent
{
public:
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  bool checkActionClass(ScAddr const & actionNode);
  std::string translateStructure(ScAddr const & structAddr, ScMemoryContext * context);
};

}  // namespace structureTranslationModule
