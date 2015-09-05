//
// Created by chingy on 9/5/15.
//

#ifndef CAMSETTINGSDOWNLOADER_CARWASHAPICLIENT_H
#define CAMSETTINGSDOWNLOADER_CARWASHAPICLIENT_H

#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/Context.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>
#include <Poco/StreamCopier.h>
#include <Poco/Logger.h>
#include <iostream>
#include <string>
#include <sstream>

class CarWashApiClient {
private:
    std::string api_url_;
    std::string str_response_;

    Poco::Logger& logger_ = Poco::Logger::get("CarWashApiClient");

public:
    CarWashApiClient();

    CarWashApiClient(const std::string &api_url):
            api_url_(api_url)
    { }

    const std::string &getApi_url() const;
    void setApi_url(const std::string &api_url);

    const std::string &getResponse() const;

    bool doRequest(const std::string & cam_id);


};


#endif //CAMSETTINGSDOWNLOADER_CARWASHAPICLIENT_H
