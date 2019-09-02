
#include "log.h"


namespace mysrv{


const char* LogLevel :: ToString( LogLevel::Level level) {
    switch(level) {
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;

    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}

LogEvent::LogEvent(Logger::ptr logger, LogLevel::Level level,const char * file,int32_t line ,\
        uint32_t elapse ,uint32_t threadid,\
        uint32_t fiberid , uint64_t time ,const std::string&  threadname)
    :m_logger(logger),
    m_level(level),
    m_file(file),
    m_line(line),
    m_elapse(elapse),
    m_threadId(threadid),
    m_fiberId(fiberid),
    m_time(time),
    m_threadName(threadname) {
    }

LogEventWrap::LogEventWrap(LogEvent::ptr e)
{
    m_event = e;
}

LogEventWrap::~LogEventWrap()
{
         m_event->getLogger()->log(m_event->getLevel(),m_event);
}

std::stringstream& LogEventWrap::getSS()
{
    return m_event->getSS();
}
Logger::Logger(const std::string& name)
        :m_name(name)
        ,m_level(LogLevel::DEBUG)
{
   m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m"));
}

LogManager::LogManager()
{
    m_root.reset(new Logger);
    m_root->AddAppender(LogAppender::ptr(new StdoutLogAppender));
}

Logger::ptr LogManager::getLogger(const std::string & name){
    auto it = m_loggers.find(name);
    return it == m_loggers.end() ? m_root : it->second;
}

void LogManager::init(){

}

void Logger::AddAppender(LogAppender::ptr appender)
{
   if(!(appender->getFormatter())){
     appender->setFormatter(m_formatter);
   }
   m_appenders.push_back(appender);
}

void Logger::DelAppender(LogAppender::ptr appender)
{
    for(auto it = m_appenders.begin() ; it != m_appenders.end() ; it++)
    {
       if(*it == appender)
           m_appenders.erase(it);
           break;
    }
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
 if(level >= m_level) {
     auto self = shared_from_this();
     // MutexType::Lock lock(m_mutex);
     if(!m_appenders.empty()) {
         for(auto& i : m_appenders) {
             i->log(self, level, event);
         }
     } /*else if(m_root) {
         m_root->log(level, event);
     }*/
 }
}

void Logger::debug(LogEvent::ptr event) {
        log(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event) {
        log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event) {
        log(LogLevel::WARN, event);
}

void Logger::error(LogEvent::ptr event) {
        log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event) {
        log(LogLevel::FATAL, event);
}

FileLogAppender::FileLogAppender(const std::string &filename):m_filename(filename){
     reopen();
}

void FileLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level ,LogEvent::ptr event)
{
   if(level >= m_level)
   {
            m_filestream << m_formatter->format(logger , level ,event)  << event->getSS().str();
   }
}

bool FileLogAppender::reopen()
{
    if(m_filestream) m_filestream.close();
    m_filestream.open(m_filename);
    return m_filestream.is_open();
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level ,LogEvent::ptr event)
{
    if(level >= m_level)
    {
      std::cout << m_formatter->format(logger , level ,event) << event->getSS().str() << std::endl;
    }
}


class MessageFormatItem : public LogFormatter::FormatItem {
    public:
            MessageFormatItem(const std::string& str = "") {}
                void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                            os << event->getContent();
               }
};

class LevelFormatItem : public LogFormatter::FormatItem {
    public:
            LevelFormatItem(const std::string& str = "") {}
                void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                            os << LogLevel::ToString(level);
               }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
    public:
            ElapseFormatItem(const std::string& str = "") {}
                void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                            os << event->getElapse();
                                }
};

class NameFormatItem : public LogFormatter::FormatItem {
    public:
            NameFormatItem(const std::string& str = "") {}
                void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                            os << event->getLogger()->getName();
                                }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
    public:
            ThreadIdFormatItem(const std::string& str = "") {}
                void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                            os << event->getThreadId();
                                }
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
    public:
            FiberIdFormatItem(const std::string& str = "") {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                   os << event->getFiberId();
            }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem {
    public:
            ThreadNameFormatItem(const std::string& str = "") {}
                void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                            os << event->getThreadName();
                                }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
    :m_format(format) {
            if(m_format.empty()) {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
    struct tm tm;
    time_t time = event->getTime();
    localtime_r(&time, &tm);
    char buf[64];
    strftime(buf, sizeof(buf), m_format.c_str(), &tm);
    os << buf;
 }
    private:
    std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
    public:
            FilenameFormatItem(const std::string& str = "") {}
                void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                            os << event->getFile();
                                }
};

class LineFormatItem : public LogFormatter::FormatItem {
    public:
            LineFormatItem(const std::string& str = "") {}
                void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                            os << event->getLine();
               }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
    public:
            NewLineFormatItem(const std::string& str = "") {}
               void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                            os << std::endl;
                                }
};


class StringFormatItem : public LogFormatter::FormatItem {
    public:
            StringFormatItem(const std::string& str)
                        :m_string(str) {}
                void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                            os << m_string;
                }
    private:
         std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
    public:
        TabFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                            os << "\t";
                }
    private:
                std::string m_string;
};

LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern) {
    init();
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
	std::stringstream ss;
	for(auto &i : m_items)
	{ //"%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
		i->format(ss,logger,level,event);
	}
	return ss.str();
}

std::ostream& LogFormatter::format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
        for(auto& i : m_items) {
                    i->format(ofs, logger, level, event);
                        }
            return ofs;
}


//%xxx %xxx{xxx} %%
void LogFormatter::init() {
    //str, format, type
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                    && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
 {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}
        XX(m, MessageFormatItem),
        XX(p, LevelFormatItem),
        XX(r, ElapseFormatItem),
        XX(c, NameFormatItem),
        XX(t, ThreadIdFormatItem),
        XX(n, NewLineFormatItem),
        XX(d, DateTimeFormatItem),
        XX(f, FilenameFormatItem),
        XX(l, LineFormatItem),
        XX(T, TabFormatItem),
        XX(F, FiberIdFormatItem),
        XX(N, ThreadNameFormatItem),
#undef XX
    };

    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }

        //std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }
    //std::cout << m_items.size() << std::endl;
}

void  LOG_LEVEL(mysrv::Logger::ptr logger , mysrv::LogLevel::Level level, std::string str)
{
    if(logger->getLevel() <=  level )
    {
        std::shared_ptr< mysrv::LogEventWrap>  wraps( new LogEventWrap (LogEvent::ptr (new mysrv::LogEvent(logger , level,__FILE__, __LINE__, 0 ,\
        mysrv::GetThreadPid(),0,time(0),"sum")))) ;
        wraps->getSS()  <<  str;
    }
}

void  MYSER_LOG_DEBUG(Logger::ptr logger, std::string str) {  LOG_LEVEL(logger,mysrv::LogLevel::DEBUG ,str); }
void  MYSER_LOG_INFO   (Logger::ptr logger ,std::string str)   {  LOG_LEVEL(logger,mysrv::LogLevel::INFO ,str); }
void  MYSER_LOG_WARN(Logger::ptr logger ,  std::string str)   {  LOG_LEVEL(logger,mysrv::LogLevel::WARN ,str);}
void  MYSER_LOG_ERROR(Logger::ptr logger, std::string str)  {  LOG_LEVEL(logger,mysrv::LogLevel::ERROR ,str);}
void  MYSER_LOG_FATAL(Logger::ptr logger,  std::string str)    {  LOG_LEVEL(logger,mysrv::LogLevel::FATAL ,str);}

}
