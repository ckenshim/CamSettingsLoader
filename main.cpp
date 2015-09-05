#include <iostream>
#include "CarWashApiClient.h"
#include <Poco/SimpleFileChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/SplitterChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <fstream>
using namespace std;

int main() {


    // *** Logger settings ****
    Poco::AutoPtr<Poco::PatternFormatter> pPF(new Poco::PatternFormatter);
    pPF->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s: %t");

    Poco::AutoPtr<Poco::ConsoleChannel> pCons(new Poco::ConsoleChannel);
    Poco::AutoPtr<Poco::FormattingChannel> pFC(new Poco::FormattingChannel(pPF, pCons));

    Poco::AutoPtr<Poco::SimpleFileChannel> pFile(new Poco::SimpleFileChannel);
    pFile->setProperty("path", "settings_loader.log");
    pFile->setProperty("rotation", "1 M");
    Poco::AutoPtr<Poco::FormattingChannel> pFF(new Poco::FormattingChannel(pPF, pFile));

    Poco::AutoPtr<Poco::SplitterChannel> pSplitter(new Poco::SplitterChannel);
    pSplitter->addChannel(pFC);
    pSplitter->addChannel(pFF);

    Poco::Logger::root().setChannel(pSplitter);
    Poco::Logger::root().setLevel("warning");

    Poco::Logger &logger_ = Poco::Logger::get("main");
    // ******************************

    try{
        Poco::File ids_file("ids.txt");

        if (!ids_file.exists()) {
            logger_.critical("CRITICAL!!! ids.txt file does not exist.");
            return -1;
        }

        if (!ids_file.canRead()) {
            logger_.critical("CRITICAL!!! Unable to read ids.txt file");
            return -1;
        }

        CarWashApiClient client;

        std::ifstream filein("ids.txt");

        int counter = 0;

        std::string str_dir = "settings";
        Poco::File settings_path(str_dir);
        settings_path.createDirectories();


        // output file stream
        std::ofstream settings_file;
        for (std::string line; std::getline(filein, line);) {
            counter++;
            bool is_ok = client.doRequest(line);

            if (is_ok) {
                settings_file.open(str_dir + "/" + std::to_string(counter) + ".txt");
                settings_file << client.getResponse();
                settings_file.close();
            }
        }

    }
    catch (Poco::Exception& exc)
    {
        logger_.error("Error: " + exc.displayText());
        logger_.error("Error: " + exc.message());
    }

    return 0;

}