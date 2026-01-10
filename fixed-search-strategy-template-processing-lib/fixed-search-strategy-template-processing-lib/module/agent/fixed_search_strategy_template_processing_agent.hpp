#pragma once

/*!
 * @file fixed_search_strategy_template_processing_agent.hpp
 * @brief Agent for processing fixed search strategy template execution requests.
 *
 * This module provides the FixedSearchStrategyTemplateProcessingAgent, an sc-agent that
 * handles action-initiated requests for executing fixed search strategy templates.
 * The agent serves as the entry point for processing complex multi-stage template
 * operations through the action-reaction mechanism of the sc-memory framework.
 *
 * ## Action Format
 *
 * The agent expects actions with the following structure:
 *
 * ```
 * action_instance
 *   -> rrel_1: template         (required: template to execute)
 *   -> rrel_2: arguments_set    (optional: set of input arguments)
 * ```
 *
 * ## Result Format
 *
 * On successful execution, the action result contains a generated structure with
 * positive arcs to all addresses involved in the template results (sets, arcs, elements).
 * This provides a complete reference to all knowledge base entities touched by the
 * template execution.
 *
 * ## Logging
 *
 * The agent maintains detailed logs in `logs/fixed_search_strategy_template_processing_agent.log`
 * with debug-level output for troubleshooting template execution issues.
 *
 * @see ScActionInitiatedAgent
 * @see FixedStrategySearchTemplate
 * @see ParameterizedTemplateBuilder
 * @see Keynodes::action_process_fixed_search_strategy_template
 */

#include <sc-memory/sc_agent.hpp>

/*!
 * @class FixedSearchStrategyTemplateProcessingAgent
 * @brief Action-initiated agent for executing fixed search strategy templates.
 *
 * FixedSearchStrategyTemplateProcessingAgent extends ScActionInitiatedAgent to provide
 * automated processing of template execution requests. The agent subscribes to actions
 * of class `action_process_fixed_search_strategy_template` and processes them by
 * building and executing the appropriate template with provided arguments.
 *
 * ## Integration with Template System
 *
 * The agent delegates template construction to ParameterizedTemplateBuilder, which
 * determines the appropriate template type (SearchTemplate, GenerateTemplate,
 * FixedStrategySearchTemplate, etc.) based on knowledge base configuration.
 * This allows the agent to handle any template type without type-specific logic.
 *
 * ## Usage Example
 *
 * @code
 * // In knowledge base, create an action:
 * action_node
 *   <- action_process_fixed_search_strategy_template;
 *   -> rrel_1: my_template_addr;
 *   -> rrel_2: my_arguments_set_addr.
 *
 * // Initiate the action (e.g., via another agent or external trigger)
 * action_initiated -> action_node.
 *
 * // Agent automatically executes template and stores results in action
 * // Result can be retrieved via action->result relation
 * @endcode
 *
 * @inherits ScActionInitiatedAgent
 * @see ScActionInitiatedAgent
 * @see ScActionInitiatedEvent
 * @see ScAction
 * @see ParameterizedTemplateBuilder
 *
 * @thread_safety Agent instances should not be shared across threads. The sc-memory
 *                framework manages agent instantiation and execution context.
 */
class FixedSearchStrategyTemplateProcessingAgent : public ScActionInitiatedAgent
{
public:
  /*!
   * @brief Default constructor initializing the agent with logging configuration.
   *
   * Constructs a FixedSearchStrategyTemplateProcessingAgent instance and initializes
   * its logger with file-based output to `logs/fixed_search_strategy_template_processing_agent.log`.
   * The logger is configured for debug-level output with auto-flushing enabled to ensure
   * all log messages are written immediately (useful for debugging template execution issues).
   *
   * The logger is stored in the inherited m_logger member from ScAgent and is used
   * throughout template execution for diagnostics and error reporting.
   */
  FixedSearchStrategyTemplateProcessingAgent();

  /*!
   * @brief Returns the action class this agent subscribes to.
   *
   * This method specifies the action class that triggers this agent's execution.
   * The sc-memory framework uses this information during agent registration to
   * establish the subscription, ensuring DoProgram is called whenever actions of
   * this class are initiated.
   *
   * @return ScAddr of the action class keynode `action_process_fixed_search_strategy_template`.
   *         This keynode identifies template processing action requests in the knowledge base.
   *
   * @see Keynodes::action_process_fixed_search_strategy_template
   * @see ScActionInitiatedAgent::GetActionClass
   */
  ScAddr GetActionClass() const override;

  /*!
   * @brief Executes the template processing workflow for an initiated action.
   *
   * This method implements the core agent logic, processing a template execution request
   * by extracting arguments, building the template, executing it, and packaging results.
   * The method is called automatically by the sc-memory framework when an action of the
   * subscribed class is initiated.
   *
   * @param event   [in] Constant reference to the action-initiated event containing
   *                     event metadata and the action instance address. Not directly
   *                     used in current implementation but available for event-specific
   *                     processing.
   *
   * @param action  [in,out] Reference to the ScAction object representing the action
   *                         being processed. Used to extract arguments, set results,
   *                         and control action completion status.
   *
   * @return ScResult indicating the overall outcome:
   *         - Success: Template executed and results packaged successfully
   *         - Failure: Template execution failed or error occurred
   *
   * @note The method uses m_context (inherited from ScAgent) for all knowledge base
   *       operations and m_logger for diagnostic output.
   *
   * @see ScActionInitiatedAgent::DoProgram
   * @see ScAction::GetArguments
   * @see ScAction::SetResult
   * @see ScAction::FinishSuccessfully
   * @see ScAction::FinishUnsuccessfully
   * @see ScAction::FinishWithError
   * @see ParameterizedTemplateBuilder::BuildTemplate
   * @see TemplateResults::IterateAll
   */
  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};
