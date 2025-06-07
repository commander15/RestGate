#ifndef RESTGATE_LARAVELLOGINMANAGER_H
#define RESTGATE_LARAVELLOGINMANAGER_H

#include <RestGate/global.h>

#include <DataGate/abstractloginmanager.h>

namespace RestLink {
class Api;
class Request;
}

namespace RestGate {

class RESTGATE_EXPORT LaravelLoginManager : public DataGate::AbstractLoginManager
{
public:
    LaravelLoginManager(RestLink::Api *api);
    virtual ~LaravelLoginManager();

protected:
    enum Operation { LogIn, LogOut };

    void attemptLogIn(const DataGate::LoginQuery &query, const DataGate::DataQueryResponseCallback &callback) override;
    void attemptLogOut(const DataGate::LoginQuery &query, const DataGate::DataQueryResponseCallback &callback) override;

    virtual RestLink::Request newRequest(Operation operation, const DataGate::LoginQuery &query) const;

    RestLink::Api *api;
};

} // namespace RestGate

#endif // RESTGATE_LARAVELLOGINMANAGER_H
