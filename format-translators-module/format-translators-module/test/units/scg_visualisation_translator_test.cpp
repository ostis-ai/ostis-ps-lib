#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

#include "agent/visualisation-translator-agent/scg_visualisation_translator_agent.hpp"

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

std::string getResultLinkContent(ScAgentContext & context, ScAction & testAction)
{
  ScStructure const & result = testAction.GetResult();
  EXPECT_TRUE(context.IsElement(result));
  auto const & resultIterator = context.CreateIterator3(result, ScType::ConstPermPosArc, ScType::ConstNodeLink);
  EXPECT_TRUE(resultIterator->Next());
  ScAddr const & resultLink = resultIterator->Get(2);
  EXPECT_TRUE(context.GetElementType(resultLink).IsLink());
  std::string resultLinkContent;
  context.GetLinkContent(resultLink, resultLinkContent);
  return resultLinkContent;
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
  std::string const resultLinkContent = getResultLinkContent(context, testAction);
  std::string const expectedResult = readFile(FORMAT_TRANSLATORS_MODULE_EXPECTED_FILES_DIR_PATH + filename + ".gwf");
  EXPECT_EQ(resultLinkContent, expectedResult) << resultLinkContent;
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

TEST_F(FormatTranslatorsTest, aLotOfTriplesDoesNotCausesTimeoutTest)
{
  checkTranslation(*m_ctx, "a_lot_of_triples");
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

TEST_F(FormatTranslatorsTest, noMainKeyElementTest)
{
  checkTranslationWithError(*m_ctx, "no_main_key_element");
}

TEST_F(FormatTranslatorsTest, mainKeyElementIsConnectorTest)
{
  checkTranslationWithError(*m_ctx, "main_key_element_is_connector");
}

TEST_F(FormatTranslatorsTest, unsupportedConnectorTypeTest)
{
  checkTranslationWithError(*m_ctx, "unsupported_connector_type");
}

}  // namespace formatTranslatorsTest
