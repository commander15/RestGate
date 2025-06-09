#ifndef RESTGATE_DATAMANAGER_H
#define RESTGATE_DATAMANAGER_H

#include <RestGate/global.h>

#include <DataGate/datamanager.h>

#include <RestLink/api.h>

namespace RestGate {

typedef std::function<RestLink::Request(const DataGate::DataRequest &)> RequestConverter;
typedef std::function<DataGate::DataResponse(RestLink::Response *)> ResponseConverter;
typedef std::function<RestLink::Body(const DataGate::DataRequest &)> BodyGenerator;

class RESTGATE_EXPORT DataManager : public DataGate::DataManager
{
public:
    typedef RestLink::AbstractRequestHandler::Method Method;

    struct DataProcessor {
        Method method = Method::GetMethod;
        QString endpoint;

        RequestConverter requestConverter;
        ResponseConverter responseConverter;
        BodyGenerator bodyGenerator;

        DataGate::DataRequestCallback progressCallback;
        DataGate::DataResponseCallback responseCallback;

        bool isValid() const
        { return method != Method::UnknownMethod && requestConverter && responseCallback; }
    };

    explicit DataManager(RestLink::Api *api);
    virtual ~DataManager();

protected:
    void run(const DataGate::DataRequest &request, const DataProcessor &processor);

    RestLink::Api *api;
};

} // namespace RestGate

#endif // RESTGATE_DATAMANAGER_H
