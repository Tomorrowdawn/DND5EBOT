#ifndef _DND5E_FACTORY
#define _DND5E_FACTORY

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
//#include <cxxabi.h>

/*
std::string demangle(const char *name) {
#ifdef __CXXABI
    int status = -4;  // some arbitrary value to eliminate the compiler warning

    std::unique_ptr<char, void (*)(void *)> res{
        abi::__cxa_demangle(name, NULL, NULL, &status), std::free};

    return (status == 0) ? res.get() : name;

#else
    if (strcmp(name, "3Dog") == 0)
        return "Dog";
    else if (strcmp(name, "3Cat") == 0)
        return "Cat";
    return "";
#endif
}
*/

/*Acknowledgement:
This code thanks to this excellent blog: http://www.nirfriedman.com/2018/04/29/unforgettable-factory/
There are also many coool articles on Nir's blog.
I name this .hpp as magic.hpp for respect of the masterpiece.
tomorrowdawn 22/2/13
*/

//PS:I've read this hpp many times and commented in Chinese.
//If you're familiar with English, you can access url above.
//PS2:For better programming,I've changed some part of source file.

namespace DND5E{
std::string name_match(const std::string& input,std::vector<std::string>& cmds);
}

template <class Base, class... Args>
class Factory {
   public:
    template <class... T>
    static std::unique_ptr<Base> make(const std::string &s, T &&...args) {
        return data().at(s)(std::forward<T>(args)...);
    }

    template <class... T>
    static std::unique_ptr<Base> fetch(const std::string&s,T&& ...args){
        static std::vector<std::string> cmds = Base::commands();
        std::string head;
        try{
            head = DND5E::name_match(s,cmds);
        }catch(...){
            throw "command no match.";
        }
        //std::cout << "head:" + head <<std ::endl; 
        return make(head,std::forward<T>(args)...);
    }

    template <class T>
    struct Registrar : Base {
        friend T;

        static bool registerT() {
            const auto name = T::name;
            Factory::data()[name] = [](Args... args) -> std::unique_ptr<Base> {
                return std::make_unique<T>(std::forward<Args>(args)...);
            };
            Factory::cmds().push_back(name);
            return true;
        }
        static bool registered;
        //特别注意,class sub:public base::Registrar<sub>的写法只能注册sub,不能注册sub的子类.
        //实际上,sub的子类是无论如何也注册不了的(由于静态变量在父类和派生类中一致)
        //如果有子类注册需求,请在你的sub类中同样实现一个注册商Registrar即可.
        //注意不要忘了外部初始化静态变量.

       private:
        Registrar() : Base(Key{}) { (void)registered; }
        //此部分要求,任何直接继承Base的子类都必须提供一个Key.
        //然而Key只能在Base下面访问到,这就意味着子类不能直接继承Base
        //附:class Base:Factory<Base,anytype>将指明Factory内的Base.也即friend声明的.
        //然而class sub:Base则不会拥有这个访问权限.这一点是标准明确指出的,友谊不可继承也不可传递.
    };

    friend Base;

   private:
    class Key {
        Key(){};
        template <class T>
        friend struct Registrar;
    };
    using FuncType = std::unique_ptr<Base> (*)(Args...);
    Factory() = default;

    static auto &data() {
        static std::unordered_map<std::string, FuncType> s;
        return s;
    }
    static std::vector<std::string> &cmds(){
        static std::vector<std::string> cmd;
        return cmd;
    }
};

template <class Base, class... Args>
template <class T>
bool Factory<Base, Args...>::Registrar<T>::registered =
    Factory<Base, Args...>::Registrar<T>::registerT();

#endif