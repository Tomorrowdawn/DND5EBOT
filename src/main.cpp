/*
extern "C"{
    #include "dice.h"
    extern Dice_command* parse(const char *input_path);
}*/

#define __STAR
#ifdef __STAR

#include "MiraiCP.hpp"
using namespace MiraiCP;
using namespace std;
#include <chrono>
//#include "dice_analyzer.hpp"
#include "command.hpp"
#include <iostream>

template<class T>
inline string get_message(T msg){
    //随版本变化
    //version 2.9.0
    return msg.message.toMiraiCode();
}

// 插件实例
class Main : public CPPPlugin {
   public:
    // 配置插件信息
    Main() : CPPPlugin(PluginConfig("ID114514", "HACHI ROKU", "0.1", "tomorrowdawn", "DND5E BOT", "22/2/9")) {}
    void onEnable() override {
        srand(chrono::system_clock::to_time_t(chrono::system_clock::now()));
        //设置种子。
        // 监听
        Event::processor.registerEvent<BotOnlineEvent>([this](BotOnlineEvent task)->void {
            botid = task.bot.id;
            Logger::logger.info<string>("Bot id="+to_string(botid));
            //auto HELLO = Friend(Master,botid);
            //HELLO.sendMessage<string>("HELLO,WORLD!");
        });
        Event::processor.registerEvent<GroupMessageEvent>([this](GroupMessageEvent task){
            auto sendid = task.sender.groupid();
            auto message = trim(get_message(task));
            if(message.find(".",0)!=0 && message.find("。",0)!=0){
                return;//忽略
            }
            string sep;
            if(message.find(".",0)==0)
                sep = ".";
            else 
                sep = "。";
            Logger::logger.info(message);
            message = message.substr(sep.length());
            transform(message.begin(),message.end(),message.begin(),::toupper);
            //全大写处理
            string answer;
            try{
                answer = DND5E::Parser::get_parse(message);
            }catch(const char* msg){
                answer = string(msg);
            }catch(...){
                answer = "未知错误!";
            }
            task.group.sendMessage(answer);
            //Logger::logger.info<MessageChain>(message);
        });
    }

    void onDisable() override {
        /*插件结束*/
    }
    inline QQID BOTID() { return botid; }
    inline string BOTNAME() { return config.name; }
   private:
    QQID Master = 326598617;
    QQID botid;
};

// 绑定当前插件实例
void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin0(new Main());
}

#endif