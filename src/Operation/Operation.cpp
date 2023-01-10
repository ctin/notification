#include "Operation/Operation.h"
#include <QDirIterator>
#include <QApplication>
#include <QDebug>

Result Operation::DoSomething(Result::ResultType resultType)
{
    static QMap<Result::ResultType, QString> typeStrings = {
        {Result::RESULT_SUCCESS, "Success"},
        {Result::RESULT_WARNING, "Warning"},
        {Result::RESULT_ERROR, "Error"}
    };

    return Result(resultType, typeStrings[resultType]);
}
