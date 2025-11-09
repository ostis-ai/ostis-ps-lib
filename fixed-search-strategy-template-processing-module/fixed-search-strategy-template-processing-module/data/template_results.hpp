#pragma once

#include <optional>

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/utils/sc_logger.hpp>
#include <sc-memory/sc_template.hpp>

#include "template_arguments.hpp"

class TemplateResults;
class ScAgentContext;

/*!
 * @class TemplateResult
 * @brief Represents a single result item from template execution containing variable bindings.
 *
 * The TemplateResult class encapsulates a single result from template execution, storing
 * mappings between entity class addresses and their corresponding arc-element pairs.
 * It provides functionality for accessing, modifying, and transferring result data
 * between different components of the template system.
 *
 * @par Key Features:
 * - Storage of variable bindings as address-to-arc-element mappings
 * - Optional retrieval of elements by entity class address
 * - Result aggregation from multiple template results
 * - Argument updating for chaining template operations
 * - Integration with TemplateResults collection container
 *
 * @note This class is designed for use within the template processing pipeline.
 * @see TemplateResults
 * @see TemplateArguments
 * @see ScAgentContext
 */
class TemplateResult
{
  friend class TemplateArguments;
  friend class TemplateResults;

public:
  /*!
   * @brief Default constructor creating an invalid template result.
   *
   * Creates a TemplateResult instance that is not associated with any context
   * or parent container. Use IsValid() to check the validity of the result.
   */
  TemplateResult();

  /*!
   * @brief Constructs a template result with required dependencies.
   *
   * @param[in] context Pointer to the message reply context for knowledge base operations
   * @param[in] logger Pointer to the system logger for debugging and error reporting
   * @param[in] results Pointer to the parent TemplateResults container
   * @param[in] index Index of this result in the results container
   * @param[in] result Initial variable bindings as address-to-arc-element mappings
   * @param[in] m_continuousResultIndicies Set of indices of continuous results for chaining operations
   * @param[in] connectedResultIndicies Set of indices of connected results for chaining operations
   */
  TemplateResult(
      ScAgentContext * context,
      utils::ScLogger * logger,
      TemplateResults * results,
      size_t index,
      ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> const & result = {},
      std::list<size_t> const & connectedResultIndicies = {});

  /*!
   * @brief Checks if this template result is valid and properly initialized.
   *
   * @return true if the result has valid context and can be used; false otherwise
   */
  bool IsValid() const;

  /*!
   * @brief Gets the index of this result within the parent container.
   *
   * @return The zero-based index of this result in the TemplateResults container
   */
  size_t GetIndex() const;

  /*!
   * @brief Gets the number of connected results associated with this result.
   *
   * @return The count of connected result indices
   */
  size_t Size() const;

  /*!
   * @brief Applies a function to each connected result (const version).
   *
   * @param[in] callback Function to apply to each connected TemplateResult
   */
  void ForEach(std::function<void(TemplateResult const &)> const & callback) const;

  /*!
   * @brief Tests if all connected results satisfy a predicate.
   *
   * @param[in] callback Predicate function to test each connected result
   * @return true if all connected results satisfy the predicate; true if no connected results
   */
  bool AllOf(std::function<bool(TemplateResult const &)> const & callback) const;

  /*!
   * @brief Tests if any connected result satisfies a predicate.
   *
   * @param[in] callback Predicate function to test each connected result
   * @return true if at least one connected result satisfies the predicate; false if none
   */
  bool AnyOf(std::function<bool(TemplateResult const &)> const & callback) const;

  /*!
   * @brief Retrieves the element address associated with a given entity class.
   *
   * @param[in] setAddr Address of the entity class set to look up
   * @return Optional containing the element address if found; std::nullopt otherwise
   */
  std::optional<std::pair<ScAddr, ScAddr>> Get(ScAddr const & setAddr) const;

  /*!
   * @brief Adds a variable binding to this result.
   *
   * @param[in] setAddr Address of the entity class set
   * @param[in] arcAddr Address of the connector arc (may be ScAddr::Empty for erased arcs)
   * @param[in] elementAddr Address of the bound element
   */
  void Add(ScAddr const & setAddr, ScAddr const & arcAddr, ScAddr const & elementAddr);

  /*!
   * @brief Merges another template result into this one.
   *
   * All variable bindings from the other result are copied to this result.
   *
   * @param[in] other The template result to merge from
   */
  void Add(TemplateResult const & other);

  /*!
   * @brief Updates template arguments with result parameters from these results.
   *
   * Transfers result parameters (as defined in the parent TemplateResults configuration)
   * from this result to the provided arguments container, enabling template chaining.
   * Only processes the first result if multiple results are available.
   *
   * @param[out] arguments Template arguments container to update with result parameters
   */
  void TryUpdateArguments(TemplateArguments & arguments) const;

private:
  ScAgentContext * m_context = nullptr;   ///< Pointer to the message reply context
  utils::ScLogger * m_logger = nullptr;   ///< Pointer to the system logger
  TemplateResults * m_results = nullptr;  ///< Pointer to the parent results container
  size_t m_index;                         ///< Index of this result in the results container
  bool m_isResults = false;               ///< Flag indicating if this is a results container
  /// Variable bindings stored as set-to-arc-element pairs
  ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> m_result;
  /// Indices of connected results for chaining operations
  std::list<size_t> m_connectedResultIndicies;

  /*!
   * @brief Adds a connected result index to this result's connection set.
   *
   * @param[in] index Index of the result to connect
   */
  void AddConnectedResultIndex(size_t index);
};

/*!
 * @class TemplateResults
 * @brief Container for managing collections of template execution results.
 *
 * The TemplateResults class manages collections of TemplateResult instances produced
 * by template execution operations. It provides comprehensive functionality for result
 * collection from both search and generation operations, with support for result sorting,
 * filtering, and parameter management.
 *
 * @par Key Features:
 * - Collection from ScTemplateSearchResult and ScTemplateGenResult
 * - Configurable result sorting based on entity text content
 * - Filter callback application for result refinement
 * - Parameter erasure support for knowledge base cleanup
 * - Result parameter extraction for template chaining
 * - Integration with the broader template processing pipeline
 *
 * @par Collection Modes:
 * The class supports two primary collection modes:
 * - Search result collection: Processes multiple results with sorting and filtering
 * - Generation result collection: Processes single generation outcomes
 *
 * @note This class is designed for use within the template processing pipeline.
 * @see TemplateResult
 * @see TemplateArguments
 * @see ScAgentContext
 */
class TemplateResults
{
  friend class TemplateResult;
  friend class SearchTemplate;
  friend class FixedStrategySearchTemplate;

public:
  /// @typedef Results
  /// @brief Type alias for the internal results container.
  using Results = std::vector<TemplateResult>;

  /*!
   * @brief Default constructor creating an empty results container.
   *
   * Creates an invalid TemplateResults instance. Use IsValid() to check
   * if the container is properly initialized before use.
   */
  TemplateResults();

  /*!
   * @brief Constructs a template results container with configuration parameters.
   *
   * @param[in] context Reference to the message reply context for knowledge base operations
   * @param[in] logger Reference to the system logger for debugging and error reporting
   * @param[in] templateAddr Address of the template node for parameter extraction
   * @param[in] sortParamAddr Address of the sorting criteria configuration (optional)
   * @param[in] eraseParamsAddr Address of parameters to erase after processing (optional)
   * @param[in] resultParamsAddr Address of result parameters configuration (optional)
   */
  TemplateResults(
      ScAgentContext & context,
      utils::ScLogger & logger,
      ScAddr const & templateAddr,
      ScAddr const & sortParamAddr,
      ScAddr const & eraseParamsAddr,
      ScAddr const & resultParamsAddr);

  /*!
   * @brief Checks if this results container is valid and properly initialized.
   *
   * @return true if the container has valid context and can be used; false otherwise
   */
  bool IsValid() const;

  /*!
   * @brief Returns the number of results in the collection.
   *
   * @return Size of the results collection, considering start result boundaries
   */
  size_t Size() const;

  /*!
   * @brief Applies a function to each result in the collection (const version).
   *
   * @param[in] callback Function to apply to each TemplateResult
   */
  void ForEach(std::function<void(TemplateResult const &)> const & callback) const;

  /*!
   * @brief Tests if all results in the collection satisfy a predicate.
   *
   * @param[in] callback Predicate function to test each result
   * @return true if all results satisfy the predicate; true if collection is empty
   */
  bool AllOf(std::function<bool(TemplateResult const &)> const & callback) const;

  /*!
   * @brief Tests if any result in the collection satisfies a predicate.
   *
   * @param[in] callback Predicate function to test each result
   * @return true if at least one result satisfies the predicate; false if none
   */
  bool AnyOf(std::function<bool(TemplateResult const &)> const & callback) const;

  /*!
   * @brief Retrieves the element address for a given entity class from any result.
   *
   * @param[in] setAddr Address of the entity class set to look up
   * @return Optional containing the first found element address; std::nullopt if not found
   */
  std::optional<std::pair<ScAddr, ScAddr>> Get(ScAddr const & setAddr) const;

  /*!
   * @brief Collects results from a template search operation.
   *
   * Processes search results by extracting variable bindings, applying sorting
   * criteria, handling parameter erasure, and filtering results through callbacks.
   * The collection process includes comprehensive logging and error handling.
   *
   * @param[in] searchResult Search results from template execution
   * @param[in] callbacks List of filter callback functions for result validation
   * @return true if result collection was successful; false otherwise
   *
   * @par Process Steps:
   * 1. Sort results based on configured criteria
   * 2. Extract erase and template parameters
   * 3. Build result collection with parameter processing
   * 4. Apply filter callbacks for refinement
   */
  bool CollectFromSearchResult(
      ScTemplateSearchResult const & searchResult,
      std::list<FilterCallback> const & callbacks = {});

  /*!
   * @brief Collects results from a template generation operation.
   *
   * Processes generation results by extracting variable bindings from a single
   * generation outcome and organizing them according to template configuration.
   *
   * @param[in] genResult Generation results from template execution
   * @return true if result collection was successful; false otherwise
   */
  bool CollectFromGenResult(ScTemplateGenResult const & genResult);

  /*!
   * @brief Updates template arguments with result parameters from these results.
   *
   * Transfers result parameters (as defined in the parent TemplateResults configuration)
   * from this result to the provided arguments container, enabling template chaining.
   * Only processes the first result if multiple results are available.
   *
   * @param[out] arguments Template arguments container to update with result parameters
   */
  void TryUpdateArguments(TemplateArguments & arguments) const;

  /*!
   * @brief Adds results from another container to this collection.
   *
   * All results from the other container are appended to this container,
   * with indices properly adjusted for the new positions.
   *
   * @param[in] other The template results container to copy from
   */
  void AddTemplateResults(TemplateResults const & other);

  /*!
   * @brief Connects results from another container to a specific result in this collection.
   *
   * Establishes connections between a result in this container and all results
   * from another container, enabling result chaining and hierarchical relationships.
   *
   * @param[in] result The result in this container to connect to
   * @param[in] otherResults The results container to connect from
   * @param[in] isResults Flag indicating if the result is a results (default: true)
   */
  void ConnectTemplateResults(
      TemplateResult const & result,
      TemplateResults const & otherResults,
      bool isResults = true);

  void MergeTemplateResults(TemplateResult const & result, TemplateResults const & otherResults, bool isResults = true);

  void IterateAll(std::function<void(ScAddr const & addr)> const & callback) const;

protected:
  ScAgentContext * m_context = nullptr;  ///< Pointer to the message reply context
  utils::ScLogger * m_logger = nullptr;  ///< Pointer to the system logger
  ScAddr m_templateAddr;                 ///< Address of the template for parameter extraction
  ScAddr m_sortParamAddr;                ///< Address of sorting criteria configuration
  ScAddr m_eraseParamsAddr;              ///< Address of parameters to erase after processing
  ScAddr m_resultParamsAddr;             ///< Address of result parameters configuration
  Results m_results;                     ///< Collection of template results
  /// Index of the last start result item in m_results (-1 if none)
  int m_lastStartResultIndex = -1;
  bool m_isSetTemplateResults;  ///< Flag indicating if this is a set template result container

  /*!
   * @brief Sorts search result indices based on configured sorting criteria.
   *
   * @param[in] searchResult Search results to analyze for sorting
   * @param[out] sortedResultItemIndices Vector to populate with sorted indices
   */
  void SortResultIndices(ScTemplateSearchResult const & searchResult, std::vector<size_t> & sortedResultItemIndices)
      const;

  /*!
   * @brief Extracts erase parameters configuration from the knowledge base.
   *
   * @param[out] eraseParams Map to populate with erase parameter mappings
   */
  void GetEraseParams(ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> & eraseParams) const;

  /*!
   * @brief Extracts result parameters configuration from the knowledge base.
   *
   * @param[out] resultParams Map to populate with result parameter mappings
   */
  void GetResultParams(ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> & resultParams) const;

  /*!
   * @brief Extracts template variable parameters from the knowledge base.
   *
   * @param[out] templateParams Map to populate with template parameter mappings
   */
  void GetTemplateParams(ScAddrToValueUnorderedMap<ScAddr> & templateParams) const;

  /*!
   * @brief Builds result collection from sorted search results.
   *
   * @param[in] searchResult Original search results
   * @param[in] sortedResultItemIndices Sorted indices for result processing
   * @param[in] resultVarArcs Template variable arc mappings
   * @param[in] eraseParams Parameters to erase during processing
   */
  void BuildResults(
      ScTemplateSearchResult const & searchResult,
      std::vector<size_t> const & sortedResultItemIndices,
      ScAddrToValueUnorderedMap<ScAddr> const & resultVarArcs,
      ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> const & eraseParams);

  /*!
   * @brief Applies filter callbacks to refine the result collection.
   *
   * @param[in] callbacks List of filter callback functions to apply
   */
  void ApplyFilters(std::list<FilterCallback> const & callbacks);

  /*!
   * @brief Processes a single result item from template execution.
   *
   * @param[in] item Single result item from template execution
   * @param[in] index Index in the results collection to populate
   * @param[in] templateParams Template variable parameter mappings
   * @param[in] eraseParams Parameters to erase during processing
   */
  void ProcessSingleResultItem(
      ScTemplateResultItem const & item,
      size_t index,
      ScAddrToValueUnorderedMap<ScAddr> const & templateParams,
      ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> const & eraseParams);
};
