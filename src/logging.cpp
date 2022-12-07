#include <Arduino.h>
#include "logging.hpp"


void println(const char *fmt, va_list args)
{
    char buffer[50];
    if ((uint32_t)vsnprintf(buffer, sizeof(buffer), fmt, args) <= sizeof(buffer))
    {
        Serial.println(buffer);
    }
    va_end(args);
}

Logger::Logger(): mLevel(Level::INFO)
{
}

Logger::Logger(Level level): mLevel(level)
{
}

void Logger::setLevel(Level level)
{
    mLevel = level;
}

void Logger::error(const char *fmt, ...) const
{
    if (mLevel >= Level::ERROR)
    {
        va_list args;
        va_start(args, fmt);
        println(fmt, args);
    }
}

void Logger::warn(const char *fmt, ...) const
{
    if (mLevel >= Level::WARN)
    {
        va_list args;
        va_start(args, fmt);
        println(fmt, args);
    }
}

void Logger::info(const char *fmt, ...) const
{
    if (mLevel >= Level::INFO)
    {
        va_list args;
        va_start(args, fmt);
        println(fmt, args);
    }
}

void Logger::debug(const char *fmt, ...) const
{
    if (mLevel >= Level::DEBUG)
    {
        va_list args;
        va_start(args, fmt);
        println(fmt, args);
    }
}
    
void Logger::trace(const char *fmt, ...) const
{
    if (mLevel >= Level::TRACE)
    {
        va_list args;
        va_start(args, fmt);
        println(fmt, args);
    }
}

void Logger::error(const __FlashStringHelper *ifsh) const
{
    if (mLevel >= Level::ERROR)
    {
        Serial.println(ifsh);
    }
}

void Logger::warn(const __FlashStringHelper *ifsh) const
{
    if (mLevel >= Level::WARN)
    {
        Serial.println(ifsh);
    }
}

void Logger::info(const __FlashStringHelper *ifsh) const
{
    if (mLevel >= Level::INFO)
    {
        Serial.println(ifsh);
    }
}

void Logger::debug(const __FlashStringHelper *ifsh) const
{
    if (mLevel >= Level::DEBUG)
    {
        Serial.println(ifsh);
    }
}

void Logger::trace(const __FlashStringHelper *ifsh) const
{
    if (mLevel >= Level::TRACE)
    {
        Serial.println(ifsh);
    }
}

