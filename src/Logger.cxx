#include "../PlateSolver/Logger.h"

#include<memory>
#include<fstream>
#include<string>
#include<iostream>

using namespace PlateSolver;
using namespace std;

std::shared_ptr<std::ofstream> Logger::m_logging_file = nullptr;
bool Logger::s_logging_on = false;

void Logger::log_message(const std::string &message)    {
    if (!s_logging_on)  {
        return;
    }
    if (m_logging_file == nullptr)  {
        cout << message << endl;
    }
    *m_logging_file << message << endl;
};

void Logger::set_log_file(const std::string &file_address, bool recreate)   {
    if (recreate)   {
        m_logging_file = make_shared<ofstream>(file_address);
    }
    else {
        m_logging_file = make_shared<ofstream>(file_address, ios::app);
    }
};

