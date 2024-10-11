#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>
#include <sc-memory/sc_agent.hpp>

#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "agent/NonAtomicActionInterpreterAgent.hpp"
#include "keynodes/Keynodes.hpp"
#include "keynodes/NonAtomicKeynodes.hpp"
#include "test/agent/QuestionFinishedSuccessfullyTestAgent.hpp"
#include "test/agent/QuestionFinishedTestAgent.hpp"
#include "test/agent/QuestionFinishedUnsuccessfullyTestAgent.hpp"
#include "test/agent/AssignDynamicArgumentTestAgent.hpp"
#include "test/agent/CheckDynamicArgumentTestAgent.hpp"
#include "test/keynodes/TestKeynodes.hpp"

using namespace nonAtomicActionInterpreterModule;

namespace nonAtomicActionInterpreterModuleTest
{
ScsLoader loader;
const std::string TEST_FILES_DIR_PATH = NON_ATOMIC_ACTION_INTERPRETER_MODULE_TEST_SRC_PATH "/testStructures/";
const int WAIT_TIME = 20000;

using NonAtomicActionInterpreterTest = ScMemoryTest;

void initialize(ScAgentContext & agentContext)
{
  agentContext.SubscribeAgent<NonAtomicActionInterpreterAgent>();
  agentContext.SubscribeAgent<QuestionFinishedSuccessfullyTestAgent>();
  agentContext.SubscribeAgent<QuestionFinishedTestAgent>();
  agentContext.SubscribeAgent<QuestionFinishedUnsuccessfullyTestAgent>();
  agentContext.SubscribeAgent<AssignDynamicArgumentTestAgent>();
  agentContext.SubscribeAgent<CheckDynamicArgumentTestAgent>();
}

void shutdown(ScAgentContext & agentContext)
{
  agentContext.UnsubscribeAgent<NonAtomicActionInterpreterAgent>();
  agentContext.UnsubscribeAgent<QuestionFinishedSuccessfullyTestAgent>();
  agentContext.UnsubscribeAgent<QuestionFinishedTestAgent>();
  agentContext.UnsubscribeAgent<QuestionFinishedUnsuccessfullyTestAgent>();
  agentContext.UnsubscribeAgent<AssignDynamicArgumentTestAgent>();
  agentContext.UnsubscribeAgent<CheckDynamicArgumentTestAgent>();
}

ScAction getFirstAction(ScAgentContext & context)
{
  ScAddr actionAddr;
  ScTemplate scTemplate;
  scTemplate.Triple(
      context.SearchElementBySystemIdentifier("test_nonatomic_action"),
      ScType::VarPermPosArc,
      ScType::VarNode >> "_nonAtomicAction");
  scTemplate.Quintuple(
      "_nonAtomicAction",
      ScType::VarCommonArc,
      ScType::VarNodeTuple >> "_tuple",
      ScType::VarPermPosArc,
      Keynodes::nrel_decomposition_of_action);
  scTemplate.Quintuple(
      "_tuple", ScType::VarPermPosArc, ScType::VarNode >> "_firstAction", ScType::VarPermPosArc, ScKeynodes::rrel_1);

  ScTemplateSearchResult results;
  context.SearchByTemplate(scTemplate, results);
  if (results.Size() == 1)
  {
    return context.ConvertToAction(results[0]["_firstAction"]);
  }
  SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Cannot find first action");
}

TEST_F(NonAtomicActionInterpreterTest, checkDynamicArguments)
{
  ScAgentContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "dynamicArguments.scs");
  initialize(context);

  ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  ScAction testAction = context.ConvertToAction(testActionNode);

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());

  ScAction action = getFirstAction(context);
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  ScIterator5Ptr iterator5 = context.CreateIterator5(
      action,
      ScType::ConstCommonArc,
      ScType::ConstNode,
      ScType::ConstPermPosArc,
      context.SearchElementBySystemIdentifier("nrel_goto"));
  EXPECT_TRUE(iterator5->Next());
  action = context.ConvertToAction(iterator5->Get(2));

  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  ScTemplate scTemplate;
  scTemplate.Quintuple(
      action, ScType::VarPermPosArc, ScType::VarNode >> "_dynamic_argument", ScType::VarPermPosArc, ScKeynodes::rrel_1);
  scTemplate.Triple("_dynamic_argument", ScType::VarTempPosArc, TestKeynodes::test_node >> "_dynamic_argument_value");
  ScTemplateSearchResult results;
  context.SearchByTemplate(scTemplate, results);
  EXPECT_TRUE(results.Size() == 1);

  EXPECT_TRUE(results[0]["_dynamic_argument_value"] == context.SearchElementBySystemIdentifier("test_node"));

  iterator5.reset();
  shutdown(context);
}

TEST_F(NonAtomicActionInterpreterTest, checkThenSequence)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "sucsesfullyFinishedSubaction.scs");
  initialize(context);

  ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  ScAction testAction = context.ConvertToAction(testActionNode);

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());

  ScAction action = getFirstAction(context);
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  ScIterator5Ptr iterator5 = context.CreateIterator5(
      action,
      ScType::ConstCommonArc,
      ScType::ConstNode,
      ScType::ConstPermPosArc,
      context.SearchElementBySystemIdentifier("nrel_then"));
  EXPECT_TRUE(iterator5->Next());
  action = context.ConvertToAction(iterator5->Get(2));

  EXPECT_TRUE(action.IsFinished());

  iterator5.reset();
  shutdown(context);
}

TEST_F(NonAtomicActionInterpreterTest, checkElseSequence)
{
  ScAgentContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "unsucsesfullyFinishedSubaction.scs");
  initialize(context);

  ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  ScAction testAction = context.ConvertToAction(testActionNode);

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());

  ScAction action = getFirstAction(context);
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedUnsuccessfully());

  ScIterator5Ptr iterator5 = context.CreateIterator5(
      action,
      ScType::ConstCommonArc,
      ScType::ConstNode,
      ScType::ConstPermPosArc,
      context.SearchElementBySystemIdentifier("nrel_else"));
  EXPECT_TRUE(iterator5->Next());
  action = context.ConvertToAction(iterator5->Get(2));

  EXPECT_TRUE(action.IsFinished());

  iterator5.reset();
  shutdown(context);
}

TEST_F(NonAtomicActionInterpreterTest, checkGotoSequence)
{
  ScAgentContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "finishedSubaction.scs");
  initialize(context);

  ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  ScAction testAction = context.ConvertToAction(testActionNode);

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());

  ScAction action = getFirstAction(context);
  EXPECT_TRUE(action.IsFinished());

  ScIterator5Ptr iterator5 = context.CreateIterator5(
      action,
      ScType::ConstCommonArc,
      ScType::ConstNode,
      ScType::ConstPermPosArc,
      context.SearchElementBySystemIdentifier("nrel_goto"));
  EXPECT_TRUE(iterator5->Next());
  action = context.ConvertToAction(iterator5->Get(2));

  EXPECT_TRUE(action.IsFinished());

  iterator5.reset();
  shutdown(context);
}

TEST_F(NonAtomicActionInterpreterTest, checkSuccessfulConditionalTransitionWithoutMatching)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "successfulConditionalTransitionWithoutMatching.scs");
  initialize(context);

  ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  ScAction testAction = context.ConvertToAction(testActionNode);

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());

  ScAction action = getFirstAction(context);
  EXPECT_TRUE(action.IsFinished());

  ScAddr const & gotoAction = utils::IteratorUtils::getAnyByOutRelation(&context, action, TestKeynodes::nrel_goto);
  EXPECT_TRUE(gotoAction.IsValid());
  action = context.ConvertToAction(gotoAction);
  EXPECT_TRUE(action.IsFinished());

  shutdown(context);
}

TEST_F(NonAtomicActionInterpreterTest, checkSuccessfulConditionalTransitionWithMatching)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "successfulConditionalTransitionWithMatching.scs");
  initialize(context);

  ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());

  ScAction testAction = context.ConvertToAction(testActionNode);

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());

  ScAction action = getFirstAction(context);
  EXPECT_TRUE(action.IsFinished());

  ScAddr const & gotoAction = utils::IteratorUtils::getAnyByOutRelation(&context, action, TestKeynodes::nrel_goto);
  EXPECT_TRUE(gotoAction.IsValid());
  action = context.ConvertToAction(gotoAction);
  EXPECT_TRUE(action.IsFinished());

  shutdown(context);
}

TEST_F(NonAtomicActionInterpreterTest, checkUnsuccessfulConditionalTransitionWithMatching)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "unsuccessfulConditionalTransitionWithMatching.scs");
  initialize(context);

  ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  ScAction testAction = context.ConvertToAction(testActionNode);

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());

  ScAction action = getFirstAction(context);
  EXPECT_TRUE(action.IsFinished());

  ScAddr const & gotoAction = utils::IteratorUtils::getAnyByOutRelation(&context, action, TestKeynodes::nrel_goto);
  EXPECT_TRUE(gotoAction.IsValid());
  action = context.ConvertToAction(gotoAction);
  EXPECT_FALSE(action.IsFinished());

  shutdown(context);
}

TEST_F(NonAtomicActionInterpreterTest, checkUnsuccessfulConditionalTransitionWithoutMatching)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "unsuccessfulConditionalTransitionWithoutMatching.scs");
  initialize(context);

  ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  ScAction testAction = context.ConvertToAction(testActionNode);

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());

  ScAction action = getFirstAction(context);
  EXPECT_TRUE(action.IsFinished());

  ScAddr const & gotoAction = utils::IteratorUtils::getAnyByOutRelation(&context, action, TestKeynodes::nrel_goto);
  EXPECT_TRUE(gotoAction.IsValid());
  action = context.ConvertToAction(gotoAction);
  EXPECT_FALSE(action.IsFinished());

  shutdown(context);
}

TEST_F(NonAtomicActionInterpreterTest, checkAllPathsInSequence)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "allPathsInSequence.scs");
  initialize(context);

  ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  ScAction testAction = context.ConvertToAction(testActionNode);

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());

  ScAction firstAction = getFirstAction(context);
  EXPECT_TRUE(firstAction.IsFinished());
  EXPECT_TRUE(firstAction.IsFinishedSuccessfully());

  ScAddr secondIncorrectActionAddr =
      utils::IteratorUtils::getAnyByOutRelation(&context, firstAction, TestKeynodes::nrel_goto);
  EXPECT_TRUE(secondIncorrectActionAddr.IsValid());
  ScAction const & secondIncorrectAction = context.ConvertToAction(secondIncorrectActionAddr);
  EXPECT_TRUE(secondIncorrectAction.IsFinished());

  ScAddr secondCorrectActionAddr =
      utils::IteratorUtils::getAnyByOutRelation(&context, firstAction, TestKeynodes::nrel_then);
  EXPECT_TRUE(secondCorrectActionAddr.IsValid());
  ScAction const & secondCorrectAction = context.ConvertToAction(secondCorrectActionAddr);
  EXPECT_FALSE(secondCorrectAction.IsFinished());

  shutdown(context);
}

TEST_F(NonAtomicActionInterpreterTest, notAllPathsInSequence)
{
  ScAgentContext & context = *m_ctx;
  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "notAllPathsInSequence.scs");
  initialize(context);

  ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  ScAction testAction = context.ConvertToAction(testActionNode);

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());

  ScAction action = getFirstAction(context);
  EXPECT_TRUE(action.IsFinished());
  EXPECT_TRUE(action.IsFinishedSuccessfully());

  ScAddr const & gotoAction = utils::IteratorUtils::getAnyByOutRelation(&context, action, TestKeynodes::nrel_goto);
  EXPECT_TRUE(gotoAction.IsValid());
  action = context.ConvertToAction(gotoAction);
  EXPECT_FALSE(action.IsFinished());

  shutdown(context);
}

TEST_F(NonAtomicActionInterpreterTest, checkArgumentsMatching)
{
  ScAgentContext & context = *m_ctx;

  loader.loadScsFile(context, TEST_FILES_DIR_PATH + "argumentsMatching.scs");
  initialize(context);

  ScAddr testActionNode = context.SearchElementBySystemIdentifier("test_action_node");
  EXPECT_TRUE(testActionNode.IsValid());
  ScAction testAction = context.ConvertToAction(testActionNode);

  EXPECT_TRUE(testAction.InitiateAndWait(WAIT_TIME));
  EXPECT_TRUE(testAction.IsFinishedSuccessfully());

  ScAction action = getFirstAction(context);
  EXPECT_TRUE(action.IsFinished());

  ScTemplate scTemplate;
  scTemplate.Quintuple(
      action,
      ScType::VarPermPosArc,
      context.SearchElementBySystemIdentifier("arg1"),
      ScType::VarPermPosArc,
      ScKeynodes::rrel_1);
  scTemplate.Quintuple(
      action,
      ScType::VarPermPosArc,
      context.SearchElementBySystemIdentifier("arg3"),
      ScType::VarPermPosArc,
      ScKeynodes::rrel_2);
  ScTemplateSearchResult results;
  context.SearchByTemplate(scTemplate, results);
  EXPECT_TRUE(results.Size() == 1);

  ScIterator5Ptr iterator5 = context.CreateIterator5(
      action,
      ScType::ConstCommonArc,
      ScType::ConstNode,
      ScType::ConstPermPosArc,
      context.SearchElementBySystemIdentifier("nrel_goto"));
  EXPECT_TRUE(iterator5->Next());
  action = context.ConvertToAction(iterator5->Get(2));
  EXPECT_TRUE(action.IsFinished());

  scTemplate.Quintuple(
      action,
      ScType::VarPermPosArc,
      context.SearchElementBySystemIdentifier("arg2"),
      ScType::VarPermPosArc,
      ScKeynodes::rrel_1);
  context.SearchByTemplate(scTemplate, results);
  EXPECT_TRUE(results.Size() == 1);

  iterator5.reset();
  shutdown(context);
}

}  // namespace nonAtomicActionInterpreterModuleTest
