#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

#include "agent/key-elements-orderer-agent/dummy_key_elements_orderer_agent.hpp"
#include "keynodes/format_translators_keynodes.hpp"

namespace dummyKeyElementsOrdererTest
{
ScsLoader loader;
const std::string KEY_ELEMENTS_ORDERER_MODULE_TEST_FILES_DIR_PATH =
    FORMAT_TRANSLATORS_MODULE_TEST_SRC_PATH "/test-structures/key-elements-orderer/";
int const WAIT_TIME = 1000;

using DummyKeyElementsOrdererTest = ScMemoryTest;

TEST_F(DummyKeyElementsOrdererTest, expectedOrderOfNodeTypes)
{
  loader.loadScsFile(*m_ctx, KEY_ELEMENTS_ORDERER_MODULE_TEST_FILES_DIR_PATH + "all_supported_node_types.scs");
  m_ctx->SubscribeAgent<formatTranslators::DummyKeyElementsOrdererAgent>();

  ScAddr const & testActionAddr = m_ctx->SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(m_ctx->IsElement(testActionAddr));
  ScAction testAction = m_ctx->ConvertToAction(testActionAddr);
  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());
  ScStructure const & actionResult = testAction.GetResult();
  ScAddr const & structureFromAction = testAction.GetArgument(1);
  EXPECT_TRUE(m_ctx->IsElement(structureFromAction));
  auto const & keyElementsOrderIterator = m_ctx->CreateIterator5(
      structureFromAction,
      ScType::ConstCommonArc,
      ScType::ConstNode,
      ScType::ConstPermPosArc,
      formatTranslators::FormatTranslatorsKeynodes::nrel_key_elements_order);
  EXPECT_TRUE(keyElementsOrderIterator->Next());
  ScAddr const & keyElementsOrder = keyElementsOrderIterator->Get(2);
  EXPECT_TRUE(m_ctx->CheckConnector(actionResult, keyElementsOrder, ScType::ConstPermPosArc));

  std::vector const nodeTypes = {
      ScType::NodeClass, ScType::NodeLink, ScType::NodeRole, ScType::NodeNonRole, ScType::NodeTuple, ScType::Node};

  EXPECT_EQ(m_ctx->GetElementEdgesAndOutgoingArcsCount(keyElementsOrder), nodeTypes.size());

  auto const & firstElementIterator = m_ctx->CreateIterator5(
      keyElementsOrder, ScType::ConstPermPosArc, ScType::Node, ScType::ConstPermPosArc, ScKeynodes::rrel_1);
  EXPECT_TRUE(firstElementIterator->Next());
  ScAddr connectorFromOrder = firstElementIterator->Get(1);
  size_t elementIndex = 0;
  while (m_ctx->IsElement(connectorFromOrder) && elementIndex < nodeTypes.size())
  {
    ScAddr const & elementFromOrder = m_ctx->GetArcTargetElement(connectorFromOrder);
    EXPECT_EQ(m_ctx->GetElementType(elementFromOrder).BitAnd(nodeTypes[elementIndex]), nodeTypes[elementIndex]);
    elementIndex++;
    if (elementIndex != nodeTypes.size())
    {
      auto const & nextConnectorIterator = m_ctx->CreateIterator5(
          connectorFromOrder,
          ScType::ConstCommonArc,
          ScType::ConstPermPosArc,
          ScType::ConstPermPosArc,
          ScKeynodes::nrel_basic_sequence);
      EXPECT_TRUE(nextConnectorIterator->Next());
      connectorFromOrder = nextConnectorIterator->Get(2);
    }
  }
  EXPECT_EQ(elementIndex, nodeTypes.size());

  m_ctx->UnsubscribeAgent<formatTranslators::DummyKeyElementsOrdererAgent>();
}

TEST_F(DummyKeyElementsOrdererTest, orderStaysNotOverridden)
{
  loader.loadScsFile(*m_ctx, KEY_ELEMENTS_ORDERER_MODULE_TEST_FILES_DIR_PATH + "structure_already_has_order.scs");
  m_ctx->SubscribeAgent<formatTranslators::DummyKeyElementsOrdererAgent>();

  ScAddr const & testActionAddr = m_ctx->SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(m_ctx->IsElement(testActionAddr));
  ScAction testAction = m_ctx->ConvertToAction(testActionAddr);
  ScAddr const & structureFromAction = testAction.GetArgument(1);
  EXPECT_TRUE(m_ctx->IsElement(structureFromAction));
  auto const & keyElementsOrderIterator = m_ctx->CreateIterator5(
      structureFromAction,
      ScType::ConstCommonArc,
      ScType::ConstNode,
      ScType::ConstPermPosArc,
      formatTranslators::FormatTranslatorsKeynodes::nrel_key_elements_order);
  EXPECT_TRUE(keyElementsOrderIterator->Next());
  ScAddr const & keyElementsOrder = keyElementsOrderIterator->Get(2);
  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());
  ScStructure const & actionResult = testAction.GetResult();
  EXPECT_TRUE(m_ctx->CheckConnector(actionResult, keyElementsOrder, ScType::ConstPermPosArc));
  auto const & secondKeyElementsOrderIterator = m_ctx->CreateIterator5(
      structureFromAction,
      ScType::ConstCommonArc,
      ScType::ConstNode,
      ScType::ConstPermPosArc,
      formatTranslators::FormatTranslatorsKeynodes::nrel_key_elements_order);
  EXPECT_TRUE(secondKeyElementsOrderIterator->Next());
  EXPECT_EQ(secondKeyElementsOrderIterator->Get(2), keyElementsOrder);
  EXPECT_FALSE(secondKeyElementsOrderIterator->Next());

  m_ctx->UnsubscribeAgent<formatTranslators::DummyKeyElementsOrdererAgent>();
}

TEST_F(DummyKeyElementsOrdererTest, noStructureToOrder)
{
  loader.loadScsFile(*m_ctx, KEY_ELEMENTS_ORDERER_MODULE_TEST_FILES_DIR_PATH + "no_structure.scs");
  m_ctx->SubscribeAgent<formatTranslators::DummyKeyElementsOrdererAgent>();

  ScAddr const & testActionAddr = m_ctx->SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(m_ctx->IsElement(testActionAddr));
  ScAction testAction = m_ctx->ConvertToAction(testActionAddr);
  ScAddr const & structureFromAction = testAction.GetArgument(1);
  EXPECT_FALSE(m_ctx->IsElement(structureFromAction));
  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedWithError());

  m_ctx->UnsubscribeAgent<formatTranslators::DummyKeyElementsOrdererAgent>();
}

}  // namespace dummyKeyElementsOrdererTest
