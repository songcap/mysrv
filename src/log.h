
#ifndef __LOG_H__
#define __LOG_H__
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <list>
#include <functional>
#include <tuple>
#include <sstream>
#include <fstream>
#include <map>
#include <ostream>


namespace mysrv{

class Logger;

/****************
 * LogLevel:日志等级
 * 划分日志的等级
 * *************/
class LogLevel {
  public:
    enum Level{
      UNKNOW = 0,
      DEBUG,
      INFO,
      WARN,
      ERROR,
      FATAL
	};
	static const char* ToString(LogLevel::Level level);
};

/**************
 * LogEvent: 日志事件 所有实际的日志内容的集合
 * 内部类 ：LogItem 接口需要被实现
 * 主要功能：
 * 每一条日志信息都是一个event
 * ************/
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, const char * file, int32_t m_line ,uint32_t elapse, uint32_t threadid , uint32_t fiberid , uint64_t time,const std::string &threadname);

    const char * getFile() const { return m_file;}
    int32_t getLine() const { return m_line; }    //行号
    uint32_t getElapse() const { return  m_elapse; } //程序启动到现在开始的毫秒数
    uint32_t getThreadId() const { return m_threadId; }
    uint32_t getFiberId() const{ return m_fiberId; }
    uint32_t getTime() const { return  m_time; }
	std::stringstream& getSS() { return m_ss;}
	
	const std::string& getThreadName() const { return m_threadName;}
    
	std::shared_ptr<Logger> getLogger() const { return m_logger;}

	std::string getContent() const{ return m_content; }//内容 
    
private:
    /// 日志器
    std::shared_ptr<Logger> m_logger;
    const char * m_file;
    int32_t m_line;    //行号
    uint32_t m_elapse; //程序启动到现在开始的毫秒数
    uint32_t m_threadId;
    uint32_t m_fiberId;
    uint32_t m_time;
	/// 线程名称
    std::string m_threadName;

	std::string m_content;//内容  
	    /// 日志内容流
    std::stringstream m_ss;
   
}; 

/**************
 * LogFormatter : 日志打印的格式
 * 内部类：FormatItem
 * 主要就是各种不同的变量打印的格式不一样，所以需要分开处理
 * 初始化解析日志的内容并按照各种格式显示
 **************/

class LogFormatter{
public:
  typedef std::shared_ptr<LogFormatter> ptr;  
  LogFormatter(const  std::string & pattern);
  virtual ~LogFormatter(){}

   std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
   std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
            
  class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
		FormatItem(const std::string& fmt = ""){};
        virtual ~FormatItem(){}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
  };
  void init();
private:
  std::string m_pattern;
  std::vector<FormatItem::ptr> m_items;
  bool m_error = false; 

};

/***************
 * LogAppender : 日志输出地
 * 日志输出的地方不同，接口类
 * stdout 
 * file 
 * ...
 * *************/
class LogAppender{
public:
   typedef std::shared_ptr< LogAppender > ptr; 
   LogAppender(){};

   virtual  void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) = 0;
   void setFormatter(LogFormatter::ptr ptr) { m_formatter = ptr; }
   LogFormatter::ptr getFormatter() const  { return m_formatter; }
protected:
  LogLevel::Level m_level;
  LogFormatter::ptr m_formatter;
};

/***********
 * Logger日志所有的功能的实现模块
 * 指定日志等级
 * 处理event
 * 设置显示格式
 *
 * 成员：
 * 包含appender appender内部使用formatter显示
 * **********/

class Logger : public std::enable_shared_from_this<Logger>{
public:
  typedef std::shared_ptr<Logger> ptr;

  Logger(const std::string & name = "root");
  
  void log(LogLevel::Level level,LogEvent::ptr event);
  void debug(LogEvent::ptr level);
  void info(LogEvent::ptr level);
  void fatal(LogEvent::ptr level);
  void error(LogEvent::ptr level);
  void warn(LogEvent::ptr level);
  
  void AddAppender(LogAppender::ptr appender);
  void DelAppender(LogAppender::ptr appender);
   
  std::string getName() const { return m_name; }
  void setLevel(LogLevel::Level level) { m_level = level; }
  LogLevel::Level getLevel()const {return m_level;}

private:
  std::string m_name;    //日志名称
  LogLevel::Level m_level; //日志级别 
  std::list<LogAppender::ptr> m_appenders; //Appender的集合       
  LogFormatter::ptr m_formatter;
};

class StdoutLogAppender : public LogAppender{
public:
  typedef std::shared_ptr<StdoutLogAppender > ptr;
  StdoutLogAppender(){}
  void log(std::shared_ptr<Logger> logger,LogLevel::Level level ,LogEvent::ptr event)override;
  private:
};

class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string &filename);
    
	bool reopen();
    void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event)override;
private:
   std::string m_filename;
   std::ofstream m_filestream;
};

}


#endif /*__LOG_H__*/



