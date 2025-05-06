#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

#include "agent/visualisation-translator-agent/scg_visualisation_translator_agent.hpp"

#include "keynodes/format_translators_keynodes.hpp"

#include <regex>

namespace formatTranslatorsTest
{
ScsLoader loader;
const std::string FORMAT_TRANSLATORS_MODULE_TEST_FILES_DIR_PATH = "test-structures/scg-visualisation-translator/";
const std::string FORMAT_TRANSLATORS_MODULE_EXPECTED_FILES_DIR_PATH =
    FORMAT_TRANSLATORS_MODULE_TEST_FILES_DIR_PATH + "/expected/";
int const WAIT_TIME = 1000;

using FormatTranslatorsTest = ScMemoryTest;

std::string readFile(std::string const & path)
{
  std::ifstream file(path);
  EXPECT_TRUE(file.is_open()) << "cannot open file " << path;
  return {(std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()};
}

void getResultLinkContent(ScAgentContext & context, ScAction & testAction, std::string & resultLinkContent)
{
  ScStructure const & result = testAction.GetResult();
  EXPECT_TRUE(context.IsElement(result));
  auto const & resultIterator = context.CreateIterator3(result, ScType::ConstPermPosArc, ScType::ConstNodeLink);
  EXPECT_TRUE(resultIterator->Next());
  ScAddr const & resultLink = resultIterator->Get(2);
  EXPECT_TRUE(context.GetElementType(resultLink).IsLink());
  context.GetLinkContent(resultLink, resultLinkContent);
}

void checkTranslation(ScAgentContext & context, std::string const & filename)
{
  loader.loadScsFile(context, FORMAT_TRANSLATORS_MODULE_TEST_FILES_DIR_PATH + filename + ".scs");
  context.SubscribeAgent<formatTranslators::SCgVisualisationTranslatorAgent>();

  ScAddr const & testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(context.IsElement(testActionNode));
  ScAction testAction = context.ConvertToAction(testActionNode);
  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  context.UnsubscribeAgent<formatTranslators::SCgVisualisationTranslatorAgent>();
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());
  std::string resultLinkContent;
  getResultLinkContent(context, testAction, resultLinkContent);
  std::regex pattern("sc_addr=\\\"\\d+\\\"");
  std::string const resultLinkContentWithoutAddr = std::regex_replace(resultLinkContent, pattern, "");
  std::string const expectedResult = readFile(FORMAT_TRANSLATORS_MODULE_EXPECTED_FILES_DIR_PATH + filename + ".gwf");
  EXPECT_EQ(resultLinkContentWithoutAddr, expectedResult) << resultLinkContentWithoutAddr;
}

TEST_F(FormatTranslatorsTest, emptyKeyElementsTest)
{
  checkTranslation(*m_ctx, "empty_key_elements");
}

TEST_F(FormatTranslatorsTest, structureWithoutConnectorsTest)
{
  checkTranslation(*m_ctx, "structure_without_connectors");
}

TEST_F(FormatTranslatorsTest, allSupportedNodeTypesTest)
{
  checkTranslation(*m_ctx, "all_supported_node_types");
}

TEST_F(FormatTranslatorsTest, allSupportedConnectorTypesTest)
{
  checkTranslation(*m_ctx, "all_supported_connector_types");
}

TEST_F(FormatTranslatorsTest, otherElementIsConnectorTest)
{
  checkTranslation(*m_ctx, "other_element_is_connector");
}

TEST_F(FormatTranslatorsTest, outerBusEndsAtInnerBusTest)
{
  checkTranslation(*m_ctx, "outer_bus_ends_at_inner_bus");
}

TEST_F(FormatTranslatorsTest, outerBusEndsAfterInnerBusTest)
{
  checkTranslation(*m_ctx, "outer_bus_ends_after_inner_bus");
}

TEST_F(FormatTranslatorsTest, multipleConnectorsBetweenNodesTest)
{
  checkTranslation(*m_ctx, "multiple_connectors_between_nodes");
}

TEST_F(FormatTranslatorsTest, noBussesTest)
{
  checkTranslation(*m_ctx, "no_busses");
}

TEST_F(FormatTranslatorsTest, hugeStructureDoesNotCausesTimeoutTest)
{
  checkTranslation(*m_ctx, "huge_structure");
}

TEST_F(FormatTranslatorsTest, frenchIdentifiersTest)
{
  checkTranslation(*m_ctx, "french_identifiers");
}

TEST_F(FormatTranslatorsTest, connectorHasIdentifierTest)
{
  checkTranslation(*m_ctx, "connector_has_identifier");
}

TEST_F(FormatTranslatorsTest, triplesSuccessfulTranslation)
{
  checkTranslation(*m_ctx, "triples");
}

TEST_F(FormatTranslatorsTest, quintuplesOtherSuccessfulTranslation)
{
  checkTranslation(*m_ctx, "quintuples_with_key_other_elements");
}

TEST_F(FormatTranslatorsTest, quintuplesRolesSuccessfulTranslation)
{
  checkTranslation(*m_ctx, "quintuples_with_key_roles");
}

TEST_F(FormatTranslatorsTest, complexSuccessfulTranslation)
{
  checkTranslation(*m_ctx, "complex");
}

TEST_F(FormatTranslatorsTest, xOverflow)
{
  checkTranslation(*m_ctx, "x_overflow");
}

TEST_F(FormatTranslatorsTest, defaultEnIdtfIsUsed)
{
  checkTranslation(*m_ctx, "default_en_idtf_is_used");
}

TEST_F(FormatTranslatorsTest, relationPriorityIsUsed)
{
  checkTranslation(*m_ctx, "relation_priority_is_used");
}

TEST_F(FormatTranslatorsTest, emptyStructureTest)
{
  checkTranslation(*m_ctx, "empty_structure");
}

TEST_F(FormatTranslatorsTest, noMainKeyElementTest)
{
  checkTranslation(*m_ctx, "no_main_key_element");
}

TEST_F(FormatTranslatorsTest, horizontlConnectorHasMultipleRelationsTest)
{
  checkTranslation(*m_ctx, "horizontal_connector_has_multiple_relations");
}

TEST_F(FormatTranslatorsTest, creatingVerticalGapBecauseOfLinkContentTest)
{
  checkTranslation(*m_ctx, "creating_vertical_gap_because_of_link_content");
}

void checkTranslationWithError(ScAgentContext & context, std::string const & filename)
{
  loader.loadScsFile(context, FORMAT_TRANSLATORS_MODULE_TEST_FILES_DIR_PATH + filename + ".scs");
  context.SubscribeAgent<formatTranslators::SCgVisualisationTranslatorAgent>();

  ScAddr const & testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(context.IsElement(testActionNode));
  ScAction testAction = context.ConvertToAction(testActionNode);
  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedWithError());
  context.UnsubscribeAgent<formatTranslators::SCgVisualisationTranslatorAgent>();
}

TEST_F(FormatTranslatorsTest, noStructureTest)
{
  checkTranslationWithError(*m_ctx, "no_structure");
}

TEST_F(FormatTranslatorsTest, noKeyElementsTest)
{
  checkTranslationWithError(*m_ctx, "no_key_elements");
}

TEST_F(FormatTranslatorsTest, keyElementsHaveLoopTest)
{
  checkTranslationWithError(*m_ctx, "key_elements_have_loop");
}

TEST_F(FormatTranslatorsTest, keyElementsHaveConnectorTest)
{
  checkTranslationWithError(*m_ctx, "key_elements_have_connector");
}

TEST_F(FormatTranslatorsTest, mainKeyElementIsConnectorTest)
{
  checkTranslationWithError(*m_ctx, "main_key_element_is_connector");
}

TEST_F(FormatTranslatorsTest, unsupportedConnectorTypeTest)
{
  checkTranslationWithError(*m_ctx, "unsupported_connector_type");
}

TEST_F(FormatTranslatorsTest, aLotOfTriplesDoesNotCausesTimeoutTest)
{
  // these are just two random not small numbers
  unsigned const TRIPLES_IN_STRUCTURE_COUNT = 828;
  unsigned const KEY_ELEMENTS_ORDER_SIZE = 881;
  ScAddr const & rootElement = m_ctx->ResolveElementSystemIdentifier("root_element", ScType::ConstNode);
  ScAddr const & conceptTestElement =
      m_ctx->ResolveElementSystemIdentifier("concept_test_element", ScType::ConstNodeClass);
  EXPECT_TRUE(m_ctx->IsElement(rootElement));
  EXPECT_TRUE(m_ctx->IsElement(conceptTestElement));
  ScStructure testStructure = m_ctx->GenerateStructure();
  testStructure << m_ctx->GenerateConnector(ScType::VarPermPosArc, conceptTestElement, rootElement);
  testStructure << conceptTestElement << conceptTestElement;
  for (unsigned i = 1; i <= TRIPLES_IN_STRUCTURE_COUNT; ++i)
  {
    ScAddr const & otherElement =
        m_ctx->ResolveElementSystemIdentifier(std::to_string(i) + "_not_root", ScType::ConstNode);
    testStructure << m_ctx->GenerateConnector(ScType::ConstPermPosArc, conceptTestElement, otherElement);
    testStructure << otherElement;
  }
  ScAddr const & mainKeyElementArc = m_ctx->GenerateConnector(ScType::ConstPermPosArc, testStructure, rootElement);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::rrel_main_key_sc_element, mainKeyElementArc);
  ScAddr const & keyElementsOrder = m_ctx->GenerateNode(ScType::ConstNodeTuple);
  ScAddr const & keyElementsArc = m_ctx->GenerateConnector(ScType::ConstCommonArc, testStructure, keyElementsOrder);
  m_ctx->GenerateConnector(
      ScType::ConstPermPosArc, formatTranslators::FormatTranslatorsKeynodes::nrel_key_elements_order, keyElementsArc);
  ScAddr arcToKeyElement = m_ctx->GenerateConnector(ScType::ConstPermPosArc, keyElementsOrder, conceptTestElement);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::rrel_1, arcToKeyElement);
  for (unsigned i = 1; i <= KEY_ELEMENTS_ORDER_SIZE; ++i)
  {
    ScAddr const nextArc = m_ctx->GenerateConnector(
        ScType::ConstPermPosArc,
        keyElementsOrder,
        m_ctx->ResolveElementSystemIdentifier(std::to_string(i) + "_not_root", ScType::ConstNode));
    ScAddr const & arcBetweenArcs = m_ctx->GenerateConnector(ScType::ConstCommonArc, arcToKeyElement, nextArc);
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_basic_sequence, arcBetweenArcs);
    arcToKeyElement = nextArc;
  }
  ScAction testAction = m_ctx->GenerateAction(formatTranslators::FormatTranslatorsKeynodes::action_visualise_to_scg);
  testAction.SetArguments(testStructure);

  m_ctx->SubscribeAgent<formatTranslators::SCgVisualisationTranslatorAgent>();

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  m_ctx->UnsubscribeAgent<formatTranslators::SCgVisualisationTranslatorAgent>();
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());
  std::string resultLinkContent;
  getResultLinkContent(*m_ctx, testAction, resultLinkContent);
  std::regex pattern("sc_addr=\\\"\\d+\\\"");
  std::string const resultLinkContentWithoutAddr = std::regex_replace(resultLinkContent, pattern, "");
  EXPECT_EQ(resultLinkContentWithoutAddr.size(), 372480u);
}

TEST_F(FormatTranslatorsTest, benchmarkDoesNotCausesTimeoutTest)
{
  unsigned const TRIPLES_IN_STRUCTURE_COUNT = 20000;
  unsigned const KEY_ELEMENTS_ORDER_SIZE = 20000;
  ScAddr const & rootElement = m_ctx->ResolveElementSystemIdentifier("root_element", ScType::ConstNode);
  ScAddr const & conceptTestElement =
      m_ctx->ResolveElementSystemIdentifier("concept_test_element", ScType::ConstNodeClass);
  EXPECT_TRUE(m_ctx->IsElement(rootElement));
  EXPECT_TRUE(m_ctx->IsElement(conceptTestElement));
  ScStructure testStructure = m_ctx->GenerateStructure();
  testStructure << m_ctx->GenerateConnector(ScType::VarPermPosArc, conceptTestElement, rootElement);
  testStructure << conceptTestElement << conceptTestElement;
  for (unsigned i = 1; i <= TRIPLES_IN_STRUCTURE_COUNT; ++i)
  {
    ScAddr const & otherElement =
        m_ctx->ResolveElementSystemIdentifier(std::to_string(i) + "_not_root", ScType::ConstNode);
    testStructure << m_ctx->GenerateConnector(ScType::ConstPermPosArc, conceptTestElement, otherElement);
    testStructure << otherElement;
  }
  ScAddr const & mainKeyElementArc = m_ctx->GenerateConnector(ScType::ConstPermPosArc, testStructure, rootElement);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::rrel_main_key_sc_element, mainKeyElementArc);
  ScAddr const & keyElementsOrder = m_ctx->GenerateNode(ScType::ConstNodeTuple);
  ScAddr const & keyElementsArc = m_ctx->GenerateConnector(ScType::ConstCommonArc, testStructure, keyElementsOrder);
  m_ctx->GenerateConnector(
      ScType::ConstPermPosArc, formatTranslators::FormatTranslatorsKeynodes::nrel_key_elements_order, keyElementsArc);
  ScAddr arcToKeyElement = m_ctx->GenerateConnector(ScType::ConstPermPosArc, keyElementsOrder, conceptTestElement);
  m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::rrel_1, arcToKeyElement);
  for (unsigned i = 1; i <= KEY_ELEMENTS_ORDER_SIZE; ++i)
  {
    ScAddr const nextArc = m_ctx->GenerateConnector(
        ScType::ConstPermPosArc,
        keyElementsOrder,
        m_ctx->ResolveElementSystemIdentifier(std::to_string(i) + "_not_root", ScType::ConstNode));
    ScAddr const & arcBetweenArcs = m_ctx->GenerateConnector(ScType::ConstCommonArc, arcToKeyElement, nextArc);
    m_ctx->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_basic_sequence, arcBetweenArcs);
    arcToKeyElement = nextArc;
  }
  ScAction testAction = m_ctx->GenerateAction(formatTranslators::FormatTranslatorsKeynodes::action_visualise_to_scg);
  testAction.SetArguments(testStructure);

  m_ctx->SubscribeAgent<formatTranslators::SCgVisualisationTranslatorAgent>();

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  m_ctx->UnsubscribeAgent<formatTranslators::SCgVisualisationTranslatorAgent>();
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());
  std::string resultLinkContent;
  getResultLinkContent(*m_ctx, testAction, resultLinkContent);
  EXPECT_FALSE(resultLinkContent.empty());
}

}  // namespace formatTranslatorsTest
