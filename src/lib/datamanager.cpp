#include "datamanager.h"

#include <Jsoner/object.h>
#include <Jsoner/array.h>

#include <DataGate/datarequest.h>
#include <DataGate/dataresponse.h>

#include <RestLink/request.h>
#include <RestLink/body.h>
#include <RestLink/response.h>

using namespace DataGate;
using namespace RestLink;

namespace RestGate {

DataManager::DataManager(RestLink::Api *api)
    : api(api)
{
}

DataManager::~DataManager()
{
}

void DataManager::run(const DataGate::DataRequest &request, const DataProcessor &processor)
{
    if (!processor.isValid()) {
        if (processor.responseCallback)
            processor.responseCallback(DataResponse());
        return;
    }

    Request restRequest = processor.requestConverter(request);
    if (!processor.endpoint.isEmpty())
        restRequest.setEndpoint(processor.endpoint);

    Body restBody = (processor.bodyGenerator ? processor.bodyGenerator(request) : Body());
    Response *restResponse = api->send(processor.method, restRequest, restBody);

    if (!restResponse) {
        processor.responseCallback(DataResponse());
        return;
    }

    if (processor.progressCallback) {
        QObject::connect(restResponse, &Response::downloadProgress, restResponse, [processor](qint64 progress, qint64 total) {
            processor.progressCallback(progress, total);
        });
    }

    QObject::connect(restResponse, &Response::finished, restResponse, [processor, restResponse] {
        DataResponse response;
        if (processor.responseConverter)
            response = processor.responseConverter(restResponse);
        else if (restResponse->header("Content-Type") == "application/json") {
            const QJsonValue value = restResponse->readJson();
            if (value.isObject())
                response.setObject(value.toObject());
            else
                response.setArray(value.toArray());
        }

        response.setCode(restResponse->httpStatusCode());
        response.setSuccess(restResponse->isSuccess());
        processor.responseCallback(response);
    });
}

} // namespace RestGate
