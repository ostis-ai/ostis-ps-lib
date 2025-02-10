#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

#include "agent/visualisation-translator-agent/scg_visualisation_translator_agent.hpp"

namespace formatTranslatorsTest
{
ScsLoader loader;
const std::string FORMAT_TRANSLATORS_MODULE_TEST_FILES_DIR_PATH =
    FORMAT_TRANSLATORS_MODULE_TEST_SRC_PATH "/testStructures/";
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
  // std::ofstream outputFile(FORMAT_TRANSLATORS_MODULE_EXPECTED_FILES_DIR_PATH + "" + filename + ".gwf");
  // if (outputFile.is_open())
  // {
  //   outputFile << resultLinkContent;
  //   outputFile.close();
  // }
}

TEST_F(FormatTranslatorsTest, emptyKeyElementsTest)
{
  checkTranslation(*m_ctx, "emptyKeyElements");
}

TEST_F(FormatTranslatorsTest, structureWithoutConnectorsTest)
{
  checkTranslation(*m_ctx, "structureWithoutConnectors");
}

TEST_F(FormatTranslatorsTest, allSupportedNodeTypesTest)
{
  checkTranslation(*m_ctx, "allSupportedNodeTypes");
}

TEST_F(FormatTranslatorsTest, allSupportedConnectorTypesTest)
{
  checkTranslation(*m_ctx, "allSupportedConnectorTypes");
}

TEST_F(FormatTranslatorsTest, otherElementIsConnectorTest)
{
  checkTranslation(*m_ctx, "otherElementIsConnector");
}

TEST_F(FormatTranslatorsTest, outerBusEndsAtInnerBusTest)
{
  checkTranslation(*m_ctx, "outerBusEndsAtInnerBus");
}

TEST_F(FormatTranslatorsTest, outerBusEndsAfterInnerBusTest)
{
  checkTranslation(*m_ctx, "outerBusEndsAfterInnerBus");
}

TEST_F(FormatTranslatorsTest, multipleConnectorsBetweenNodesTest)
{
  checkTranslation(*m_ctx, "multipleConnectorsBetweenNodes");
}

TEST_F(FormatTranslatorsTest, noBussesTest)
{
  checkTranslation(*m_ctx, "noBusses");
}

TEST_F(FormatTranslatorsTest, hugeStructureDoesNotCausesTimeoutTest)
{
  checkTranslation(*m_ctx, "hugeStructure");
}

TEST_F(FormatTranslatorsTest, aLotOfTriplesDoesNotCausesTimeoutTest)
{
  checkTranslation(*m_ctx, "aLotOfTriples");
}

TEST_F(FormatTranslatorsTest, frenchIdentifiersTest)
{
  checkTranslation(*m_ctx, "frenchIdentifiers");
}

TEST_F(FormatTranslatorsTest, connectorHasIdentifierTest)
{
  checkTranslation(*m_ctx, "connectorHasIdentifier");
}

TEST_F(FormatTranslatorsTest, triplesSuccessfulTranslation)
{
  checkTranslation(*m_ctx, "triples");
}

TEST_F(FormatTranslatorsTest, quintuplesOtherSuccessfulTranslation)
{
  checkTranslation(*m_ctx, "quintuplesWithKeyOtherElements");
}

TEST_F(FormatTranslatorsTest, quintuplesRolesSuccessfulTranslation)
{
  checkTranslation(*m_ctx, "quintuplesWithKeyRoles");
}

TEST_F(FormatTranslatorsTest, complexSuccessfulTranslation)
{
  checkTranslation(*m_ctx, "complex");
}

TEST_F(FormatTranslatorsTest, xOverflow)
{
  checkTranslation(*m_ctx, "xOverflow");
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

TEST_F(FormatTranslatorsTest, emptyStructureTest)
{
  checkTranslation(*m_ctx, "emptyStructure");
}

TEST_F(FormatTranslatorsTest, noStructureTest)
{
  checkTranslationWithError(*m_ctx, "noStructure");
}

TEST_F(FormatTranslatorsTest, noKeyElementsTest)
{
  checkTranslationWithError(*m_ctx, "noKeyElements");
}

TEST_F(FormatTranslatorsTest, keyElementsHaveLoopTest)
{
  checkTranslationWithError(*m_ctx, "keyElementsHaveLoop");
}

TEST_F(FormatTranslatorsTest, keyElementsHaveConnectorTest)
{
  checkTranslationWithError(*m_ctx, "keyElementsHaveConnector");
}

TEST_F(FormatTranslatorsTest, noMainKeyElementTest)
{
  checkTranslationWithError(*m_ctx, "noMainKeyElement");
}

TEST_F(FormatTranslatorsTest, mainKeyElementIsConnectorTest)
{
  checkTranslationWithError(*m_ctx, "mainKeyElementIsConnector");
}

TEST_F(FormatTranslatorsTest, unsupportedConnectorTypeTest)
{
  checkTranslationWithError(*m_ctx, "unsupportedConnectorType");
}

}  // namespace formatTranslatorsTest
