#include "template_results.hpp"

#include <variant>

#include <sc-memory/sc_oriented_set.hpp>
#include <sc-memory/sc_agent_context.hpp>

TemplateResult::TemplateResult() = default;

TemplateResult::TemplateResult(
    ScAgentContext * context,
    utils::ScLogger * logger,
    TemplateResults * results,
    size_t index,
    ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> const & result,
    std::list<size_t> const & connectedResultIndicies)
  : m_context(context)
  , m_logger(logger)
  , m_results(results)
  , m_index(index)
  , m_result(result)
  , m_connectedResultIndicies(connectedResultIndicies)
{
}

bool TemplateResult::IsValid() const
{
  return m_context != null_ptr;
}

size_t TemplateResult::GetIndex() const
{
  return m_index;
}

size_t TemplateResult::Size() const
{
  return m_connectedResultIndicies.size();
}

void TemplateResult::ForEach(std::function<void(TemplateResult const &)> const & callback) const
{
  std::function<void(TemplateResult const &)> ForEachItem = [&](TemplateResult const & result)
  {
    if (result.m_isResults)
    {
      result.ForEach(
          [&](TemplateResult const & childResult)
          {
            TemplateResult resultCopy = std::move(childResult);
            resultCopy.m_result.insert(result.m_result.cbegin(), result.m_result.cend());

            if (resultCopy.m_isResults)
              ForEachItem(resultCopy);
            else
              callback(resultCopy);
          });
    }
    else
      callback(result);
  };

  for (size_t index : m_connectedResultIndicies)
  {
    auto const & result = m_results->m_results[index];
    ForEachItem(result);
  }
}

bool TemplateResult::AllOf(std::function<bool(TemplateResult const &)> const & callback) const
{
  std::function<bool(TemplateResult const &)> AllOfItems = [&](TemplateResult const & result) -> bool
  {
    if (result.m_isResults)
    {
      return result.AllOf(
          [&](TemplateResult const & childResult) -> bool
          {
            TemplateResult resultCopy = std::move(childResult);
            resultCopy.m_result.insert(result.m_result.cbegin(), result.m_result.cend());

            if (resultCopy.m_isResults)
              return AllOfItems(resultCopy);
            else
              return callback(resultCopy);
          });
    }
    else
      return callback(result);
  };

  for (size_t index : m_connectedResultIndicies)
  {
    auto const & result = m_results->m_results[index];
    if (!AllOfItems(result))
      return false;
  }

  return true;
}

bool TemplateResult::AnyOf(std::function<bool(TemplateResult const &)> const & callback) const
{
  std::function<bool(TemplateResult const &)> AnyOfItems = [&](TemplateResult const & result) -> bool
  {
    if (result.m_isResults)
    {
      return result.AnyOf(
          [&](TemplateResult const & childResult) -> bool
          {
            TemplateResult resultCopy = std::move(childResult);
            resultCopy.m_result.insert(result.m_result.cbegin(), result.m_result.cend());

            if (resultCopy.m_isResults)
              return AnyOfItems(resultCopy);
            else
              return callback(resultCopy);
          });
    }
    else
      return callback(result);
  };

  for (size_t index : m_connectedResultIndicies)
  {
    auto const & result = m_results->m_results[index];
    if (AnyOfItems(result))
      return true;
  }

  return false;
}

std::optional<std::pair<ScAddr, ScAddr>> TemplateResult::Get(ScAddr const & setAddr) const
{
  auto const & GetValue = [&](TemplateResult const & result) -> std::optional<std::pair<ScAddr, ScAddr>>
  {
    auto const & it = m_result.find(setAddr);
    return it == m_result.cend() ? std::nullopt
                                 : std::optional<std::pair<ScAddr, ScAddr>>({it->second.first, it->second.second});
  };

  std::optional<std::pair<ScAddr, ScAddr>> resultOpt = GetValue(*this);
  if (resultOpt)
    return resultOpt;

  std::function<void(TemplateResult const &)> SearchValue = [&](TemplateResult const & result) -> void
  {
    result.AnyOf(
        [&](TemplateResult const & nextResult) -> bool
        {
          if (auto const it = nextResult.Get(setAddr))
          {
            resultOpt = it;
            return true;
          }

          SearchValue(nextResult);
          return false;
        });
  };

  SearchValue(*this);
  return resultOpt;
}

void TemplateResult::Add(ScAddr const & setAddr, ScAddr const & arcAddr, ScAddr const & elementAddr)
{
  m_result.insert({setAddr, {arcAddr, elementAddr}});
}

void TemplateResult::Add(TemplateResult const & other)
{
  m_result.insert(other.m_result.cbegin(), other.m_result.cend());
}

void TemplateResult::TryUpdateArguments(TemplateArguments & arguments) const
{
  ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> resultParams;
  m_results->GetResultParams(resultParams);

  for (auto const & [setAddr, _] : resultParams)
  {
    if (auto const it = Get(setAddr))
    {
      m_logger->Debug("Result found for set ", setAddr, ": arc ", it->first, " element ", it->second);
      arguments.Add(setAddr, it->first, it->second);
    }
    else
      m_logger->Debug("Result not found for set ", setAddr);
  }
}

void TemplateResult::AddConnectedResultIndex(size_t index)
{
  m_connectedResultIndicies.push_back(index);
}

TemplateResults::TemplateResults() = default;

TemplateResults::TemplateResults(
    ScAgentContext & context,
    utils::ScLogger & logger,
    ScAddr const & templateAddr,
    ScAddr const & sortParamAddr,
    ScAddr const & eraseParamsAddr,
    ScAddr const & resultParamsAddr)
  : m_context(&context)
  , m_logger(&logger)
  , m_templateAddr(templateAddr)
  , m_sortParamAddr(sortParamAddr)
  , m_eraseParamsAddr(eraseParamsAddr)
  , m_resultParamsAddr(resultParamsAddr)
{
}

bool TemplateResults::IsValid() const
{
  return m_context != nullptr;
}

size_t TemplateResults::Size() const
{
  return m_results.size() > 0 ? (m_lastStartResultIndex == -1 ? m_results.size() : m_lastStartResultIndex + 1) : 0;
}

void TemplateResults::ForEach(std::function<void(TemplateResult const &)> const & callback) const
{
  std::function<void(TemplateResult const &)> ForEachItem = [&](TemplateResult const & result)
  {
    if (result.m_isResults)
    {
      result.ForEach(
          [&](TemplateResult const & childResult)
          {
            TemplateResult resultCopy = std::move(childResult);
            resultCopy.m_result.insert(result.m_result.cbegin(), result.m_result.cend());

            if (resultCopy.m_isResults)
              ForEachItem(resultCopy);
            else
              callback(resultCopy);
          });
    }
    else
      callback(result);
  };

  for (size_t i = 0; i < Size(); ++i)
  {
    auto const & result = m_results[i];
    ForEachItem(result);
  }
}

bool TemplateResults::AllOf(std::function<bool(TemplateResult const &)> const & callback) const
{
  std::function<bool(TemplateResult const &)> AllOfItems = [&](TemplateResult const & result) -> bool
  {
    if (result.m_isResults)
    {
      return result.AllOf(
          [&](TemplateResult const & childResult) -> bool
          {
            TemplateResult resultCopy = std::move(childResult);
            resultCopy.m_result.insert(result.m_result.cbegin(), result.m_result.cend());

            if (resultCopy.m_isResults)
              return AllOfItems(resultCopy);
            else
              return callback(resultCopy);
          });
    }
    else
      return callback(result);
  };

  for (size_t i = 0; i < Size(); ++i)
  {
    auto const & result = m_results[i];
    if (!AllOfItems(result))
      return false;
  }

  return true;
}

bool TemplateResults::AnyOf(std::function<bool(TemplateResult const &)> const & callback) const
{
  std::function<bool(TemplateResult const &)> AnyOfItems = [&](TemplateResult const & result) -> bool
  {
    if (result.m_isResults)
    {
      return result.AnyOf(
          [&](TemplateResult const & childResult) -> bool
          {
            TemplateResult resultCopy = std::move(childResult);
            resultCopy.m_result.insert(result.m_result.cbegin(), result.m_result.cend());

            if (resultCopy.m_isResults)
              return AnyOfItems(resultCopy);
            else
              return callback(resultCopy);
          });
    }
    else
      return callback(result);
  };

  for (size_t i = 0; i < Size(); ++i)
  {
    auto const & result = m_results[i];
    if (AnyOfItems(result))
      return true;
  }

  return false;
}

std::optional<std::pair<ScAddr, ScAddr>> TemplateResults::Get(ScAddr const & setAddr) const
{
  std::optional<std::pair<ScAddr, ScAddr>> resultOpt = std::nullopt;

  std::function<void(std::variant<TemplateResult, TemplateResults> const &)> SearchValue =
      [&](std::variant<TemplateResult, TemplateResults> const & result) -> void
  {
    std::visit(
        [&](auto && arg)
        {
          arg.AnyOf(
              [&](TemplateResult const & nextResult) -> bool
              {
                if (auto const it = nextResult.Get(setAddr))
                {
                  resultOpt = it;
                  return true;
                }

                SearchValue(nextResult);
                return false;
              });
        },
        result);
  };

  SearchValue(*this);

  return resultOpt;
}

bool TemplateResults::CollectFromSearchResult(
    ScTemplateSearchResult const & searchResult,
    std::list<FilterCallback> const & callbacks)
{
  m_logger->Debug("Collect results for condition template ", m_templateAddr);
  bool status = true;

  std::vector<size_t> sortedResultItemIndices;
  SortResultIndices(searchResult, sortedResultItemIndices);

  ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> eraseParams;
  GetEraseParams(eraseParams);
  ScAddrToValueUnorderedMap<ScAddr> templateParams;
  GetTemplateParams(templateParams);

  BuildResults(searchResult, sortedResultItemIndices, templateParams, eraseParams);

  ApplyFilters(callbacks);

  m_logger->Debug(
      "Results for condition template ",
      m_templateAddr,
      " collected. Count is ",
      m_results.size(),
      ". Status is ",
      status);

  return status;
}

bool TemplateResults::CollectFromGenResult(ScTemplateGenResult const & genResult)
{
  m_logger->Debug("Collect results for condition template ", m_templateAddr);
  bool status = true;

  ScAddrToValueUnorderedMap<ScAddr> templateParams;
  GetTemplateParams(templateParams);

  if (m_results.size() == 0)
    m_results.emplace_back(m_context, m_logger, this, 0);
  ProcessSingleResultItem(genResult, 0, templateParams, {});

  m_logger->Debug(
      "Results for condition template ",
      m_templateAddr,
      " collected. Count is ",
      m_results.size(),
      ". Status is ",
      status);

  return status;
}

void TemplateResults::TryUpdateArguments(TemplateArguments & arguments) const
{
  if (Size() == 0)
    return;

  ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> resultParams;
  GetResultParams(resultParams);

  for (auto const & [setAddr, _] : resultParams)
  {
    if (auto const it = Get(setAddr))
    {
      m_logger->Debug("Result found for set ", setAddr, ": arc ", it->first, " element ", it->second);
      arguments.Add(setAddr, it->first, it->second);
    }
  }
}

void TemplateResults::AddTemplateResults(TemplateResults const & other)
{
  size_t size = m_results.size();
  m_results.reserve(size + other.m_results.size());
  for (auto const & result : other.m_results)
  {
    size_t const newIndex = result.GetIndex() + size;

    std::list<size_t> connectedResultIndicies;
    for (size_t index : result.m_connectedResultIndicies)
      connectedResultIndicies.push_back(index + size);

    m_results.emplace_back(m_context, m_logger, this, newIndex, result.m_result, connectedResultIndicies);
  }
}

void TemplateResults::ConnectTemplateResults(
    TemplateResult const & result,
    TemplateResults const & otherResults,
    bool isResults)
{
  m_results[result.GetIndex()].m_isResults = isResults;
  otherResults.ForEach(
      [&](TemplateResult const & otherResult)
      {
        size_t const newIndex = otherResult.GetIndex() + m_results.size();
        m_results[result.GetIndex()].AddConnectedResultIndex(newIndex);
      });

  if (m_lastStartResultIndex == -1)
    m_lastStartResultIndex = m_results.size() - 1;

  AddTemplateResults(otherResults);
}

void TemplateResults::MergeTemplateResults(
    TemplateResult const & result,
    TemplateResults const & otherResults,
    bool isResults)
{
  auto & thisResult = m_results[result.GetIndex()];
  thisResult.m_isResults = isResults;
  if (otherResults.Size() > 0)
  {
    auto & otherResult = otherResults.m_results[0];
    thisResult.m_result.insert(otherResult.m_result.cbegin(), otherResult.m_result.cend());

    thisResult.m_connectedResultIndicies.insert(
        thisResult.m_connectedResultIndicies.cbegin(),
        otherResult.m_connectedResultIndicies.cbegin(),
        otherResult.m_connectedResultIndicies.cend());
  }
}

void TemplateResults::IterateAll(std::function<void(ScAddr const & addr)> const & callback) const
{
  for (auto const & result : m_results)
  {
    for (auto const & pair : result.m_result)
    {
      callback(pair.first);
      callback(pair.second.first);
      callback(pair.second.second);
    }
  }
}

void TemplateResults::SortResultIndices(
    ScTemplateSearchResult const & searchResult,
    std::vector<size_t> & sortedResultItemIndices) const
{
  static auto const IsNumber = [](std::string const & s) -> bool
  {
    if (s.empty())
      return false;
    size_t i = (s[0] == '-' || s[0] == '+') ? 1 : 0;
    if (i == s.size())
      return false;
    for (; i < s.size(); ++i)
      if (!std::isdigit(s[i]))
        return false;
    return true;
  };

  if (m_sortParamAddr.IsValid())
  {
    ScAddr const varArcAddr = m_sortParamAddr;
    ScAddrToValueUnorderedMap<size_t> entitiesToIndicies;

    ScAddr sortSetAddr;
    bool isSortSetFound = false;
    for (size_t i = 0; i < searchResult.Size(); ++i)
    {
      ScAddr const arcAddr = searchResult[i][varArcAddr];
      auto const [setAddr, entityAddr] = m_context->GetConnectorIncidentElements(arcAddr);

      if (sortSetAddr.IsValid() && sortSetAddr != setAddr)
        isSortSetFound = false;
      else
      {
        sortSetAddr = setAddr;
        isSortSetFound = true;
      }

      entitiesToIndicies.insert({entityAddr, i});
    }

    sortedResultItemIndices.reserve(entitiesToIndicies.size());
    if (isSortSetFound)
    {
      ScOrientedSet const sortSet = m_context->ConvertToOrientedSet(sortSetAddr);

      ScAddr elementAddr;
      while ((elementAddr = sortSet.Next()).IsValid())
      {
        auto const it = entitiesToIndicies.find(elementAddr);
        if (it != entitiesToIndicies.cend())
          sortedResultItemIndices.push_back(it->second);
      }
    }

    if (!isSortSetFound || sortedResultItemIndices.size() < entitiesToIndicies.size())
    {
      sortedResultItemIndices.clear();

      std::vector<std::pair<std::string, size_t>> itemsIndicesWithText;
      itemsIndicesWithText.reserve(entitiesToIndicies.size());
      std::transform(
          entitiesToIndicies.begin(),
          entitiesToIndicies.end(),
          std::back_inserter(itemsIndicesWithText),
          [&](std::pair<ScAddr, size_t> const & pair)
          {
            // if (auto const textIt = m_context->SearchEntityText(pair.first))
            //   return std::make_pair(*textIt, pair.second);
            // else
            return std::make_pair(std::string(), pair.second);
          });

      std::sort(
          itemsIndicesWithText.begin(),
          itemsIndicesWithText.end(),
          [](auto const & a, auto const & b)
          {
            bool aIsNum = IsNumber(a.first);
            bool bIsNum = IsNumber(b.first);
            if (aIsNum && bIsNum)
              return std::stoll(a.first) < std::stoll(b.first);
            else
              return a.first < b.first;
          });

      std::transform(
          itemsIndicesWithText.begin(),
          itemsIndicesWithText.end(),
          std::back_inserter(sortedResultItemIndices),
          [](auto & pair)
          {
            return std::move(pair.second);
          });
    }
  }
  else
  {
    // If no sorting is specified, process items in their original order
    sortedResultItemIndices.reserve(searchResult.Size());
    for (size_t i = 0; i < searchResult.Size(); ++i)
      sortedResultItemIndices.push_back(i);
  }
}

void TemplateResults::GetEraseParams(ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> & eraseParams) const
{
  if (m_eraseParamsAddr.IsValid())
  {
    m_context->ConvertToSet(m_eraseParamsAddr)
        .ForEach(
            [&](ScAddr const &, ScAddr const & paramArcAddr, ScAddr const &, ScAddr const &)
            {
              auto const [setAddr, paramElementAddr] = m_context->GetConnectorIncidentElements(paramArcAddr);
              eraseParams.insert({paramArcAddr, {setAddr, paramElementAddr}});
            });
  }

  m_logger->Debug("Found ", eraseParams.size(), " erase params");
}

void TemplateResults::GetResultParams(ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> & resultParams) const
{
  if (m_resultParamsAddr.IsValid())
  {
    m_context->ConvertToSet(m_resultParamsAddr)
        .ForEach(
            [&](ScAddr const &, ScAddr const & paramArcAddr, ScAddr const &, ScAddr const &)
            {
              auto const [setAddr, paramElementAddr] = m_context->GetConnectorIncidentElements(paramArcAddr);
              resultParams.insert({setAddr, {paramArcAddr, paramElementAddr}});
            });
  }

  m_logger->Debug("Found ", resultParams.size(), " output params");
}

void TemplateResults::GetTemplateParams(ScAddrToValueUnorderedMap<ScAddr> & templateParams) const
{
  ScIterator5Ptr const it5 = m_context->CreateIterator5(
      ScType::Node, ScType::VarMembershipArc, ScType::Unknown, ScType::ConstPermPosArc, m_templateAddr);

  while (it5->Next())
  {
    ScAddr const setAddr = it5->Get(0);
    ScAddr const varArcAddr = it5->Get(1);
    templateParams.insert({setAddr, varArcAddr});
  }
}

void TemplateResults::BuildResults(
    ScTemplateSearchResult const & searchResult,
    std::vector<size_t> const & sortedResultItemIndices,
    ScAddrToValueUnorderedMap<ScAddr> const & resultVarArcs,
    ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> const & eraseParams)
{
  m_results.reserve(searchResult.Size());

  for (size_t i = m_results.size(); i < searchResult.Size(); ++i)
    m_results.emplace_back(m_context, m_logger, this, i);

  for (size_t i = 0; i < sortedResultItemIndices.size(); ++i)
  {
    size_t const index = sortedResultItemIndices[i];
    auto const & item = searchResult[index];
    ProcessSingleResultItem(item, i, resultVarArcs, eraseParams);
  }
}

void TemplateResults::ProcessSingleResultItem(
    ScTemplateResultItem const & item,
    size_t index,
    ScAddrToValueUnorderedMap<ScAddr> const & templateParams,
    ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> const & eraseParams)
{
  for (auto const & [setAddr, varArcAddr] : templateParams)
  {
    std::string const setElementSystemIdentifier = m_context->GetElementSystemIdentifier(setAddr);

    if (item.Has(varArcAddr))
    {
      ScAddr const arcAddr = item[varArcAddr];
      ScAddr const elementAddr = m_context->GetArcTargetElement(arcAddr);
      auto const it = eraseParams.find(varArcAddr);

      if (it == eraseParams.cend())
      {
        m_results[index].Add(setAddr, arcAddr, elementAddr);
        m_logger->Debug(
            "Triple: arc ",
            arcAddr,
            " from ",
            setElementSystemIdentifier,
            " to ",
            elementAddr,
            " added to search results");
      }
      else
      {
        m_context->EraseElement(arcAddr);
        m_results[index].Add(setAddr, ScAddr::Empty, elementAddr);
        m_logger->Debug(
            "Arc ", arcAddr, " from ", setElementSystemIdentifier, " to ", elementAddr, " erased from search result");
        m_logger->Debug(
            "Tuple: set ", setElementSystemIdentifier, " with element ", elementAddr, " added to search results");
      }
    }
    else
      m_logger->Debug("Arc variable ", varArcAddr, " not found in search result");
  }
}

void TemplateResults::ApplyFilters(std::list<FilterCallback> const & callbacks)
{
  if (callbacks.empty())
    return;

  m_logger->Debug("Filter results by filter callbacks");

  Results filteredResults;
  filteredResults.reserve(m_results.size());

  size_t i = 1;
  ForEach(
      [&](TemplateResult const & result)
      {
        bool const filterResult = std::all_of(
            callbacks.cbegin(),
            callbacks.cend(),
            [&](auto const & callback) -> bool
            {
              TemplateArguments arguments{*m_context, *m_logger};
              result.TryUpdateArguments(arguments);
              return callback ? callback(arguments) : true;
            });

        if (filterResult)
        {
          m_logger->Debug("Result item ", i, " passed filters");
          filteredResults.emplace_back(std::move(result));
        }
        else
          m_logger->Debug("Result item ", i, " failed filters");

        ++i;
      });

  m_results = std::move(filteredResults);
}
