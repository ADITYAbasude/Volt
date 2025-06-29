#pragma once

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QDateTime>
#include <QMutex>
#include <QDebug>
#include <memory>

class VoltLogger {
public:
    enum LogLevel {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    // Singleton instance
    static VoltLogger& instance();
    
    // Initialize logger with file path and settings
    void initialize(const QString& logFilePath = "logs/volt.log", 
                   LogLevel minLevel = DEBUG,
                   bool clearOnStart = true,
                   bool enableConsole = true);
    
    // Core logging methods (Winston-style)
    void trace(const QString& message, const QString& category = "APP");
    void debug(const QString& message, const QString& category = "APP");
    void info(const QString& message, const QString& category = "APP");
    void warn(const QString& message, const QString& category = "APP");
    void error(const QString& message, const QString& category = "APP");
    void fatal(const QString& message, const QString& category = "APP");
    
    // Convenience methods for different categories
    void theme(const QString& message, LogLevel level = DEBUG);
    void ui(const QString& message, LogLevel level = DEBUG);
    void editor(const QString& message, LogLevel level = DEBUG);
    void system(const QString& message, LogLevel level = INFO);
    
    // Template methods for formatting with arguments (Qt-style)
    template<typename T1>
    void debugf(const QString& format, T1 arg1) {
        log(DEBUG, QString(format).arg(arg1), "APP");
    }
    
    template<typename T1, typename T2>
    void debugf(const QString& format, T1 arg1, T2 arg2) {
        log(DEBUG, QString(format).arg(arg1).arg(arg2), "APP");
    }
    
    template<typename T1, typename T2, typename T3>
    void debugf(const QString& format, T1 arg1, T2 arg2, T3 arg3) {
        log(DEBUG, QString(format).arg(arg1).arg(arg2).arg(arg3), "APP");
    }
    
    template<typename T1, typename T2, typename T3, typename T4>
    void debugf(const QString& format, T1 arg1, T2 arg2, T3 arg3, T4 arg4) {
        log(DEBUG, QString(format).arg(arg1).arg(arg2).arg(arg3).arg(arg4), "APP");
    }
    
    template<typename T1>
    void infof(const QString& format, T1 arg1) {
        log(INFO, QString(format).arg(arg1), "APP");
    }
    
    template<typename T1, typename T2>
    void infof(const QString& format, T1 arg1, T2 arg2) {
        log(INFO, QString(format).arg(arg1).arg(arg2), "APP");
    }
    
    template<typename T1>
    void warnf(const QString& format, T1 arg1) {
        log(WARN, QString(format).arg(arg1), "APP");
    }
    
    template<typename T1, typename T2>
    void warnf(const QString& format, T1 arg1, T2 arg2) {
        log(WARN, QString(format).arg(arg1).arg(arg2), "APP");
    }
    
    template<typename T1>
    void errorf(const QString& format, T1 arg1) {
        log(ERROR, QString(format).arg(arg1), "APP");
    }
    
    template<typename T1, typename T2>
    void errorf(const QString& format, T1 arg1, T2 arg2) {
        log(ERROR, QString(format).arg(arg1).arg(arg2), "APP");
    }
    
    // Configuration
    void setLogLevel(LogLevel level);
    void enableConsoleOutput(bool enable);
    void enableFileOutput(bool enable);
    
    // Session management
    void startSession();
    void endSession();
    
    // Get formatted log for display
    QString getRecentLogs(int lines = 50);
    
    // Cleanup
    void shutdown();
    
    // Public access to core logging function for macros
    void log(LogLevel level, const QString& message, const QString& category);

private:
    VoltLogger() = default;
    ~VoltLogger();
    
    // Helper functions
    QString formatLogEntry(LogLevel level, const QString& message, const QString& category);
    QString levelToString(LogLevel level);
    QString levelToColorCode(LogLevel level);
    
    // Member variables
    std::unique_ptr<QFile> m_logFile;
    std::unique_ptr<QTextStream> m_logStream;
    QMutex m_mutex;
    LogLevel m_minLevel = DEBUG;
    bool m_consoleEnabled = true;
    bool m_fileEnabled = true;
    bool m_initialized = false;
    QString m_logFilePath;
    
    // Session tracking
    QDateTime m_sessionStart;
    int m_logCount = 0;
};

// Convenience macros (Winston-style)
#define VOLT_TRACE(msg) VoltLogger::instance().trace(msg)
#define VOLT_DEBUG(msg) VoltLogger::instance().debug(msg)
#define VOLT_INFO(msg) VoltLogger::instance().info(msg)
#define VOLT_WARN(msg) VoltLogger::instance().warn(msg)
#define VOLT_ERROR(msg) VoltLogger::instance().error(msg)
#define VOLT_FATAL(msg) VoltLogger::instance().fatal(msg)

// Category-specific macros
#define VOLT_THEME(msg) VoltLogger::instance().theme(msg)
#define VOLT_UI(msg) VoltLogger::instance().ui(msg)
#define VOLT_EDITOR(msg) VoltLogger::instance().editor(msg)
#define VOLT_SYSTEM(msg) VoltLogger::instance().system(msg)

// Formatted logging macros (simpler approach)
#define VOLT_TRACE_F(fmt, arg1) VoltLogger::instance().log(VoltLogger::TRACE, QString(fmt).arg(arg1), "APP")
#define VOLT_DEBUG_F(fmt, arg1) VoltLogger::instance().log(VoltLogger::DEBUG, QString(fmt).arg(arg1), "APP")
#define VOLT_INFO_F(fmt, arg1) VoltLogger::instance().log(VoltLogger::INFO, QString(fmt).arg(arg1), "APP")
#define VOLT_WARN_F(fmt, arg1) VoltLogger::instance().log(VoltLogger::WARN, QString(fmt).arg(arg1), "APP")
#define VOLT_ERROR_F(fmt, arg1) VoltLogger::instance().log(VoltLogger::ERROR, QString(fmt).arg(arg1), "APP")
#define VOLT_SYSTEM_F(fmt, arg1) VoltLogger::instance().log(VoltLogger::INFO, QString(fmt).arg(arg1), "SYSTEM")
#define VOLT_THEME_F(fmt, arg1) VoltLogger::instance().log(VoltLogger::DEBUG, QString(fmt).arg(arg1), "THEME")

// Two-argument versions
#define VOLT_TRACE_F2(fmt, arg1, arg2) VoltLogger::instance().log(VoltLogger::TRACE, QString(fmt).arg(arg1).arg(arg2), "APP")
#define VOLT_DEBUG_F2(fmt, arg1, arg2) VoltLogger::instance().log(VoltLogger::DEBUG, QString(fmt).arg(arg1).arg(arg2), "APP")
#define VOLT_INFO_F2(fmt, arg1, arg2) VoltLogger::instance().log(VoltLogger::INFO, QString(fmt).arg(arg1).arg(arg2), "APP")
#define VOLT_WARN_F2(fmt, arg1, arg2) VoltLogger::instance().log(VoltLogger::WARN, QString(fmt).arg(arg1).arg(arg2), "APP")
#define VOLT_THEME_F2(fmt, arg1, arg2) VoltLogger::instance().log(VoltLogger::DEBUG, QString(fmt).arg(arg1).arg(arg2), "THEME")

// Three-argument versions
#define VOLT_TRACE_F3(fmt, arg1, arg2, arg3) VoltLogger::instance().log(VoltLogger::TRACE, QString(fmt).arg(arg1).arg(arg2).arg(arg3), "APP")
#define VOLT_DEBUG_F3(fmt, arg1, arg2, arg3) VoltLogger::instance().log(VoltLogger::DEBUG, QString(fmt).arg(arg1).arg(arg2).arg(arg3), "APP")

// Four-argument versions
#define VOLT_DEBUG_F4(fmt, arg1, arg2, arg3, arg4) VoltLogger::instance().log(VoltLogger::DEBUG, QString(fmt).arg(arg1).arg(arg2).arg(arg3).arg(arg4), "APP")
