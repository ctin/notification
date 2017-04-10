#include "Result/Result.h"

Result::Result(ResultType type_, const QString& message_)
    : type(type_)
    , message(message_)
{
}

Result::Result(Result&& result)
    : type(result.type)
    , message(std::move(result.message))
{
    result.type = RESULT_SUCCESS;
}

Result& Result::operator=(Result&& result)
{
    if (this != &result)
    {
        type = result.type;
        message = std::move(result.message);
        result.type = RESULT_SUCCESS;
    }
    return *this;
}

ResultList::ResultList()
    : allOk(true)
{
}

ResultList::ResultList(const Result& result)
    : allOk(result)
{
    results.push_back(result);
}

ResultList::ResultList(Result&& result)
{
    AddResult(std::move(result));
}

ResultList::ResultList(ResultList&& resultList)
    : allOk(resultList.allOk)
    , results(std::move(resultList.results))
{
}

ResultList& ResultList::operator=(ResultList&& resultList)
{
    if (this != &resultList)
    {
        allOk = resultList.allOk;
        results = std::move(resultList.results);
    }
    return *this;
}

ResultList& ResultList::operator<<(const Result& result)
{
    return AddResult(std::move(result));
}

ResultList& ResultList::operator<<(Result&& result)
{
    return AddResult(std::move(result));
}

ResultList& ResultList::AddResult(const Result& result)
{
    allOk &= result;
    results.push_back(result);
    return *this;
}

ResultList& ResultList::AddResult(Result&& result)
{
    allOk &= result;
    results.emplace_back(std::move(result));
    return *this;
}

ResultList& ResultList::AddResult(const Result::ResultType type, const QString& message)
{
    return AddResult(Result(type, message));
}

ResultList& ResultList::AddResultList(const ResultList& resultList)
{
    allOk &= resultList.allOk;
    results.insert(results.end(), resultList.results.begin(), resultList.results.end());
    return *this;
}

ResultList& ResultList::AddResultList(ResultList&& resultList)
{
    Q_ASSERT(this != &resultList);
    allOk &= resultList.allOk;
    if (results.empty())
    {
        results = std::move(resultList.results);
    }
    else
    {
        std::move(std::begin(resultList.results), std::end(resultList.results), std::back_inserter(results));
        resultList.results.clear();
    }
    return *this;
}
