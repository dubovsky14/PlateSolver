#pragma once

#include<memory>
#include<fstream>
#include<string>

namespace PlateSolver   {
    class Logger    {
        public:
            static void log_message(const std::string &message);

            static void set_log_file(const std::string &file_address, bool recreate = false);

            static void enable_logging()    {s_logging_on = true;};

            static void disable_logging()   {s_logging_on = false;};
        private:
            Logger();

            static std::shared_ptr<std::ofstream> m_logging_file;
            static bool s_logging_on;

    };
}