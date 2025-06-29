#include "VoltLogger.h"
#include <QDir>
#include <QCoreApplication>
#include <QThread>
#include <iostream>

VoltLogger& VoltLogger::instance() {
    static VoltLogger instance;
    return instance;
}

VoltLogger::~VoltLogger() {
    shutdown();
}

void VoltLogger::initialize(const QString& logFilePath, LogLevel minLevel, bool clearOnStart, bool enableConsole) {
    QMutexLocker locker(&m_mutex);
    
    if (m_initialized) {
        return; // Already initialized
    }
    
    m_logFilePath = logFilePath;
    m_minLevel = minLevel;
    m_consoleEnabled = enableConsole;
    m_fileEnabled = true;
    
    // Create logs directory if it doesn't exist
    QFileInfo fileInfo(logFilePath);
    QDir logDir = fileInfo.absoluteDir();
    if (!logDir.exists()) {
        logDir.mkpath(".");
    }
    
    // Initialize log file
    m_logFile = std::make_unique<QFile>(logFilePath);
    
    QIODevice::OpenMode openMode = QIODevice::WriteOnly | QIODevice::Text;
    if (!clearOnStart) {
        openMode |= QIODevice::Append;
    }
    
    if (m_logFile->open(openMode)) {
        m_logStream = std::make_unique<QTextStream>(m_logFile.get());
        m_initialized = true;
        
        // Start session
        startSession();
        
        if (m_consoleEnabled) {
            std::cout << "✅ VoltLogger initialized successfully" << std::endl;
            std::cout << "📁 Log file: " << logFilePath.toStdString() << std::endl;
            std::cout << "📊 Log level: " << levelToString(minLevel).toStdString() << std::endl;
        }
    } else {
        if (m_consoleEnabled) {
            std::cerr << "❌ Failed to initialize VoltLogger - cannot open log file: " 
                     << logFilePath.toStdString() << std::endl;
        }
    }
}

void VoltLogger::startSession() {
    m_sessionStart = QDateTime::currentDateTime();
    m_logCount = 0;
    
    QString sessionHeader = QString(
        "========================================================\n"
        "VOLT EDITOR LOG SESSION STARTED\n"
        "Time: %1\n"
        "Application: %2\n"
        "Working Directory: %3\n"
        "Thread: %4\n"
        "========================================================"
    ).arg(m_sessionStart.toString("yyyy-MM-dd hh:mm:ss.zzz"))
     .arg(QCoreApplication::applicationName())
     .arg(QDir::currentPath())
     .arg(QString::number(reinterpret_cast<qintptr>(QThread::currentThreadId())));
    
    if (m_logStream) {
        *m_logStream << sessionHeader << "\n";
        m_logStream->flush();
    }
    
    if (m_consoleEnabled) {
        std::cout << sessionHeader.toStdString() << std::endl;
    }
}

void VoltLogger::endSession() {
    if (!m_initialized) return;
    
    QDateTime sessionEnd = QDateTime::currentDateTime();
    qint64 duration = m_sessionStart.msecsTo(sessionEnd);
    
    QString sessionFooter = QString(
        "========================================================\n"
        "VOLT EDITOR SESSION ENDED\n"
        "End Time: %1\n"
        "Duration: %2 ms (%3 seconds)\n"
        "Total Log Entries: %4\n"
        "========================================================"
    ).arg(sessionEnd.toString("yyyy-MM-dd hh:mm:ss.zzz"))
     .arg(duration)
     .arg(duration / 1000.0, 0, 'f', 2)
     .arg(m_logCount);
    
    if (m_logStream) {
        *m_logStream << sessionFooter << "\n";
        m_logStream->flush();
    }
    
    if (m_consoleEnabled) {
        std::cout << sessionFooter.toStdString() << std::endl;
    }
}

void VoltLogger::log(LogLevel level, const QString& message, const QString& category) {
    if (!m_initialized || level < m_minLevel) {
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    
    QString formattedEntry = formatLogEntry(level, message, category);
    m_logCount++;
    
    // Write to file
    if (m_fileEnabled && m_logStream) {
        *m_logStream << formattedEntry << "\n";
        m_logStream->flush(); // Ensure immediate write
    }
    
    // Write to console
    if (m_consoleEnabled) {
        std::cout << formattedEntry.toStdString() << std::endl;
    }
}

QString VoltLogger::formatLogEntry(LogLevel level, const QString& message, const QString& category) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    QString threadId = QString::number(reinterpret_cast<qintptr>(QThread::currentThreadId()));
    
    return QString("[%1] [%2] [%3] [Thread:%4] %5")
        .arg(timestamp)
        .arg(levelStr.leftJustified(5))
        .arg(category.leftJustified(8))
        .arg(threadId.rightJustified(4))
        .arg(message);
}

QString VoltLogger::levelToString(LogLevel level) {
    switch (level) {
        case TRACE: return "TRACE";
        case DEBUG: return "DEBUG";
        case INFO:  return "INFO";
        case WARN:  return "WARN";
        case ERROR: return "ERROR";
        case FATAL: return "FATAL";
        default:    return "UNKNOWN";
    }
}

QString VoltLogger::levelToColorCode(LogLevel level) {
    switch (level) {
        case TRACE: return "\033[37m";  // White
        case DEBUG: return "\033[36m";  // Cyan
        case INFO:  return "\033[32m";  // Green
        case WARN:  return "\033[33m";  // Yellow
        case ERROR: return "\033[31m";  // Red
        case FATAL: return "\033[35m";  // Magenta
        default:    return "\033[0m";   // Reset
    }
}

// Logging methods
void VoltLogger::trace(const QString& message, const QString& category) {
    log(TRACE, message, category);
}

void VoltLogger::debug(const QString& message, const QString& category) {
    log(DEBUG, message, category);
}

void VoltLogger::info(const QString& message, const QString& category) {
    log(INFO, message, category);
}

void VoltLogger::warn(const QString& message, const QString& category) {
    log(WARN, message, category);
}

void VoltLogger::error(const QString& message, const QString& category) {
    log(ERROR, message, category);
}

void VoltLogger::fatal(const QString& message, const QString& category) {
    log(FATAL, message, category);
}

// Category-specific methods
void VoltLogger::theme(const QString& message, LogLevel level) {
    log(level, message, "THEME");
}

void VoltLogger::ui(const QString& message, LogLevel level) {
    log(level, message, "UI");
}

void VoltLogger::editor(const QString& message, LogLevel level) {
    log(level, message, "EDITOR");
}

void VoltLogger::system(const QString& message, LogLevel level) {
    log(level, message, "SYSTEM");
}

// Configuration methods
void VoltLogger::setLogLevel(LogLevel level) {
    QMutexLocker locker(&m_mutex);
    m_minLevel = level;
    info(QString("Log level changed to: %1").arg(levelToString(level)), "CONFIG");
}

void VoltLogger::enableConsoleOutput(bool enable) {
    QMutexLocker locker(&m_mutex);
    m_consoleEnabled = enable;
}

void VoltLogger::enableFileOutput(bool enable) {
    QMutexLocker locker(&m_mutex);
    m_fileEnabled = enable;
}

QString VoltLogger::getRecentLogs(int lines) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_logFile || !m_logFile->exists()) {
        return "No log file available";
    }
    
    // Read the file and return last N lines
    QFile file(m_logFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "Cannot read log file";
    }
    
    QStringList allLines;
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        allLines.append(stream.readLine());
    }
    
    // Return last N lines
    int startIndex = qMax(0, allLines.size() - lines);
    QStringList recentLines = allLines.mid(startIndex);
    
    return recentLines.join("\n");
}

void VoltLogger::shutdown() {
    if (!m_initialized) return;
    
    endSession();
    
    if (m_logStream) {
        m_logStream->flush();
        m_logStream.reset();
    }
    
    if (m_logFile) {
        m_logFile->close();
        m_logFile.reset();
    }
    
    m_initialized = false;
}
