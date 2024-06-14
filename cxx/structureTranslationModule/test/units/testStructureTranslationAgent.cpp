/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
 
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

void testTranslator(ScMemoryContext & context, auto & translator, std::string fileName, std::vector<std::string> answer_phrases)
{
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + fileName);
  ScAddr test_structure = context.HelperFindBySystemIdtf(TEST_STRUCTURE_ALIAS);
  auto const & answer = translator.translate(test_structure);

  for (std::string phrase : answer_phrases)
  {
    EXPECT_TRUE(answer.find(phrase) != std::string::npos) << phrase;
  }

  deinitializeClasses();
}

TEST_F(StructureTranslationTest, TestFromConceptTranslator)
{
  std::vector<std::string> answer_phrases = {"red is color", "apple is fruit"};
  ScMemoryContext & context = *m_ctx;
  FromConceptTranslator translator(&context);
  testTranslator(context, translator, "testFromConceptTranslator.scs", answer_phrases);
}

TEST_F(StructureTranslationTest, TestNrelInLinkTranslator)
{
  std::vector<std::string> answer_phrases = {"apple name Arnold"};
  ScMemoryContext & context = *m_ctx;
  NrelInLinkTranslator translator(&context);
  testTranslator(context, translator, "testNrelInLinkTranslator.scs", answer_phrases);
}

TEST_F(StructureTranslationTest, TestNrelInQuasybinaryLinkTranslator)
{
  std::vector<std::string> answer_phrases = {"man synonyms human", "man synonyms guy"};
  ScMemoryContext & context = *m_ctx;
  NrelInQuasybinaryLinkTranslator translator(&context);
  testTranslator(context, translator, "testNrelInQuasybinaryLinkTranslator.scs", answer_phrases);
}

TEST_F(StructureTranslationTest, TestNrelFromQuasybinaryLinkTranslator)
{
  std::vector<std::string> answer_phrases = {"apple decomposition pulp", "apple decomposition peel"};
  ScMemoryContext & context = *m_ctx;
  NrelFromQuasybinaryLinkTranslator translator(&context);
  testTranslator(context, translator, "testNrelFromQuasybinaryLinkTranslator.scs", answer_phrases);
}

TEST_F(StructureTranslationTest, TestNrelInQuasybinaryNodeTranslator)
{
  std::vector<std::string> answer_phrases = {"human parents first parent", "human parents second parent"};
  ScMemoryContext & context = *m_ctx;
  NrelInQuasybinaryNodeTranslator translator(&context);
  testTranslator(context, translator, "testNrelInQuasybinaryNodeTranslator.scs", answer_phrases);
}

TEST_F(StructureTranslationTest, TestNrelFromQuasybinaryNodeTranslator)
{
  std::vector<std::string> answer_phrases = {"thing parts decomposition big part", "thing parts decomposition little part"};
  ScMemoryContext & context = *m_ctx;
  NrelFromQuasybinaryNodeTranslator translator(&context);
  testTranslator(context, translator, "testNrelFromQuasybinaryNodeTranslator.scs", answer_phrases);
}

TEST_F(StructureTranslationTest, TestNrelFromNodeTranslator)
{
  std::vector<std::string> answer_phrases = {"man likes pizza", "man likes to eat pizza"};
  ScMemoryContext & context = *m_ctx;
  NrelFromNodeTranslator translator(&context);
  testTranslator(context, translator, "testNrelFromNodeTranslator.scs", answer_phrases);
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

  std::string answer;

  for (const auto & translator : translators)
  {
    std::string const & translation = translator->translate(test_structure);
    answer += translation;
  }

  EXPECT_TRUE(answer.find("red is color") != std::string::npos);
  EXPECT_TRUE(answer.find("apple is fruit") != std::string::npos);

  EXPECT_TRUE(answer.find("apple name Arnold") != std::string::npos);

  EXPECT_TRUE(answer.find("man synonyms human") != std::string::npos);
  EXPECT_TRUE(answer.find("man synonyms guy") != std::string::npos);

  EXPECT_TRUE(answer.find("apple decomposition pulp") != std::string::npos);
  EXPECT_TRUE(answer.find("apple decomposition peel") != std::string::npos);

  EXPECT_TRUE(answer.find("human parents first parent") != std::string::npos);
  EXPECT_TRUE(answer.find("human parents second parent") != std::string::npos);

  EXPECT_TRUE(answer.find("thing parts decomposition big part") != std::string::npos);
  EXPECT_TRUE(answer.find("thing parts decomposition little part") != std::string::npos);

  EXPECT_TRUE(answer.find("man likes pizza") != std::string::npos);
  EXPECT_TRUE(answer.find("man likes to eat pizza") != std::string::npos);

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
