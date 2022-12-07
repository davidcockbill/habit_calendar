#ifndef _LOGGING_HPP_
#define _LOGGING_HPP_

#include <Arduino.h>

enum Level { ERROR, WARN, INFO, DEBUG, TRACE};

class Logger
{
private:
    Level mLevel;

public:
    Logger();
    Logger(Level level);

    void setLevel(Level level);

    void error(const char *fmt, ...) const;
    void warn(const char *fmt, ...) const;
    void info(const char *fmt, ...) const;
    void debug(const char *fmt, ...) const;
    void trace(const char *fmt, ...) const;

    void error(const __FlashStringHelper *ifsh) const;
    void warn(const __FlashStringHelper *ifsh) const;
    void info(const __FlashStringHelper *ifsh) const;
    void debug(const __FlashStringHelper *ifsh) const;
    void trace(const __FlashStringHelper *ifsh) const;
};

static const Logger LOGGER(Level::INFO);

#endif