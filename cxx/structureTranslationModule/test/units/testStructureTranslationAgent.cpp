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

void testTranslator(ScMemoryContext & context, StructureTranslator & translator, std::string fileName, std::vector<std::string> answerPhrases)
{
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + fileName);
  ScAddr test_structure = context.HelperFindBySystemIdtf(TEST_STRUCTURE_ALIAS);
  std::string const & answer = translator.translate(test_structure).str();

  for (std::string const & phrase : answerPhrases)
  {
    EXPECT_TRUE(answer.find(phrase) != std::string::npos);
  }

  deinitializeClasses();
}

TEST_F(StructureTranslationTest, TestFromConceptTranslator)
{
  std::vector<std::string> answerPhrases = {"красный это цвет", "яблоко это фрукт"};
  ScMemoryContext & context = *m_ctx;
  FromConceptTranslator translator(&context);
  testTranslator(context, translator, "testFromConceptTranslator.scs", answerPhrases);
}

TEST_F(StructureTranslationTest, TestNrelInLinkTranslator)
{
  std::vector<std::string> answerPhrases = {"яблоко имя Арнольд"};
  ScMemoryContext & context = *m_ctx;
  NrelInLinkTranslator translator(&context);
  testTranslator(context, translator, "testNrelInLinkTranslator.scs", answerPhrases);
}

TEST_F(StructureTranslationTest, TestNrelInQuasybinaryLinkTranslator)
{
  std::vector<std::string> answerPhrases = {"человек синонимы хомо сапиенс", "человек синонимы людь"};
  ScMemoryContext & context = *m_ctx;
  NrelInQuasybinaryLinkTranslator translator(&context);
  testTranslator(context, translator, "testNrelInQuasybinaryLinkTranslator.scs", answerPhrases);
}

TEST_F(StructureTranslationTest, TestNrelFromQuasybinaryLinkTranslator)
{
  std::vector<std::string> answerPhrases = {"яблоко декомпозиция мякоть", "яблоко декомпозиция кожура"};
  ScMemoryContext & context = *m_ctx;
  NrelFromQuasybinaryLinkTranslator translator(&context);
  testTranslator(context, translator, "testNrelFromQuasybinaryLinkTranslator.scs", answerPhrases);
}

TEST_F(StructureTranslationTest, TestNrelInQuasybinaryNodeTranslator)
{
  std::vector<std::string> answerPhrases = {"человек родители первый родитель", "человек родители второй родитель"};
  ScMemoryContext & context = *m_ctx;
  NrelInQuasybinaryNodeTranslator translator(&context);
  testTranslator(context, translator, "testNrelInQuasybinaryNodeTranslator.scs", answerPhrases);
}

TEST_F(StructureTranslationTest, TestNrelFromQuasybinaryNodeTranslator)
{
  std::vector<std::string> answerPhrases = {"штука декомпозиция на части большая часть", "штука декомпозиция на части маленькая часть"};
  ScMemoryContext & context = *m_ctx;
  NrelFromQuasybinaryNodeTranslator translator(&context);
  testTranslator(context, translator, "testNrelFromQuasybinaryNodeTranslator.scs", answerPhrases);
}

TEST_F(StructureTranslationTest, TestNrelFromNodeTranslator)
{
  std::vector<std::string> answerPhrases = {"человек любит пицца", "человек любит есть пицца"};
  ScMemoryContext & context = *m_ctx;
  NrelFromNodeTranslator translator(&context);
  testTranslator(context, translator, "testNrelFromNodeTranslator.scs", answerPhrases);
}

TEST_F(StructureTranslationTest, TestAllTranslators)
{
  ScMemoryContext & context = *m_ctx;
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testAllTranslators.scs");
  ScAddr testStructure = context.HelperFindBySystemIdtf(TEST_STRUCTURE_ALIAS);
  auto translator1 = new NrelInLinkTranslator(&context);
  auto translator2 = new NrelInQuasybinaryLinkTranslator(&context);
  auto translator3 = new NrelFromQuasybinaryLinkTranslator(&context);
  auto translator4 = new NrelInQuasybinaryNodeTranslator(&context);
  auto translator5 = new NrelFromQuasybinaryNodeTranslator(&context);
  auto translator6 = new NrelFromNodeTranslator(&context);
  auto translator7 = new FromConceptTranslator(&context);

  std::set<StructureTranslator *, StructureTranslatorComporator> translators;
  translators.insert(translator1);
  translators.insert(translator2);
  translators.insert(translator3);
  translators.insert(translator4);
  translators.insert(translator5);
  translators.insert(translator6);
  translators.insert(translator7);

  std::stringstream translations;
  std::string answer;

  for (const auto & translator : translators)
  {
    std::stringstream const & translation = translator->translate(testStructure);
    translations << translation.str();
  }

  answer = translations.str();

  EXPECT_TRUE(answer.find("красный это цвет") != std::string::npos);
  EXPECT_TRUE(answer.find("яблоко это фрукт") != std::string::npos);

  EXPECT_TRUE(answer.find("яблоко имя Арнольд") != std::string::npos);

  EXPECT_TRUE(answer.find("мужик синонимы хомо сапиенс") != std::string::npos);
  EXPECT_TRUE(answer.find("мужик синонимы людь") != std::string::npos);

  EXPECT_TRUE(answer.find("яблоко декомпозиция мякоть") != std::string::npos);
  EXPECT_TRUE(answer.find("яблоко декомпозиция кожура") != std::string::npos);

  EXPECT_TRUE(answer.find("человек родители первый родитель") != std::string::npos);
  EXPECT_TRUE(answer.find("человек родители второй родитель") != std::string::npos);

  EXPECT_TRUE(answer.find("штука декомпозиция на части большая часть") != std::string::npos);
  EXPECT_TRUE(answer.find("штука декомпозиция на части маленькая часть") != std::string::npos);

  EXPECT_TRUE(answer.find("человек любит пицца") != std::string::npos);
  EXPECT_TRUE(answer.find("человек любит есть пицца") != std::string::npos);

  deinitializeClasses();
  for (const auto & translator : translators)
    delete translator;
}

TEST_F(StructureTranslationTest, TestAllAgent)
{
  ScMemoryContext & context = *m_ctx;
  initializeClasses();

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "testAgentOnAllTranslations.scs");
  ScAddr testQuestionNode = context.HelperFindBySystemIdtf(TEST_QUESTION_NODE_ALIAS);
  EXPECT_TRUE(testQuestionNode.IsValid());

  EXPECT_TRUE(utils::AgentUtils::applyAction(&context, testQuestionNode, WAIT_TIME));
  EXPECT_TRUE(context.HelperCheckEdge(
      scAgentsCommon::CoreKeynodes::question_finished_successfully,
      testQuestionNode,
      ScType::EdgeAccessConstPosPerm));
  ScAddr const & answer = utils::IteratorUtils::getAnyByOutRelation(&context, testQuestionNode, scAgentsCommon::CoreKeynodes::nrel_answer);
  ASSERT_TRUE(answer.IsValid());
  ScAddr const & answerLink = utils::IteratorUtils::getAnyFromSet(&context, answer);
  ASSERT_TRUE(context.GetElementType(answerLink).IsLink());

  deinitializeClasses();
}

}  // namespace StructureTranslationAgent
