#pragma once

/*!
 * @file fixed_strategy_search_template.hpp
 * @brief Multi-stage template processing with fixed init-next execution strategy.
 *
 * This module provides the FixedStrategySearchTemplate class, which implements a two-stage
 * template execution strategy against the sc-memory knowledge base. It orchestrates the
 * sequential application of an initial template followed by one or more "next" templates,
 * where the results of the initial template feed into the next template's arguments.
 *
 * FixedStrategySearchTemplate extends ParameterizedTemplate to support complex, multi-stage
 * query patterns where results from one template phase are used to parameterize subsequent
 * phases. This enables expressing dependencies between query stages, such as:
 * - "Find all entities of type X, then for each, find related entities of type Y"
 * - "Search for initial candidates, then filter/expand each candidate with additional criteria"
 * - "Multi-hop graph traversal where each hop depends on previous results"
 *
 * FixedStrategySearchTemplate is particularly useful for:
 * - Multi-stage queries with dependent stages
 * - Graph traversal patterns (find node, then find neighbors, then find neighbors' properties)
 * - Hierarchical searches (find categories, then find items in each category)
 * - Iterative refinement (broad search, then detailed search per result)
 * - Complex relationship queries requiring multiple template applications
 *
 * @see parameterized_template.hpp
 * @see ParameterizedTemplate
 * @see TemplateResults
 * @see ParameterizedTemplateBuilder
 * @see ScAgentContext
 */

#include "parameterized_template.hpp"

class ScAgentContext;

/*!
 * @class FixedStrategySearchTemplate
 * @brief Two-stage template execution with fixed init-next processing strategy.
 *
 * FixedStrategySearchTemplate provides a specialized implementation of ParameterizedTemplate
 * that orchestrates multi-stage template execution. It applies an initial template to
 * produce a set of results, then applies a next template to each initial result,
 * aggregating all outcomes into a unified result set.
 *
 * This approach enables expressing complex query patterns that would be difficult or
 * impossible to express in a single template. By chaining templates and propagating
 * results between stages, it supports dependencies where later query stages are
 * parameterized by earlier results.
 *
 * @inherits ParameterizedTemplate
 * @see ParameterizedTemplateBuilder
 * @see TemplateResults::ConnectTemplateResults
 * @see TemplateResults::MergeTemplateResults
 * @see ProcessNextTemplates
 *
 * @thread_safety Not thread-safe; external synchronization required for concurrent use.
 */
class FixedStrategySearchTemplate : public ParameterizedTemplate
{
  friend class ParameterizedTemplateBuilder;

public:
  /*!
   * @brief Destructor for FixedStrategySearchTemplate.
   *
   * Cleans up resources allocated by the FixedStrategySearchTemplate instance. Since
   * this class primarily works with sc-memory references (template addresses) rather
   * than owned resources, the destructor performs minimal cleanup beyond base class
   * destruction.
   */
  ~FixedStrategySearchTemplate();

protected:
  /// Address of the initial template to execute in the first stage.
  /// This template is applied with the original arguments to produce initial results
  /// that feed into the next stage. Retrieved from knowledge base via rrel_init_template.
  ScAddr m_initTemplateAddr;

  /// Address of the next template to execute in the second stage.
  /// This template is applied once for each initial result, with arguments combining
  /// the original arguments and the current init result. Retrieved from knowledge base
  /// via rrel_next_template. May be invalid if no next stage is configured.
  ScAddr m_nextTemplateAddr;

  /*!
   * @brief Executes the two-stage fixed strategy template processing.
   *
   * This method implements the core multi-stage execution logic for FixedStrategySearchTemplate.
   * It orchestrates the sequential application of init and next templates, managing argument
   * propagation and result aggregation across stages. The execution follows a fixed pattern:
   *
   * 1. Creates a fresh TemplateResults container with template
   *    configuration (addresses for template, result params, etc.). Note that sort and
   *    erase params are set to ScAddr::Empty for this template type.
   *
   * 2. Constructs the initial template from
   *    m_initTemplateAddr using ParameterizedTemplateBuilder and applies it with the
   *    provided arguments. If this fails, the entire operation fails immediately.
   *
   * 3. Adds the initial template results to the final output
   *    container if the output is currently empty (first results to be added).
   *
   * 4. If m_nextTemplateAddr is valid, invokes
   *    ProcessNextTemplates to apply the next template to each init result, aggregating
   *    all next-stage results into the final output.
   *
   * 5. Returns true only if both init and next stages (if present)
   *    succeeded; false if any stage failed.
   *
   * The method ensures that results flow correctly between stages: each next template
   * application receives both the original arguments and the current init result as
   * its input arguments, enabling query patterns where later stages depend on earlier
   * results.
   *
   * @param params      [in] Reference to template parameters extracted from the knowledge
   *                         base. Note: This parameter is not used in the current
   *                         implementation as init/next templates load their own parameters.
   *
   * @param arguments   [in] Reference to runtime arguments providing variable bindings
   *                         for the initial template. These arguments are propagated to
   *                         next template applications along with init results.
   *
   * @param results     [out] Reference to the results container. A fresh TemplateResults
   *                          object is created and assigned, containing aggregated results
   *                          from both init and next template stages.
   *
   * @param callbacks   [in] Reference to filter callback functions. Note: This parameter
   *                         is not used in the current implementation as filtering is
   *                         delegated to the composed templates.
   *
   * @return @c true if both init and next template stages (if configured) succeeded;
   *         @c false if init template failed or any next template application failed.
   *
   * @see ProcessNextTemplates
   * @see ParameterizedTemplateBuilder::BuildTemplate
   * @see TemplateResults::AddTemplateResults
   */
  bool ApplyImpl(
      ScTemplateParams const & params,
      TemplateArguments const & arguments,
      TemplateResults & results,
      std::list<FilterCallback> const & callbacks) const override;

  /*!
   * @brief Processes next template stage for all initial results.
   *
   * This helper method implements the next-stage processing loop, applying the next
   * template once for each result produced by the initial template. It manages argument
   * composition (combining original arguments with init results) and result aggregation
   * (merging or connecting next results with their corresponding init results).
   *
   * The method uses TemplateResults::AllOf to iterate over all init results, ensuring
   * that the next template is applied to each one. The aggregation strategy (merge vs.
   * connect) is determined by checking whether the init template is marked as a "search
   * set template" in the knowledge base.
   *
   * @param arguments             [in] Reference to the original arguments passed to ApplyImpl.
   *                                   These are combined with each init result to form
   *                                   arguments for next template applications.
   *
   * @param startTemplateResults  [in,out] Reference to the initial template results (despite
   *                                       parameter name using "start"). Each result is
   *                                       processed to apply the next template. The container
   *                                       is also used as the source for AllOf iteration.
   *                                       Note: Parameter name is "startTemplateResults" but
   *                                       should be "initTemplateResults" for consistency.
   *
   * @param results               [in,out] Reference to the final results container where
   *                                       aggregated next template results are accumulated.
   *                                       Each next template's results are either connected
   *                                       or merged with their corresponding init result.
   *
   * @return @c true if the next template was successfully applied to ALL init results;
   *         @c false if any next template application failed (fail-fast).
   *
   * @see TemplateResults::AllOf
   * @see TemplateResults::ConnectTemplateResults
   * @see TemplateResults::MergeTemplateResults
   * @see ParameterizedTemplateBuilder::BuildTemplate
   */
  bool ProcessNextTemplates(
      TemplateArguments const & arguments,
      TemplateResults & startTemplateResults,
      TemplateResults & results) const;

protected:
  /*!
   * @brief Protected constructor for initialization by ParameterizedTemplateBuilder.
   *
   * Constructs a FixedStrategySearchTemplate instance with the necessary context and
   * configuration for executing multi-stage template operations. This constructor is
   * protected to ensure that FixedStrategySearchTemplate instances are created only
   * through the builder pattern.
   *
   * The constructor delegates to the ParameterizedTemplate base class constructor to
   * establish agent context, logging, and template address references, then immediately
   * calls Load() to retrieve init and next template addresses from the knowledge base.
   *
   * @param context       [in] Reference to the ScAgentContext providing knowledge base
   *                           access and agent-specific operations for template construction
   *                           and execution across multiple stages.
   *
   * @param logger        [in] Reference to the ScLogger instance used for logging multi-stage
   *                           template execution, stage transitions, result aggregation,
   *                           and debugging information.
   *
   * @param templateAddr  [in] Constant reference to the ScAddr of the fixed strategy template
   *                           node in the knowledge base. This node should have role relations
   *                           (rrel_init_template, rrel_next_template) pointing to the
   *                           constituent template stages.
   *
   * @see ParameterizedTemplate::ParameterizedTemplate
   * @see ParameterizedTemplateBuilder
   * @see Load
   */
  FixedStrategySearchTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);

  /*!
   * @brief Loads init and next template addresses from the knowledge base.
   *
   * This method retrieves the configuration for the two-stage template execution by
   * reading role relations from the knowledge base. It looks for elements connected to
   * the template address via specific role relations:
   *
   * - rrel_init_template - identifies the initial template for the first stage
   * - rrel_next_template - identifies the next template for the second stage
   *
   * The method uses GetElementsByRoles (inherited from ScSet via ParameterizedTemplate)
   * to efficiently query for both role relations in a single operation. Retrieved addresses
   * are stored in m_initTemplateAddr and m_nextTemplateAddr member variables.
   *
   * @see GetElementsByRoles
   * @see Keynodes::rrel_init_template
   * @see Keynodes::rrel_next_template
   */
  void Load();
};