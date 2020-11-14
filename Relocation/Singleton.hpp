#ifndef UNISE_SINGLETON_H_
#define UNISE_SINGLETON_H_

#include <pthread.h>

/*
 * A macro to disallow the copy constructor and operator= functions
 * This should be used in the priavte:declarations for a class
 */
#define DISALLOW_COPY_AND_ASSIGN(TypeName)   \
        TypeName(const TypeName&);           \
        TypeName& operator=(const TypeName&)

namespace unise
{

    /*Thread-safe, no-manual destroy Singleton template*/
    template<typename T>
    class Singleton
    {
    public:
        /*Get the singleton instance*/
        static T* get() {
            pthread_once(&_p_once, &Singleton::_new);
            return _instance;
        }

    private:
        Singleton();
        ~Singleton();

        /*Construct the singleton instance*/
        static void _new() {
            _instance = new T();
        }

        /*
         * Destruct the singleton instance
         * Only work with gcc
         */
        __attribute__((destructor)) static void _delete() {
            typedef char T_must_be_complete[sizeof(T) == 0 ? -1 : 1];
            (void) sizeof(T_must_be_complete);
            delete _instance;
        }

        static pthread_once_t _p_once;      ///< Initialization once control
        static T*             _instance;    ///< The singleton instance

    private:
        DISALLOW_COPY_AND_ASSIGN(Singleton);
    };

    template<typename T>
    pthread_once_t Singleton<T>::_p_once = PTHREAD_ONCE_INIT;

    template<typename T>
    T* Singleton<T>::_instance = NULL;
}

#endif

/* vim:set ts=4 sw=4 expandtab */
