//
// Created by chingy on 9/5/15.
//

#include "CarWashApiClient.h"

CarWashApiClient::CarWashApiClient():
        api_url_("https://image-receiver-dot-carwashauthority-1007.appspot.com/_ah/api/imagereceiverendpoints/v1/discoverCamera?val=")
{}


const std::string & CarWashApiClient::getApi_url() const {
    return api_url_;
}

void CarWashApiClient::setApi_url(const std::string &api_url) {
    api_url_ = api_url;
}

bool CarWashApiClient::doRequest(const std::string & cam_id){
    bool success = false;
    try{

        std::string url = "https://image-receiver-dot-carwashauthority-1007.appspot.com/_ah/api/imagereceiverendpoints/v1/discoverCamera?val=" + cam_id;

        Poco::URI uri(url);
        std::string path(uri.getPathAndQuery());
        if (path.empty()) path = "/";

        const Poco::Net::Context::Ptr ptrContext( new Poco::Net::Context( Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH" ) );
        Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort(), ptrContext);
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
        Poco::Net::HTTPResponse response;



        for (int i=0;i<5;i++){
            logger_.information("Sending request to server...");
            logger_.information("Camera Id: " + cam_id);

            session.sendRequest(request);
            std::istream& rs = session.receiveResponse(response);

            std::string status = "Status :" + std::to_string(response.getStatus());

            std::ostringstream oss;
            Poco::StreamCopier::copyStream(rs, oss);
            str_response_ = oss.str();
            if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
                logger_.information(status);
                logger_.information(response.getReason());
                success = true;
                break;
            }
            else{
                logger_.warning(status);
                logger_.warning(response.getReason());
                logger_.warning("Failure. Repeating request for cam_id '" + cam_id +"'");
                success = false;
            }

        }



    }
    catch (Poco::Exception& exc)
    {
        logger_.error("Error: " + exc.displayText());
        logger_.error("Error: " + exc.message());
    }

    return success;
}

const std::string &CarWashApiClient::getResponse() const {
    return str_response_;
}
