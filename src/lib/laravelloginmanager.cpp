#include "laravelloginmanager.h"

#include <Jsoner/object.h>
#include <Jsoner/array.h>

#include <DataGate/dataresponse.h>
#include <DataGate/authenticator.h>

#include <RestLink/request.h>
#include <RestLink/body.h>
#include <RestLink/response.h>

using namespace Jsoner;
using namespace DataGate;
using namespace RestLink;

namespace RestGate {

LaravelLoginManager::LaravelLoginManager(RestLink::Api *api)
    : m_api(api)
{
}

LaravelLoginManager::~LaravelLoginManager()
{
}

void LaravelLoginManager::attemptLogIn(const LoginQuery &query, const DataQueryResponseCallback &callback)
{
    Jsoner::Object object;
    object.put("email", query.identifier());
    object.put("password", query.password());

    m_api->post(newRequest(LogIn, query), object, [=](RestLink::Response *response) {
        DataGate::DataResponse res;
        res.setSuccess(response->isSuccess());

        if (response->isSuccess()) {
            res.setObject(response->readJsonObject());
        } else {
            res.setCode(DataGate::AuthenticationError::BadCredentials);

            if (response->hasHttpStatusCode()) {
                const Object object = response->readJsonObject();
                res.setText(object.string("message"));
            }
        }

        callback(res);
    });
}

void LaravelLoginManager::attemptLogOut(const LoginQuery &query, const DataQueryResponseCallback &callback)
{
    Request request = newRequest(LogOut, query);
    if (!request.endpoint().isEmpty()) {
        DataResponse response;
        response.setSuccess(true);
        callback(response);
        return;
    }

    m_api->post(request, query.object(), [=](RestLink::Response *response) {
        DataGate::DataResponse res;

        if (response->isSuccess()) {
            res.setSuccess(true);
            res.setObject(response->readJsonObject());
        } else {
            res.setSuccess(false);

            if (response->hasHttpStatusCode()) {
                const Object object = response->readJsonObject();
                res.setText(object.string("message"));
            }
        }

        callback(res);
    });
}

Request LaravelLoginManager::newRequest(Operation operation, const DataGate::LoginQuery &query) const
{
    Q_UNUSED(query);

    RestLink::Request request(operation == LogIn ? "/user/login" : "/user/logout");
    request.setHeader("Accept", "application/json");
    return request;
}


} // namespace RestGate
