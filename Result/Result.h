#pragma once

#include <QString>
#include <deque>

struct Result
{
    enum ResultType
    {
        RESULT_SUCCESS,
        RESULT_WARNING,
        RESULT_ERROR
    };
    Result(const ResultType type = RESULT_SUCCESS, const QString& message = QString());
    Result(const Result& result) = default;
    Result(Result&& result);
    Result& operator=(const Result& result) = default;
    Result& operator=(Result&& result);
    operator bool() const;
    ResultType type = RESULT_SUCCESS;
    QString message;
};

inline Result::operator bool() const
{
    return type == RESULT_SUCCESS || type == RESULT_WARNING;
}

class ResultList
{
public:
    explicit ResultList();
    explicit ResultList(const Result& result);
    ResultList(Result&& result);
    ResultList(const ResultList& resultList) = default;
    ResultList(ResultList&& resultList);
    ~ResultList() = default;
    operator bool() const;
    bool IsSuccess() const;
    ResultList& operator=(const ResultList& resultList) = default;
    ResultList& operator=(ResultList&& resultList);
    ResultList& operator<<(const Result& result);
    ResultList& operator<<(Result&& result);
    ResultList& AddResult(const Result& result);
    ResultList& AddResult(Result&& result);
    ResultList& AddResult(const Result::ResultType type = Result::RESULT_SUCCESS, const QString& message = QString());
    ResultList& AddResultList(const ResultList& resultList);
    ResultList& AddResultList(ResultList&& resultList);

    const std::deque<Result>& GetResults() const;

private:
    bool allOk;
    std::deque<Result> results;
};

inline ResultList::operator bool() const
{
    return allOk;
}

inline bool ResultList::IsSuccess() const
{
    return allOk;
}

inline const std::deque<Result>& ResultList::GetResults() const
{
    return results;
}
