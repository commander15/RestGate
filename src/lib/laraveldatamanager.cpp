#include "laraveldatamanager.h"

#include <QtCore/qvariant.h>

#include <Jsoner/object.h>
#include <Jsoner/array.h>

#include <DataGate/datarequest.h>
#include <DataGate/dataresponse.h>
#include <DataGate/abstractdataclient.h>

#include <RestLink/request.h>
#include <RestLink/body.h>
#include <RestLink/response.h>

using namespace Jsoner;
using namespace DataGate;
using namespace RestLink;

namespace RestGate {

LaravelDataManager::LaravelDataManager(Api *api)
    : DataManager(api)
{
}

LaravelDataManager::~LaravelDataManager()
{
}

bool LaravelDataManager::hasFeature(Feature feature, AbstractDataClient *client) const
{
    Q_UNUSED(feature);
    return client;
}

void LaravelDataManager::fetchSomeSearchSuggestions(const DataRequest &request, const DataRequestCallback &onProgress, const DataResponseCallback &onResponse)
{
    DataProcessor processor = {
        .method = Method::GetMethod,
        .requestConverter = [this](const DataRequest &request) -> Request {
            Request newRequest = this->newRequest("/search/suggestions", request);
            newRequest.addQueryParameter("query", request.query());
            return newRequest;
        },
        .progressCallback = onProgress,
        .responseCallback = onResponse
    };

    run(request, processor);
}

void LaravelDataManager::fetchManyObjects(const DataRequest &request, const DataRequestCallback &onProgress, const DataResponseCallback &onResponse)
{
    DataProcessor processor = {
        .method = Method::GetMethod,
        .requestConverter = [this](const DataRequest &request) -> Request {
            Request newRequest = this->newRequest("/", request);

            if (request.hasQuery())
                newRequest.addQueryParameter("query", request.query());

            const QVariantMap filters = request.filters();
            const QStringList filterNames = filters.keys();
            for (const QString &filterName : filterNames)
                newRequest.addQueryParameter(filterName, filters.value(filterName));

            if (request.hasSort())
                newRequest.addQueryParameter("sort_by", request.sortField() + '.' + (request.sortOrder() == Qt::DescendingOrder ? "desc" : "asc"));

            return newRequest;
        },
        .responseConverter = [](Response *response) -> DataResponse {
            DataResponse res;

            if (response->isSuccess()) {
                const Object object = response->readJsonObject();
                res.setArray(object.array("data"));
                res.setPage(object.integer("current_page"));
                res.setPageCount(object.integer("last_page"));

                const QStringList fields = { "from", "to", "per_page", "total" };
                for (const QString &field : fields)
                    res.setData(field, object.variant(field));
            } else if (response->hasHttpStatusCode()) {
                const Object object = response->readJsonObject();

                res.setTitle(tr("Error"));
                res.setText(tr("The server reported an error."));
                if (object.has("message"))
                    res.setInformativeText(object.string("message"));

                if (object.contains("errors")) {
                    QString msg;
                    const Array errors = object.array("errors");

                    for (const QJsonValue &value : errors)
                        msg.append(value.toString() + "\n");

                    msg.append(QStringLiteral("\nStatus: HTTP %1 %2").arg(response->httpStatusCode()).arg(response->httpReasonPhrase()));
                    msg.append("\n*" + tr("If the problem persists, please contact the developer."));

                    res.setDetailedText(msg);
                }

                res.setObject(object);
            } else if (response->hasNetworkError()) {
                res.setTitle(tr("Network Error"));
                res.setText(tr("A network error occured"));
                res.setDetailedText(response->networkErrorString());
            } else {
                res.setTitle(tr("Error"));
                res.setText(tr("An unknown error occured, we don't known more"));
            }

            return res;
        },
        .progressCallback = onProgress,
        .responseCallback = onResponse
    };

    run(request, processor);
}

void LaravelDataManager::fetchOneObject(const DataRequest &request, const DataRequestCallback &onProgress, const DataResponseCallback &onResponse)
{
    DataProcessor processor = {
        .method = Method::GetMethod,
        .requestConverter = [this](const DataRequest &request) -> Request {
            return newRequest("/" + request.object().string("id"), request);
        },
        .progressCallback = onProgress,
        .responseCallback = onResponse
    };

    run(request, processor);
}

void LaravelDataManager::addOneObject(const DataRequest &request, const DataRequestCallback &onProgress, const DataResponseCallback &onResponse)
{
    DataProcessor processor = {
        .method = Method::PostMethod,
        .requestConverter = [this](const DataRequest &request) -> Request {
            return newRequest("/" + request.object().string("id"), request);
        },
        .bodyGenerator = [](const DataRequest &request) -> Body {
            return Body(request.object());
        },
        .progressCallback = onProgress,
        .responseCallback = onResponse
    };

    run(request, processor);
}

void LaravelDataManager::editOneObject(const DataRequest &request, const DataRequestCallback &onProgress, const DataResponseCallback &onResponse)
{
    DataProcessor processor = {
        .method = Method::PutMethod,
        .requestConverter = [this](const DataRequest &request) -> Request {
            return newRequest("/" + request.object().string("id"), request);
        },
        .bodyGenerator = [](const DataRequest &request) -> Body {
            return Body(request.object());
        },
        .progressCallback = onProgress,
        .responseCallback = onResponse
    };

    run(request, processor);
}

void LaravelDataManager::deleteOneObject(const DataGate::DataRequest &request, const DataGate::DataRequestCallback &onProgress, const DataGate::DataResponseCallback &onResponse)
{
    DataProcessor processor = {
        .method = Method::DeleteMethod,
        .requestConverter = [this](const DataRequest &request) -> Request {
            return newRequest("/" + request.object().string("id"), request);
        },
        .progressCallback = onProgress,
        .responseCallback = onResponse
    };

    run(request, processor);
}

void LaravelDataManager::deleteManyObjects(const DataRequest &request, const DataRequestCallback &onProgress, const DataResponseCallback &onResponse)
{
    DataProcessor processor = {
        .method = Method::PostMethod,
        .requestConverter = [this](const DataRequest &request) -> Request {
            return newRequest("/bulk-delete", request);
        },
        .bodyGenerator = [](const DataRequest &request) -> Body {
            Array objects = request.array();
            std::transform(objects.begin(), objects.end(), objects.begin(), [](const QJsonValue &value) {
                return value.toObject().value("id");
            });

            Object object;
            object.put("ids", objects);
            return Body(object);
        },
        .progressCallback = onProgress,
        .responseCallback = onResponse
    };

    run(request, processor);
}

Request LaravelDataManager::newRequest(const QString &subEndpoint, const DataGate::DataRequest &request) const
{
    QString endpoint = request.parameter("endpoint").toString();
    if (endpoint.isEmpty() && request.client())
        endpoint = request.client()->parameter("endpoint").toString();

    if  (endpoint.isEmpty())
        return Request();

    Request req(endpoint  + (subEndpoint == '/' ? QString() : subEndpoint));
    req.setHeader("Accept", "application/json");
    return req;
}

} // namespace RestGate
