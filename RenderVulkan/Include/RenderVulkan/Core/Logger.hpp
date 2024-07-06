#pragma once

#include "RenderVulkan/Util/DateTime.hpp"
#include "RenderVulkan/Util/Formatter.hpp"

#define Logger_WriteConsole(message, level) Logger::WriteConsole(message, __FUNCTION__, level)
#define Logger_ThrowException(message, fatal) Logger::ThrowException(message, __FUNCTION__, fatal)

#define Logger_ThrowIfFailed(result, message, fatal) if (result != VK_SUCCESS) Logger_ThrowException(Formatter::Format("{}; '{}'", message, std::to_string(result)), fatal)

using namespace RenderVulkan::Util;

namespace RenderVulkan
{
    namespace Core
    {
        enum class LogLevel
        {
            INFORMATION,
            DEBUGGING,
            WARNING,
            ERROR,
            FATAL_ERROR
        };

        class Logger
        {

        public:

            static void WriteConsole(const String& message, const String& function, const LogLevel& level)
            {
                static String name = GetFunctionName(function);
                name[0] = toupper(name[0]);

                switch (level)
                {

                case LogLevel::INFORMATION:
                    std::cout << Formatter::Format(Formatter::ColorFormat("&2&l[{}] [Thread/INFORMATION] [{}]: {}"), DateTime::Get("%H:%S:%M"), name, message) << std::endl;
                    break;

                case LogLevel::DEBUGGING:
                    std::cout << Formatter::Format(Formatter::ColorFormat("&1&l[{}] [Thread/DEBUGGING] [{}]: {}"), DateTime::Get("%H:%S:%M"), name, message) << std::endl;
                    break;

                case LogLevel::WARNING:
                    std::cout << Formatter::Format(Formatter::ColorFormat("&6[{}] [Thread/WARNING] [{}]: {}"), DateTime::Get("%H:%S:%M"), name, message) << std::endl;
                    break;

                case LogLevel::ERROR:
                    std::cout << Formatter::Format(Formatter::ColorFormat("&c[{}] [Thread/ERROR] [{}]: {}"), DateTime::Get("%H:%S:%M"), name, message) << std::endl;
                    break;

                case LogLevel::FATAL_ERROR:
                    std::cout << Formatter::Format(Formatter::ColorFormat("&4[{}] [Thread/FATAL ERROR] [{}]: {}"), DateTime::Get("%H:%S:%M"), name, message) << std::endl;
                    break;

                default:
                    break;
                }
            }

            static void ThrowException(const String& message, const String& function, bool fatal)
			{
                if (fatal)
                {
                    WriteConsole(message, function, LogLevel::FATAL_ERROR);

                    MessageBeep(MB_ICONERROR);
                    MessageBoxA(NULL, message.c_str(), "Fatal Error", MB_ICONERROR | MB_OK);

                    exit(EXIT_FAILURE);
                }
				else
				    WriteConsole(message, function, LogLevel::ERROR);
			}

        private:

            static String GetFunctionName(const String& input)
            {
                Size position = input.rfind("::");

                if (position != String::npos)
                    position += 2;
                else
                    position = 0;

                return input.substr(position);
            }

        };
    }
}