
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
#include "util.h"
#include "singleton.h"

#define MYSRV_LOG_ROOT mysrv::LoggerManager::GetInstance()->getRoot()

namespace mysrv{

class Logger;

/****************
 * LogLevel:鏃ュ織绛夌骇
 * 鍒掑垎鏃ュ織鐨勭瓑绾�
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
 * LogEvent: 鏃ュ織浜嬩欢 鎵�鏈夊疄闄呯殑鏃ュ織鍐呭�圭殑闆嗗悎
 * 鍐呴儴绫� 锛歀ogItem 鎺ュ彛闇�瑕佽��瀹炵幇
 * 涓昏�佸姛鑳斤細
 * 姣忎竴鏉℃棩蹇椾俊鎭�閮芥槸涓�涓猠vent
 * ************/
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level,  const char * file, int32_t m_line ,uint32_t elapse, uint32_t threadid , uint32_t fiberid , uint64_t time,const std::string &threadname);

    const char * getFile() const { return m_file;}
    int32_t getLine() const { return m_line; }    //琛屽彿
    uint32_t getElapse() const { return  m_elapse; } //绋嬪簭鍚�鍔ㄥ埌鐜板湪寮�濮嬬殑姣�绉掓暟
    uint32_t getThreadId() const { return m_threadId; }
    uint32_t getFiberId() const{ return m_fiberId; }
    uint32_t getTime() const { return  m_time; }
	std::stringstream & getSS() { return m_ss;}
    LogLevel::Level   getLevel()  const  { return m_level;}
	const std::string& getThreadName() const { return m_threadName;}

	std::shared_ptr<Logger> getLogger() const { return m_logger;}

	std::string getContent() const{ return m_content; }//鍐呭��

private:
    /// 鏃ュ織鍣�
    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
    const char * m_file;
    int32_t m_line;    //琛屽彿
    uint32_t m_elapse; //绋嬪簭鍚�鍔ㄥ埌鐜板湪寮�濮嬬殑姣�绉掓暟
    uint32_t m_threadId;
    uint32_t m_fiberId;
    uint32_t m_time;
	/// 绾跨▼鍚嶇О
    std::string m_threadName;

	std::string m_content;//鍐呭��
	    /// 鏃ュ織鍐呭�规祦
    std::stringstream m_ss;

};

class LogEventWrap{
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();

    std::stringstream &getSS();
private:
    LogEvent::ptr m_event;
};


/**************
 * LogFormatter : 鏃ュ織鎵撳嵃鐨勬牸寮�
 * 鍐呴儴绫伙細FormatItem
 * 涓昏�佸氨鏄�鍚勭�嶄笉鍚岀殑鍙橀噺鎵撳嵃鐨勬牸寮忎笉涓�鏍凤紝鎵�浠ラ渶瑕佸垎寮�澶勭悊
 * 鍒濆�嬪寲瑙ｆ瀽鏃ュ織鐨勫唴瀹瑰苟鎸夌収鍚勭�嶆牸寮忔樉绀�
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
 * LogAppender : 鏃ュ織杈撳嚭鍦�
 * 鏃ュ織杈撳嚭鐨勫湴鏂逛笉鍚岋紝鎺ュ彛绫�
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

  void setLevel(LogLevel::Level  level){m_level = level;}
  LogLevel::Level getLevel() const {return m_level;}
protected:
  LogLevel::Level m_level;
  LogFormatter::ptr m_formatter;
};

/***********
 * Logger鏃ュ織鎵�鏈夌殑鍔熻兘鐨勫疄鐜版ā鍧�
 * 鎸囧畾鏃ュ織绛夌骇
 * 澶勭悊event
 * 璁剧疆鏄剧ず鏍煎紡
 *
 * 鎴愬憳锛�
 * 鍖呭惈appender appender鍐呴儴浣跨敤formatter鏄剧ず
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
  std::string m_name;    //鏃ュ織鍚嶇О
  LogLevel::Level m_level; //鏃ュ織绾у埆
  std::list<LogAppender::ptr> m_appenders; //Appender鐨勯泦鍚�
  LogFormatter::ptr m_formatter;
};
/***********
管理所有的logger
以及负责读取配置文件内部的信息
************/
class LogManager{
public:
    LogManager();
    Logger::ptr getLogger(const std::string & name);

    void init();
    Logger::ptr getRoot() const {return m_root;}
private:
    std::map<std::string , Logger::ptr> m_loggers;
    Logger::ptr m_root;
};

typedef mysrv::Singleton<LogManager> LoggerManager;

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

void LOG_LEVEL(mysrv::Logger::ptr logger , mysrv::LogLevel::Level level ,  std::string str);

void  MYSER_LOG_DEBUG(Logger::ptr logger, std::string str);
void MYSER_LOG_INFO   (Logger::ptr logger,std::string str);
void MYSER_LOG_WARN(Logger::ptr logger, std::string str);
void  MYSER_LOG_ERROR(Logger::ptr logger , std::string str);
void  MYSER_LOG_FATAL(Logger::ptr logger,  std::string str)  ;

}




#endif /*__LOG_H__*/
