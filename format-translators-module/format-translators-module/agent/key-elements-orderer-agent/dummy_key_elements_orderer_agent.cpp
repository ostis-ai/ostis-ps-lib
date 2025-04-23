#include "dummy_key_elements_orderer_agent.hpp"

#include <sc-agents-common/utils/IteratorUtils.hpp>

#include "keynodes/format_translators_keynodes.hpp"

namespace formatTranslators
{
ScAddr DummyKeyElementsOrdererAgent::GetActionClass() const
{
  return FormatTranslatorsKeynodes::action_order_key_elements;
}

bool DummyKeyElementsOrdererAgent::CheckInitiationCondition(ScActionInitiatedEvent const & event)
{
  return m_context.CheckConnector(GetActionClass(), event.GetOtherElement(), ScType::ConstPermPosArc);
}

ScResult DummyKeyElementsOrdererAgent::DoProgram(ScAction & action)
{
  try
  {
    ScAddr const & structure = action.GetArgument(1);
    if (!m_context.IsElement(structure))
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Cannot find structure to order key elements");

    ScAddr elementsOrder = utils::IteratorUtils::getAnyByOutRelation(
        &m_context, structure, FormatTranslatorsKeynodes::nrel_key_elements_order);
    if (!m_context.IsElement(elementsOrder))
    {
      ScAddrUnorderedSet elementsInOrder;
      elementsInOrder.insert(structure);

      ScAddr lastConnector;
      elementsOrder = m_context.GenerateNode(ScType::ConstNodeTuple);
      std::list const nodeTypes = {
          ScType::NodeSuperclass,
          ScType::NodeClass,
          ScType::NodeLink,
          ScType::NodeRole,
          ScType::NodeNonRole,
          ScType::NodeTuple,
          ScType::Node};

      for (auto const & nodeType : nodeTypes)
        AddNodesToOrder(structure, elementsOrder, elementsInOrder, lastConnector, nodeType);

      ScAddr const & arc = m_context.GenerateConnector(ScType::ConstCommonArc, structure, elementsOrder);
      m_context.GenerateConnector(ScType::ConstPermPosArc, FormatTranslatorsKeynodes::nrel_key_elements_order, arc);
    }

    action.FormResult(elementsOrder);
    return action.FinishSuccessfully();
  }
  catch (utils::ScException const & exception)
  {
    m_logger.Error(exception.Description());
    return action.FinishWithError();
  }
}

void DummyKeyElementsOrdererAgent::AddNodesToOrder(
    ScAddr const & structure,
    ScAddr const & elementsOrder,
    ScAddrUnorderedSet & elementsInOrder,
    ScAddr & lastMembershipArc,
    ScType const & nodesType) const
{
  auto const & nodesIterator = m_context.CreateIterator3(structure, ScType::ConstPermPosArc, nodesType);
  while (nodesIterator->Next())
  {
    ScAddr const & node = nodesIterator->Get(2);
    if (elementsInOrder.count(node))
      continue;
    elementsInOrder.insert(node);
    ScAddr const & membershipArc = m_context.GenerateConnector(ScType::ConstPermPosArc, elementsOrder, node);
    if (!m_context.IsElement(lastMembershipArc))
      m_context.GenerateConnector(ScType::ConstPermPosArc, FormatTranslatorsKeynodes::rrel_1, membershipArc);
    else
    {
      ScAddr const & commonArc = m_context.GenerateConnector(ScType::ConstCommonArc, lastMembershipArc, membershipArc);
      m_context.GenerateConnector(ScType::ConstPermPosArc, FormatTranslatorsKeynodes::nrel_basic_sequence, commonArc);
    }
    lastMembershipArc = membershipArc;
  }
}
}  // namespace formatTranslators
