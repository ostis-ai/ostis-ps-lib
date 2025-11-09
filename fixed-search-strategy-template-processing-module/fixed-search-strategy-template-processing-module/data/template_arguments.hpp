#pragma once

/*!
 * @file template_arguments.hpp
 * @brief Container for template parameter bindings and argument management.
 *
 * This module provides the TemplateArguments class, which manages variable bindings
 * and parameter mappings for parameterized template execution against the sc-memory
 * knowledge base. It serves as the primary mechanism for passing runtime arguments
 * to templates, mapping entity class addresses to specific element instances.
 *
 * TemplateArguments acts as a flexible argument container that can:
 * - Collect arguments from sc-memory sets
 * - Merge arguments from multiple sources (other TemplateArguments, TemplateResults)
 * - Convert arguments to ScTemplateParams for template instantiation
 * - Validate that required template parameters are satisfied by available arguments
 *
 * @see template_results.hpp
 * @see TemplateResult
 * @see ScTemplateParams
 * @see ScAgentContext
 */

#include <optional>

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/utils/sc_logger.hpp>
#include <sc-memory/sc_set.hpp>

class TemplateResult;
class ScAgentContext;

/*!
 * @typedef ArgumentType
 * @brief Mapping type for argument storage: entity class address to (arc, element) pair.
 *
 * Represents the internal storage format for template arguments, mapping each entity
 * class or parameter set address to a pair containing:
 * - The arc address connecting the set to the element (membership arc)
 * - The element address that is a member of the set
 */
using ArgumentType = ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>>;

/*!
 * @class TemplateArguments
 * @brief Container managing parameter bindings and variable substitutions for template execution.
 *
 * TemplateArguments provides a flexible container for managing runtime arguments passed
 * to parameterized templates. It maintains mappings from entity classes (parameter sets)
 * to specific element instances, supporting argument collection from various sources
 * and conversion to template parameter formats.
 *
 * @see TemplateResult
 * @see ScTemplateParams
 * @see FilterCallback
 *
 * @thread_safety Not thread-safe; external synchronization required for concurrent access.
 */
class TemplateArguments
{
public:
  /*!
   * @typedef Arguments
   * @brief Vector of argument mappings for batch processing or iteration.
   *
   * Defines a collection type for multiple argument sets, used when processing
   * multiple argument configurations or iterating over argument batches.
   */
  using Arguments = std::vector<ArgumentType>;

  /*!
   * @brief Default constructor creating a standalone arguments container.
   *
   * Constructs a TemplateArguments instance without agent context or logger references.
   * This mode is suitable for simple argument passing or intermediate storage where
   * knowledge base access and logging are not required.
   *
   * In this mode, methods requiring context (CollectFromSet, GetTemplateParams) cannot
   * be safely used as they rely on valid context and logger pointers.
   *
   * @see TemplateArguments(ScAgentContext&, utils::ScLogger&)
   */
  TemplateArguments();

  /*!
   * @brief Constructor creating a context-aware arguments container.
   *
   * Constructs a TemplateArguments instance with references to agent context and logger,
   * enabling full functionality including argument collection from sc-memory sets and
   * parameter validation with detailed logging.
   *
   * This is the preferred constructor for most use cases where arguments will be
   * collected from or validated against the knowledge base.
   *
   * @param context   [in] Reference to the ScAgentContext providing knowledge base
   *                       access for argument collection and parameter resolution.
   *                       The context pointer is stored and must remain valid for
   *                       the lifetime of this TemplateArguments instance.
   *
   * @param logger    [in] Reference to the ScLogger instance for logging argument
   *                       operations, parameter validation results, and debugging
   *                       information. The logger pointer is stored and must remain
   *                       valid for the lifetime of this instance.
   *
   * @note The context and logger are stored as pointers; ensure they remain valid
   *       throughout the lifetime of this TemplateArguments object.
   */
  TemplateArguments(ScAgentContext & context, utils::ScLogger & logger);

  /*!
   * @brief Retrieves the element address bound to a specific entity class.
   *
   * Queries the argument bindings to find the element address associated with the
   * specified entity class (set) address. If the set address has an argument binding,
   * returns the bound element address; otherwise, returns std::nullopt.
   *
   * This method is useful for checking whether a specific parameter has been bound
   * and retrieving its value without requiring exception handling.
   *
   * @param setAddr   [in] Constant reference to the entity class (set) address whose
   *                       bound element is to be retrieved.
   *
   * @return std::optional<ScAddr> containing the bound element address if the set
   *         address has an argument binding; std::nullopt if no binding exists.
   *
   * @note Only the element address is returned, not the membership arc address.
   *       If the arc is needed, access m_arguments directly.
   */
  std::optional<ScAddr> Get(ScAddr const & setAddr) const;

  /*!
   * @brief Collects arguments from an sc-memory set by reading membership relations.
   *
   * Populates the argument bindings by iterating over all membership arcs in the
   * provided sc-memory set. For each arc connecting the set to an element, adds
   * an argument binding mapping the set address to the (arc, element) pair.
   *
   * This method is typically used to collect input parameters from parameter sets
   * defined in the knowledge base, where set membership represents parameter bindings.
   *
   * @param set   [in] Constant reference to the ScSet from which to collect argument
   *                   bindings. The set is queried for all membership arcs, and each
   *                   arc's incident elements are used to construct argument mappings.
   *
   * @pre This method requires that the TemplateArguments instance was created with
   *      the context-aware constructor, as it uses m_context for element queries.
   *
   * @warning Calling this method on a TemplateArguments created with the default
   *          constructor (m_context == nullptr) will cause undefined behavior.
   *
   * @see ScSet::GetElements
   */
  void CollectFromSet(ScSet const & set);

  /*!
   * @brief Merges argument bindings from another TemplateArguments instance.
   *
   * Inserts all argument bindings from the provided TemplateArguments into this
   * instance's argument collection. If a set address already exists in this instance,
   * the behavior depends on the underlying map semantics (typically, existing bindings
   * are preserved and new bindings are ignored for duplicate keys).
   *
   * This method is commonly used to combine arguments from multiple sources, such as
   * when propagating arguments through multi-stage template execution.
   *
   * @param arguments   [in] Constant reference to another TemplateArguments instance
   *                         whose argument bindings should be merged into this instance.
   *
   * @see Add(TemplateResult const&)
   */
  void Add(TemplateArguments const & arguments);

  /*!
   * @brief Adds a single argument binding for a set-element pair.
   *
   * Inserts a single argument binding mapping the specified entity class (set) address
   * to the provided (arc, element) pair. This method is useful for programmatically
   * constructing argument bindings when the arc and element addresses are known.
   *
   * @param setAddr       [in] Constant reference to the entity class (set) address
   *                           that serves as the parameter key.
   *
   * @param arcAddr       [in] Constant reference to the membership arc address
   *                           connecting the set to the element.
   *
   * @param elementAddr   [in] Constant reference to the element address that is
   *                           bound as the parameter value.
   *
   * @note If setAddr already has a binding, the behavior depends on map semantics
   *       (typically existing bindings are preserved).
   */
  void Add(ScAddr const & setAddr, ScAddr const & arcAddr, ScAddr const & elementAddr);

  /*!
   * @brief Adds argument bindings from a TemplateResult.
   *
   * Inserts all argument bindings stored in the provided TemplateResult into this
   * TemplateArguments instance. This method enables result propagation from one
   * template execution to serve as arguments for subsequent template executions,
   * which is essential for multi-stage template processing.
   *
   * TemplateResult stores its bindings in the same format as TemplateArguments
   * (m_result), so the insertion is straightforward.
   *
   * @param result   [in] Constant reference to a TemplateResult whose argument
   *                      bindings should be added to this TemplateArguments instance.
   *
   * @see TemplateResult
   * @see FixedStrategySearchTemplate::ProcessNextTemplates
   */
  void Add(TemplateResult const & result);

  /*!
   * @brief Converts argument bindings to ScTemplateParams for template instantiation.
   *
   * This method performs the crucial task of converting high-level argument bindings
   * (entity class to element mappings) into low-level ScTemplateParams that can be
   * used for template variable substitution. It validates that all required input
   * parameters (defined in inputParamsAddr) are satisfied by the available arguments.
   *
   * @param templateAddr      [in] Constant reference to the template address being
   *                               instantiated. Used for logging and error reporting.
   *
   * @param inputParamsAddr   [in] Constant reference to the address of the input
   *                               parameters set in the knowledge base. This set
   *                               defines which parameters the template requires.
   *                               If invalid, no parameters are required (returns true).
   *
   * @param params            [out] Reference to the ScTemplateParams object to populate
   *                                with variable substitutions. For each matched argument,
   *                                both the arc and element are added as substitutions.
   *
   * @return @c true if all required input parameters were satisfied by the available
   *         arguments; @c false if any required parameters are missing (logged as warnings).
   *
   * @pre This method requires that the TemplateArguments instance was created with
   *      the context-aware constructor, as it uses m_context and m_logger.
   *
   * @warning Calling this method on a TemplateArguments created with the default
   *          constructor will cause undefined behavior.
   *
   * @see ScTemplateParams
   * @see ParameterizedTemplate::m_inputParamsAddr
   */
  bool GetTemplateParams(ScAddr const & templateAddr, ScAddr const & inputParamsAddr, ScTemplateParams & params) const;

private:
  /// Pointer to the message reply context for knowledge base access.
  /// Null if created with default constructor; valid if created with context-aware constructor.
  ScAgentContext * m_context = nullptr;

  /// Pointer to the system logger for debugging and error reporting.
  /// Null if created with default constructor; valid if created with context-aware constructor.
  utils::ScLogger * m_logger = nullptr;

  /*!
   * @brief Collection of argument mappings from entity classes to arc-element pairs.
   *
   * Stores the core argument bindings in a hash map structure:
   * - Key: Entity class (set) address representing a parameter
   * - Value: Pair of (membership arc address, element address)
   *
   * This structure preserves the full sc-memory membership relationship, allowing
   * templates to reference both the element and its membership arc when needed.
   */
  ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> m_arguments;
};

/*!
 * @typedef FilterCallback
 * @brief Function type for filter predicates accepting template arguments.
 *
 * Defines the signature for filter callback functions used in template filtering
 * pipelines. Filter callbacks accept a TemplateArguments instance (by value, allowing
 * the callback to inspect argument bindings) and return a boolean indicating whether
 * the arguments satisfy the filter constraint.
 *
 * @see TemplateArguments
 * @see ParameterizedTemplate::ApplyImpl
 */
using FilterCallback = std::function<bool(TemplateArguments)>;