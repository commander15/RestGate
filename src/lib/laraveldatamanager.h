#ifndef RESTGATE_LARAVELDATAMANAGER_H
#define RESTGATE_LARAVELDATAMANAGER_H

#include <RestGate/global.h>
#include <RestGate/datamanager.h>

#include <QtCore/qcoreapplication.h>

namespace RestGate {

class RESTGATE_EXPORT LaravelDataManager : public DataManager
{
    Q_DECLARE_TR_FUNCTIONS(Laravel)

public:
    explicit LaravelDataManager(RestLink::Api *api);
    virtual ~LaravelDataManager();

    bool hasFeature(Feature feature, DataGate::AbstractDataClient *client) const override;

protected:
    void fetchSomeSearchSuggestions(const DataGate::DataRequest &request, const DataGate::DataRequestCallback &onProgress, const DataGate::DataResponseCallback &onResponse) override;
    void fetchManyObjects(const DataGate::DataRequest &request, const DataGate::DataRequestCallback &onProgress, const DataGate::DataResponseCallback &onResponse) override;
    void fetchOneObject(const DataGate::DataRequest &request, const DataGate::DataRequestCallback &onProgress, const DataGate::DataResponseCallback &onResponse) override;
    void addOneObject(const DataGate::DataRequest &request, const DataGate::DataRequestCallback &onProgress, const DataGate::DataResponseCallback &onResponse) override;
    void editOneObject(const DataGate::DataRequest &request, const DataGate::DataRequestCallback &onProgress, const DataGate::DataResponseCallback &onResponse) override;
    void deleteOneObject(const DataGate::DataRequest &request, const DataGate::DataRequestCallback &onProgress, const DataGate::DataResponseCallback &onResponse) override;
    void deleteManyObjects(const DataGate::DataRequest &request, const DataGate::DataRequestCallback &onProgress, const DataGate::DataResponseCallback &onResponse) override;

    virtual RestLink::Request newRequest(const QString &endpoint, const DataGate::DataRequest &request) const;
};

} // namespace RestGate

#endif // RESTGATE_LARAVELDATAMANAGER_H
