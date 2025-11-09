#pragma once

/*!
 * @file ParameterizedTemplate.hpp
 * @brief Base class for parameterized template processing in the sc-memory knowledge base system.
 *
 * This module defines the abstract base class for templates that can be applied with
 * variable arguments to perform pattern matching and filtering operations against
 * the semantic memory knowledge base. It provides infrastructure for managing template
 * parameters, filter constraints, and result collection through a unified interface.
 *
 * The ParameterizedTemplate class extends ScSet to inherit collection semantics and
 * provides mechanisms for:
 * - Applying templates with dynamic parameters to knowledge base queries
 * - Managing positive and negative filter constraints
 * - Handling template configuration and metadata through sc-memory addresses
 * - Supporting time-based operations with configurable wait times
 * - Processing input/output parameters and result ordering
 *
 * @see template_arguments.hpp
 * @see template_results.hpp
 * @see ScAgentContext
 * @see FilterTemplate
 * @see NotFilterTemplate
 */

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_set.hpp>
#include <sc-memory/utils/sc_logger.hpp>

#include "template_arguments.hpp"
#include "template_results.hpp"

class ScAgentContext;
class FilterTemplate;
class NotFilterTemplate;

/*!
 * @class ParameterizedTemplate
 * @brief Abstract base class for templates that apply with parameterized arguments.
 *
 * ParameterizedTemplate provides a framework for executing complex query patterns
 * against the sc-memory knowledge base with variable parameters. Derived classes
 * implement specific template logic through the ApplyImpl virtual method, while
 * this base class handles common concerns such as filtering, parameter management,
 * and agent context coordination.
 *
 * This class is designed for use within sc-agent systems to enable dynamic,
 * constraint-based pattern matching and knowledge base transformations.
 *
 * @inherits ScSet
 * @see TemplateArguments
 * @see TemplateResults
 *
 * @thread_safety Methods are not thread-safe; external synchronization is required
 *                 for concurrent access to the same instance.
 */
class ParameterizedTemplate : public ScSet
{
public:
  /*!
   * @brief Virtual destructor for proper cleanup of derived classes.
   *
   * Ensures that resources allocated by derived template implementations
   * are properly released when instances are destroyed.
   */
  virtual ~ParameterizedTemplate();

  /*!
   * @brief Applies the template to the given arguments and collects results.
   *
   * This is the primary public interface for template execution. It accepts
   * a set of template arguments (variable bindings and constraints) and
   * executes the template logic, collecting matched results into the provided
   * TemplateResults container. The method coordinates filtering, parameter
   * binding, and result collection.
   *
   * The actual template execution logic is delegated to the ApplyImpl virtual
   * method, which subclasses must implement with their specific semantics.
   *
   * @param arguments   [in] Reference to the template arguments providing
   *                         variable bindings, entity references, and constraint
   *                         specifications for this template application.
   *
   * @param results     [out] Reference to a TemplateResults container where
   *                          all matched results are accumulated. Results are
   *                          appended to existing content in this container.
   *
   * @return @c true if the template was successfully applied and results were
   *         collected (may include zero results if no patterns matched);
   *         @c false if a fatal error occurred during template application.
   *
   * @see ApplyImpl
   * @see TemplateArguments
   * @see TemplateResults
   */
  bool Apply(TemplateArguments const & arguments, TemplateResults & results) const;

protected:
  /*!
   * @brief Pure virtual method for implementing specific template application logic.
   *
   * This method must be implemented by derived classes to define the concrete
   * semantics of how a template processes its arguments and generates results.
   * The implementation receives pre-processed parameters, argument bindings,
   * and filter callbacks, allowing it to focus on core template logic without
   * worrying about filter management or parameter marshalling.
   *
   * The method should populate the @p results container with all matches found
   * during template execution, respecting any constraints imposed by the filter
   * callbacks.
   *
   * @param params      [in] Reference to parsed template parameters extracted
   *                         from the knowledge base, containing template-specific
   *                         configuration and metadata.
   *
   * @param arguments   [in] Reference to runtime template arguments providing
   *                         variable bindings and entity constraints for pattern
   *                         matching against the knowledge base.
   *
   * @param results     [out] Reference to the result collection where all matched
   *                          patterns and bindings should be accumulated.
   *
   * @param callbacks   [in] Reference to a list of filter callback functions
   *                         that define additional constraints and filtering
   *                         criteria to apply during template matching.
   *
   * @return @c true if template execution completed successfully (even if no
   *         results were found); @c false if a critical error prevented normal
   *         execution.
   *
   * @see Apply
   */
  virtual bool ApplyImpl(
      ScTemplateParams const & params,
      TemplateArguments const & arguments,
      TemplateResults & results,
      std::list<FilterCallback> const & callbacks) const = 0;

protected:
  /// Reference to message reply context for knowledge base access and semantic memory queries.
  /// Provides the connection to the sc-memory instance and handles all semantic memory operations.
  ScAgentContext & m_replyContext;

  /// Reference to system logger for debugging and error reporting.
  /// Used to record template execution events, warnings, and errors for diagnostics.
  utils::ScLogger & m_logger;

  /// Address of the primary action template node in the knowledge base.
  /// References the main template entity that defines the pattern to be matched.
  ScAddr m_templateAddr;

  /// Address of the action template type identifier.
  /// Specifies the semantic type or category of the template for runtime classification.
  ScAddr m_templateTypeAddr;

  /// Address of the wait time configuration for time-based operations.
  /// Stores the timeout or delay duration (in milliseconds) for asynchronous operations.
  ScAddr m_waitTimeMsAddr;

  /// Address of the sorting criteria entity class for result ordering.
  /// References the parameter or property used to sort template results.
  ScAddr m_sortParamAddr;

  /// Address of the input parameters set defining required entities.
  /// Identifies which entities and properties must be provided as input to the template.
  ScAddr m_inputParamsAddr;

  /// Address of parameters to erase after template processing.
  /// Specifies which parameters or temporary entities should be removed from results
  /// or cleaned up after template execution completes.
  ScAddr m_eraseParamsAddr;

  /// Address of result parameters set for output configuration.
  /// Defines which entities and properties should be included in the output results.
  ScAddr m_resultParamsAddr;

  /// Collection of filter template addresses for positive filtering.
  /// Stores all template addresses that must match (inclusion constraints) for a result to be valid.
  ScAddrUnorderedSet m_filterTemplateAddrs;

  /// Collection of not-filter template addresses for negative filtering.
  /// Stores all template addresses that must NOT match (exclusion constraints) for a result to be valid.
  ScAddrUnorderedSet m_notFilterTemplateAddrs;

  /*!
   * @}
   */

  /*!
   * @brief Protected constructor for initialization by derived classes.
   *
   * Initializes the template with references to the agent context, logger, and
   * the sc-memory set address. This constructor is protected to enforce use through
   * derived classes, ensuring type-specific template semantics are defined.
   *
   * @param context   [in] Reference to the ScAgentContext providing knowledge base
   *                       access and agent-specific functionality.
   *
   * @param logger    [in] Reference to the ScLogger instance for logging template
   *                       execution events and diagnostic information.
   *
   * @param setAddr   [in] Constant reference to the ScAddr of the template set node
   *                       in the knowledge base that defines this template's structure.
   *
   * @see ScAgentContext
   * @see utils::ScLogger
   */
  ParameterizedTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & setAddr);

  /*!
   * @brief Loads template configuration and metadata from the knowledge base.
   *
   * This method initializes the template by reading its configuration data from
   * the sc-memory knowledge base. It populates member variables such as
   * m_templateAddr, m_inputParamsAddr, filter template collections, and other
   * metadata based on semantic relationships and constraints defined in the
   * knowledge base.
   *
   * This method should be called during template initialization to establish
   * all necessary configuration before the template is applied.
   *
   * @note The Load method is typically called from the constructor or immediately
   *       after construction to initialize the template state.
   *
   * @see m_templateAddr
   * @see m_inputParamsAddr
   * @see m_filterTemplateAddrs
   * @see m_notFilterTemplateAddrs
   */
  void Load();

  /*!
   * @brief Applies a positive filter constraint to validate template arguments.
   *
   * This method executes a filter template specified by @p filterTemplateAddr
   * against the provided template arguments. A positive filter must match
   * (return @c true) for the result to satisfy filtering constraints.
   *
   * Positive filters define inclusion criteria—results that fail this filter
   * should be excluded from the final template results.
   *
   * @param filterTemplateAddr  [in] Address of the filter template to apply.
   *                                 Must be a valid sc-memory address referencing
   *                                 a FilterTemplate entity.
   *
   * @param arguments           [in] Reference to template arguments to test against
   *                                 the filter criteria.
   *
   * @return @c true if the filter matched the arguments (constraint satisfied);
   *         @c false if the filter did not match (constraint violated).
   *
   * @see NotFilter
   * @see FilterTemplate
   */
  bool Filter(ScAddr const & filterTemplateAddr, TemplateArguments const & arguments) const;

  /*!
   * @brief Applies a negative filter constraint to validate template arguments.
   *
   * This method executes an exclusion filter template specified by
   * @p notFilterTemplateAddr against the provided template arguments. A negative
   * filter must NOT match (return @c false) for the result to satisfy filtering
   * constraints.
   *
   * Negative filters define exclusion criteria—results that match this filter
   * should be excluded from the final template results.
   *
   * @param notFilterTemplateAddr  [in] Address of the not-filter template to apply.
   *                                    Must be a valid sc-memory address referencing
   *                                    a NotFilterTemplate entity.
   *
   * @param arguments              [in] Reference to template arguments to test against
   *                                    the exclusion filter criteria.
   *
   * @return @c true if the filter did NOT match the arguments (constraint satisfied);
   *         @c false if the filter matched the arguments (constraint violated).
   *
   * @see Filter
   * @see NotFilterTemplate
   */
  bool NotFilter(ScAddr const & notFilterTemplateAddr, TemplateArguments const & arguments) const;
};
