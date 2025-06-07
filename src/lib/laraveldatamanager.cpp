#include "laraveldatamanager.h"

#include <QtCore/qvariant.h>

#include <Jsoner/object.h>
#include <Jsoner/array.h>

#include <DataGate/dataquery.h>
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
    : api(api)
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

void LaravelDataManager::fetchSomeSearchSuggestions(const DataQuery &query, const DataQueryProgressCallback &onProgress, const DataQueryResponseCallback &onResponse)
{
    Request request = newRequest("/search/suggestions", query);
    request.addQueryParameter("query", query.string());
    request.setHeader("Accept", "application/json");

    auto processing = [=](Response *response) {
        DataResponse res;
        res.setSuccess(response->isSuccess());
        res.setArray(response->readJsonArray());
        onResponse(res);
    };

    Response *response = api->get(request);
    registerResponse(response, processing, onProgress);
}

void LaravelDataManager::fetchManyObjects(const DataQuery &query, const DataQueryProgressCallback &onProgress, const DataQueryResponseCallback &onResponse)
{
    Request request = newRequest("/", (query));
    request.setHeader("Accept", "application/json");

    const QString queryString = query.string();
    if (queryString.size() > 0)
        request.addQueryParameter("query", queryString);

    const QVariantHash filters = query.filters();
    const QStringList filterNames = filters.keys();
    for (const QString &filterName : filterNames)
        request.addQueryParameter(filterName, filters.value(filterName));

    const QString sortField = query.sortField();
    if (!sortField.isEmpty())
        request.addQueryParameter("sort_by", sortField + '.' + (query.sortOrder() == Qt::DescendingOrder ? "desc" : "asc"));

    request.addQueryParameter("page", query.page());

    auto processing = [=](Response *response) {
        DataResponse res;
        res.setSuccess(response->isSuccess());

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

        onResponse(res);
    };

    Response *response = api->get(request);
    registerResponse(response, processing, onProgress);
}

void LaravelDataManager::fetchOneObject(const DataQuery &query, const DataQueryProgressCallback &onProgress, const DataQueryResponseCallback &onResponse)
{
    Request request = newRequest('/' + query.object().string("id"), query);
    request.setHeader("Accept", "application/json");

    auto processing = [=](Response *response) {
        DataResponse res;
        res.setSuccess(response->isSuccess());
        res.setObject(response->readJsonObject());
        onResponse(res);
    };

    Response *response = api->get(request);
    registerResponse(response, processing, onProgress);
}

void LaravelDataManager::addOneObject(const DataQuery &query, const DataQueryProgressCallback &onProgress, const DataQueryResponseCallback &onResponse)
{
    Request request = newRequest("/", (query));
    request.setHeader("Accept", "application/json");

    auto processing = [=](Response *response) {
        DataResponse res;
        res.setSuccess(response->isSuccess());
        res.setObject(response->readJsonObject());
        onResponse(res);
    };

    Response *response = api->post(request, query.object());
    registerResponse(response, processing, onProgress);
}

void LaravelDataManager::editOneObject(const DataQuery &query, const DataQueryProgressCallback &onProgress, const DataQueryResponseCallback &onResponse)
{
    Request request = newRequest('/' + query.object().string("id"), query);
    request.setHeader("Accept", "application/json");

    auto processing = [=](Response *response) {
        DataResponse res;
        res.setSuccess(response->isSuccess());
        res.setObject(response->readJsonObject());
        onResponse(res);
    };

    Response *response = api->put(request, query.object());
    registerResponse(response, processing, onProgress);
}

void LaravelDataManager::deleteOneObject(const DataGate::DataQuery &query, const DataGate::DataQueryProgressCallback &onProgress, const DataGate::DataQueryResponseCallback &onResponse)
{
    Request request = newRequest('/' + query.object().string("id"), query);

    auto processing = [=](Response *response) {
        DataResponse res;
        res.setSuccess(response->isSuccess());
        res.setObject(response->readJsonObject());
        onResponse(res);
    };

    Response *response = api->deleteResource(request);
    registerResponse(response, processing, onProgress);
}

void LaravelDataManager::deleteManyObjects(const DataQuery &query, const DataQueryProgressCallback &onProgress, const DataQueryResponseCallback &onResponse)
{
    Request request = newRequest("/bulk-delete", query);

    auto processing = [=](Response *response) {
        DataResponse res;
        res.setSuccess(response->isSuccess());
        res.setObject(response->readJsonObject());
        onResponse(res);
    };

    Array ids;
    Array objects = query.array();
    std::transform(objects.begin(), objects.end(), std::back_inserter(ids), [](const QJsonValue &value) {
        return value.toObject().value("id");
    });

    Response *response = api->post(request, ids);
    registerResponse(response, processing, onProgress);
}

Request LaravelDataManager::newRequest(const QString &subEndpoint, const DataGate::DataQuery &query) const
{
    QString endpoint = query.parameter("endpoint").toString();
    if (endpoint.isEmpty() && query.client())
        endpoint = query.client()->parameter("endpoint").toString();

    if  (endpoint.isEmpty())
        return Request();

    Request request = endpoint  + (subEndpoint == '/' ? QString() : subEndpoint);
    request.setHeader("Accept", "application/json");
    return request;
}

void LaravelDataManager::registerResponse(Response *response, const ApiRunCallback &callback, const DataQueryProgressCallback &onProgress)
{
    QObject::connect(response, &Response::downloadProgress, response, onProgress);
    QObject::connect(response, &Response::finished, response, [callback, response] { callback(response); });
    QObject::connect(response, &Response::finished, response, &QObject::deleteLater);
}

} // namespace RestGate
