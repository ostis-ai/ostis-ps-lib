#include "scs_loader.hpp"

#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "keynodes/TranslationKeynodes.hpp"

#include "agent/StructureTranslationAgent.hpp"

#include "translator/StructureTranslatorSet.hpp"
#include "translator/StructureTranslator.hpp"
#include "translator/NrelInLinkTranslator.hpp"
#include "translator/NrelInQuasybinaryLinkTranslator.hpp"
#include "translator/NrelInQuasybinaryNodeTranslator.hpp"
#include "translator/NrelFromQuasybinaryNodeTranslator.hpp"
#include "translator/FromConceptTranslator.hpp"
#include "translator/NrelFromNodeTranslator.hpp"
#include "translator/NrelFromQuasybinaryLinkTranslator.hpp"

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc_test.hpp"

#include <algorithm>

using namespace structureTranslationModule;

namespace StructureTranslationAgent
{
ScsLoader loader;
std::string const & TEST_FILES_DIR_PATH = STRUCTURE_TRANSLATION_MODULE_TEST_SRC_PATH "/testStructures/StructureTranslation/";
std::string const & TEST_QUESTION_NODE_ALIAS = "test_question_node";
std::string const & TEST_STRUCTURE_ALIAS = "test_structure";
int const WAIT_TIME = 3000;
size_t const MAX_TRANSLATIONS = 30;

using StructureTranslationTest = ScMemoryTest;

void initializeClasses()
{
  scAgentsCommon::CoreKeynodes::InitGlobal();
  TranslationKeynodes::InitGlobal();
  ScAgentInit(true);
  SC_AGENT_REGISTER(structureTranslationModule::StructureTranslationAgent);
}

void deinitializeClasses()
{
  SC_AGENT_UNREGISTER(structureTranslationModule::StructureTranslationAgent);
}

TEST_F(StructureTranslationTest, TestSetFillingWithPointers)
{
  ScMemoryContext & context = *m_ctx;
  auto translator1 = new NrelInLinkTranslator(&context);
  auto translator2 = new NrelInQuasybinaryLinkTranslator(&context);
  auto translator3 = new NrelFromQuasybinaryLinkTranslator(&context);
  auto translator4 = new NrelInQuasybinaryNodeTranslator(&context);
  auto translator5 = new NrelFromQuasybinaryNodeTranslator(&context);
  auto translator6 = new NrelFromNodeTranslator(&context);
  auto translator7 = new FromConceptTranslator(&context);

  std::set<StructureTranslator *, StructureTranslatorCmp> translators;
  translators.insert(translator1);
  translators.insert(translator2);
  translators.insert(translator3);
  translators.insert(translator4);
  translators.insert(translator5);
  translators.insert(translator6);
  translators.insert(translator7);
  translators.insert(translator7);
  translators.insert(translator6);
  translators.insert(translator5);
  translators.insert(translator4);
  translators.insert(translator3);
  translators.insert(translator2);
  translators.insert(translator1);
  EXPECT_EQ(translators.size(), 7u);
  for (const auto & item: translators)
    delete item;
}

TEST_F(StructureTranslationTest, TestFromConceptTranslator)
{
  ScMemoryContext & context = *m_ctx;
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testFromConceptTranslator.scs");
  ScAddr test_structure = context.HelperFindBySystemIdtf(TEST_STRUCTURE_ALIAS);

  auto translator = new FromConceptTranslator(&context);
  auto const & answer = translator->translate(test_structure);

  EXPECT_EQ(answer.size(), 2u);
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "red is color"), answer.cend());
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "apple is fruit"), answer.cend());

  deinitializeClasses();
  delete translator;
}

TEST_F(StructureTranslationTest, TestNrelInLinkTranslator)
{
  ScMemoryContext & context = *m_ctx;
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testNrelInLinkTranslator.scs");
  ScAddr test_structure = context.HelperFindBySystemIdtf(TEST_STRUCTURE_ALIAS);

  auto translator = new NrelInLinkTranslator(&context);
  auto const & answer = translator->translate(test_structure);

  EXPECT_EQ(answer.size(), 1u);
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "apple name Arnold"), answer.cend());

  deinitializeClasses();
  delete translator;
}

TEST_F(StructureTranslationTest, TestNrelInQuasybinaryLinkTranslator)
{
  ScMemoryContext & context = *m_ctx;
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testNrelInQuasybinaryLinkTranslator.scs");
  ScAddr test_structure = context.HelperFindBySystemIdtf(TEST_STRUCTURE_ALIAS);

  auto translator = new NrelInQuasybinaryLinkTranslator(&context);
  auto const & answer = translator->translate(test_structure);

  EXPECT_EQ(answer.size(), 2u);
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "man synonyms human"), answer.cend());
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "man synonyms guy"), answer.cend());

  deinitializeClasses();
  delete translator;
}

TEST_F(StructureTranslationTest, TestNrelFromQuasybinaryLinkTranslator)
{
  ScMemoryContext & context = *m_ctx;
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testNrelFromQuasybinaryLinkTranslator.scs");
  ScAddr test_structure = context.HelperFindBySystemIdtf(TEST_STRUCTURE_ALIAS);

  auto translator = new NrelFromQuasybinaryLinkTranslator(&context);
  auto const & answer = translator->translate(test_structure);

  EXPECT_EQ(answer.size(), 2u);
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "apple decomposition pulp"), answer.cend());
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "apple decomposition peel"), answer.cend());

  deinitializeClasses();
  delete translator;
}

TEST_F(StructureTranslationTest, TestNrelInQuasybinaryNodeTranslator)
{
  ScMemoryContext & context = *m_ctx;
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testNrelInQuasybinaryNodeTranslator.scs");
  ScAddr test_structure = context.HelperFindBySystemIdtf(TEST_STRUCTURE_ALIAS);

  auto translator = new NrelInQuasybinaryNodeTranslator(&context);
  auto const & answer = translator->translate(test_structure);

  EXPECT_EQ(answer.size(), 2u);
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "human parents first parent"), answer.cend());
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "human parents second parent"), answer.cend());

  deinitializeClasses();
  delete translator;
}

TEST_F(StructureTranslationTest, TestNrelFromQuasybinaryNodeTranslator)
{
  ScMemoryContext & context = *m_ctx;
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testNrelFromQuasybinaryNodeTranslator.scs");
  ScAddr test_structure = context.HelperFindBySystemIdtf(TEST_STRUCTURE_ALIAS);

  auto translator = new NrelFromQuasybinaryNodeTranslator(&context);
  auto const & answer = translator->translate(test_structure);

  EXPECT_EQ(answer.size(), 2u);
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "thing parts decomposition big part"), answer.cend());
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "thing parts decomposition little part"), answer.cend());

  deinitializeClasses();
  delete translator;
}

TEST_F(StructureTranslationTest, TestNrelFromNodeTranslator)
{
  ScMemoryContext & context = *m_ctx;
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testNrelFromNodeTranslator.scs");
  ScAddr test_structure = context.HelperFindBySystemIdtf(TEST_STRUCTURE_ALIAS);

  auto translator = new NrelFromNodeTranslator(&context);
  auto const & answer = translator->translate(test_structure);

  EXPECT_EQ(answer.size(), 2u);
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "man likes pizza"), answer.cend());
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "man likes to eat pizza"), answer.cend());

  deinitializeClasses();
  delete translator;
}

TEST_F(StructureTranslationTest, TestAllTranslators)
{
  ScMemoryContext & context = *m_ctx;
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testAllTranslators.scs");
  ScAddr test_structure = context.HelperFindBySystemIdtf(TEST_STRUCTURE_ALIAS);
  auto translator1 = new NrelInLinkTranslator(&context);
  auto translator2 = new NrelInQuasybinaryLinkTranslator(&context);
  auto translator3 = new NrelFromQuasybinaryLinkTranslator(&context);
  auto translator4 = new NrelInQuasybinaryNodeTranslator(&context);
  auto translator5 = new NrelFromQuasybinaryNodeTranslator(&context);
  auto translator6 = new NrelFromNodeTranslator(&context);
  auto translator7 = new FromConceptTranslator(&context);

  std::set<StructureTranslator *, StructureTranslatorCmp> translators;
  translators.insert(translator1);
  translators.insert(translator2);
  translators.insert(translator3);
  translators.insert(translator4);
  translators.insert(translator5);
  translators.insert(translator6);
  translators.insert(translator7);

  std::set<std::string> answer;

  for (const auto & translator : translators)
  {
    std::vector<std::string> const & translations = translator->translate(test_structure);
    std::copy(translations.begin(), translations.end(), std::inserter(answer, answer.cend()));
  }

  EXPECT_EQ(answer.size(), 13u);

  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "red is color"), answer.cend());
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "apple is fruit"), answer.cend());

  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "apple name Arnold"), answer.cend());

  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "man synonyms human"), answer.cend());
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "man synonyms guy"), answer.cend());

  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "apple decomposition pulp"), answer.cend());
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "apple decomposition peel"), answer.cend());

  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "human parents first parent"), answer.cend());
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "human parents second parent"), answer.cend());

  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "thing parts decomposition big part"), answer.cend());
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "thing parts decomposition little part"), answer.cend());

  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "man likes pizza"), answer.cend());
  EXPECT_NE(std::find(answer.cbegin(), answer.cend(), "man likes to eat pizza"), answer.cend());

  deinitializeClasses();
  for (const auto & translator : translators)
    delete translator;
}

TEST_F(StructureTranslationTest, TestAllAgent)
{
  ScMemoryContext & context = *m_ctx;
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testAgentOnAllTranslations.scs");
  ScAddr test_question_node = context.HelperFindBySystemIdtf(TEST_QUESTION_NODE_ALIAS);
  EXPECT_TRUE(test_question_node.IsValid());

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, test_question_node, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully,
      test_question_node,
      ScType::EdgeAccessConstPosPerm));
  ScAddr const & answer = utils::IteratorUtils::getAnyByOutRelation(&context, test_question_node, scAgentsCommon::CoreKeynodes::nrel_answer);
  ASSERT_TRUE(answer.IsValid());
  ScAddr const & answerLink = utils::IteratorUtils::getAnyFromSet(&context, answer);
  ASSERT_TRUE(context.GetElementType(answerLink).IsLink());

  deinitializeClasses();
}

}  // namespace StructureTranslationAgent
