
#include <iostream>
#include "IpCamVideoAcquisition.h"
#include "Camera.h"
#include <opencv2/opencv.hpp>
#include <ctime>
#include <chrono>
#include <string>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/HTTPCredentials.h>
#include <Poco/Exception.h>
#include <Poco/SimpleFileChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/SplitterChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/File.h>
#include <Poco/Logger.h>
#include <Poco/URI.h>
#include <Poco/AutoPtr.h>
#include <Poco/StreamCopier.h>
#include <fstream>


using namespace std::chrono;

void parseJSON(const std::string & str_json, Camera &camera, Poco::Logger & logger);
bool readSettingsFile(const std::string & file_name, Camera & camera, Poco::Logger & logger );

const std::string SETTINGS_DIR = "settings/";


int main(int argc, char* argv[])
{
    if (argc != 2){
        std::cout << "usage: camworker [id]\n";
        return -1;
    }

    // *** Logger settings ****
    Poco::AutoPtr<Poco::PatternFormatter> pPF(new Poco::PatternFormatter);
    pPF->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s: %t");

    Poco::AutoPtr<Poco::ConsoleChannel> pCons(new Poco::ConsoleChannel);
    Poco::AutoPtr<Poco::FormattingChannel> pFC(new Poco::FormattingChannel(pPF, pCons));

    Poco::AutoPtr<Poco::SimpleFileChannel> pFile(new Poco::SimpleFileChannel);
    pFile->setProperty("path", "camworker_" + std::string(argv[1]) +".log");
    pFile->setProperty("rotation", "1 M");
    Poco::AutoPtr<Poco::FormattingChannel> pFF(new Poco::FormattingChannel(pPF, pFile));

    Poco::AutoPtr<Poco::SplitterChannel> pSplitter(new Poco::SplitterChannel);
    pSplitter->addChannel(pFC);
    pSplitter->addChannel(pFF);

    Poco::Logger::root().setChannel(pSplitter);
    Poco::Logger::root().setLevel("debug");

    Poco::Logger &logger_ = Poco::Logger::get("main");
    // ******************************


    std::string file_name = SETTINGS_DIR + argv[1] + ".txt";
    Poco::File settings_file(file_name);

    if (!settings_file.exists()) {
        logger_.critical("CRITICAL!!! " + file_name + " file does not exist.");
        return -1;
    }

    if (!settings_file.canRead()) {
        logger_.critical("CRITICAL!!! Unable to read " + file_name + " file");
        return -1;
    }


    Camera camera;

    bool result = readSettingsFile(file_name, camera, logger_);

    // if settings read unsuccessfully, then terminate
    if(!result){
        logger_.error("Unable to read settings file. Cannot continue.");
        return -1;
    }

    bool work_ = true;
    //vae::VideoAcquisition* ipcam_ = new vae::IpCamVideoAcquisition();
    std::string url = camera.video_source();
    //ipcam_->set_url(url);

    //if(ipcam_->Init() != 0){
    //    std::cout << "Error: Initialization failed.\n";
    //    return -1;
    //}

    cv::Ptr<cv::BackgroundSubtractor> mog = cv::createBackgroundSubtractorMOG2();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));

    cv::Mat im_gray;
    cv::Mat frame_small;
    int counter = 0;

    //AVFrame* pframe = NULL;
    while(work_){

        //pframe = ipcam_->NextFrame();

        //if(pframe!=NULL){
//            cv::Mat image(pframe->height, pframe->width, CV_8UC3, pframe->data[0]);
//            int new_w = image.size().width / 6;
//            int new_h = image.size().height / 6;
//            cv::resize(image, frame_small, cv::Size(new_w, new_h));
//
//            cvtColor(frame_small, im_gray, CV_BGR2GRAY);
//
//            cv::Mat mask;
//            mog->apply(im_gray, mask);
//
//
//            cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
//
//            int blob = 0;
//
//            for (int i = 0; i < new_h; i++){
//                for (int j = 0; j < new_w; j++){
//                    uchar value = mask.at<uchar>(i, j);
//                    if (value != 0){
//                        blob++;
//                    }
//                }
//            }
//            if (blob > 1500){
//                std::cout << blob << "\n";
//                milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
//                cv::imwrite("5E5311FA-B1A1-472D_" + std::to_string(ms.count()) + ".jpg", image);
//            }
            //imshow("morph", mask);
            //imshow("real", frame);
            //if (cv::waitKey(1) >= 0) break;
        //}
    }
    return 0;
}

bool readSettingsFile(const std::string & file_name, Camera & camera, Poco::Logger & logger ){

    try {
        std::ifstream ifs(file_name);

        std::string str_json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        parseJSON(str_json, camera, logger);

        return true;

    }
    catch (Poco::Exception& exc)
    {
        logger.error("Error: " + exc.displayText());
        logger.error("Error: " + exc.message());
    }

    return false;

}

void parseJSON(const std::string & str_json, Camera &camera, Poco::Logger& logger) {
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var result = parser.parse(str_json);
    Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
    Poco::Dynamic::Var camId = object->get("cameraId");
    Poco::Dynamic::Var sourceUrl = object->get("sourceUrl");
    Poco::Dynamic::Var urlSafeKey = object->get("urlSafeKey");
    Poco::Dynamic::Var isActive = object->get("isActive");
    Poco::Dynamic::Var threshold = object->get("threshold");

    logger.information("Parsing json settings file...");

    if (!camId.isEmpty() && camId.isString()) {
        camera.set_camera_id(camId.toString());
        logger.information("camId: " + camId.toString());
    }

    if (!sourceUrl.isEmpty() && sourceUrl.isString()) {
        camera.set_video_source(sourceUrl.toString());
        logger.information("sourceUrl: " + sourceUrl.toString());
    }

    if (!urlSafeKey.isEmpty() && urlSafeKey.isString()) {
        camera.set_websafe_url(urlSafeKey.toString());
        logger.information("urlSafeKey: " + urlSafeKey.toString() );
    }

    if (!isActive.isEmpty() && isActive.isBoolean()) {
        bool val = isActive.convert<bool>();
        camera.set_active(val);
        if(val)
            logger.information("isActive: true");
        else
            logger.information("isActive: false");
    }

    if (!threshold.isEmpty() && threshold.isString()) {
        int val = threshold.convert<int>();
        camera.set_motion_threshold(val);
        logger.information("threshold: " + std::to_string(val) );
    }

}




