#ifndef RESTGATE_LARAVELDATAMANAGER_H
#define RESTGATE_LARAVELDATAMANAGER_H

#include <RestGate/global.h>

#include <QtCore/qcoreapplication.h>

#include <DataGate/datamanager.h>

#include <RestLink/api.h>

namespace RestGate {

class RESTGATE_EXPORT LaravelDataManager : public DataGate::DataManager
{
    Q_DECLARE_TR_FUNCTIONS(Laravel)

public:
    explicit LaravelDataManager(RestLink::Api *api);
    virtual ~LaravelDataManager();

    bool hasFeature(Feature feature, DataGate::AbstractDataClient *client) const override;

protected:
    void fetchSomeSearchSuggestions(const DataGate::DataQuery &query, const DataGate::DataQueryProgressCallback &onProgress, const DataGate::DataQueryResponseCallback &onResponse) override;
    void fetchManyObjects(const DataGate::DataQuery &query, const DataGate::DataQueryProgressCallback &onProgress, const DataGate::DataQueryResponseCallback &onResponse) override;
    void fetchOneObject(const DataGate::DataQuery &query, const DataGate::DataQueryProgressCallback &onProgress, const DataGate::DataQueryResponseCallback &onResponse) override;
    void addOneObject(const DataGate::DataQuery &query, const DataGate::DataQueryProgressCallback &onProgress, const DataGate::DataQueryResponseCallback &onResponse) override;
    void editOneObject(const DataGate::DataQuery &query, const DataGate::DataQueryProgressCallback &onProgress, const DataGate::DataQueryResponseCallback &onResponse) override;
    void deleteOneObject(const DataGate::DataQuery &query, const DataGate::DataQueryProgressCallback &onProgress, const DataGate::DataQueryResponseCallback &onResponse) override;
    void deleteManyObjects(const DataGate::DataQuery &query, const DataGate::DataQueryProgressCallback &onProgress, const DataGate::DataQueryResponseCallback &onResponse) override;

    virtual RestLink::Request newRequest(const QString &subEndpoint, const DataGate::DataQuery &query) const;

private:
    RestLink::Api *m_api;

    void registerResponse(RestLink::Response *response, const RestLink::ApiRunCallback &callback, const DataGate::DataQueryProgressCallback &onProgress);
};

} // namespace RestGate

#endif // RESTGATE_LARAVELDATAMANAGER_H
