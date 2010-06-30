
#if !defined GC_H
#define GC_H
#include <stdlib.h>
#include <pthread.h>

#define DEBUG 1

#if DEBUG
#include <iostream>
using namespace std;
#endif

static const int GC_LOCK_INIT_ERR = 1000;
static const int GC_LOCK_AQUIRE_ERR = 1001;
static const int GC_LOCK_RELEASE_ERR = 1002;

struct rcounter {
    long count;
    pthread_mutex_t lock;
};

template <class T>
class $ {
    public:
        $();
        $(T* ptr);
        $(const $<T>& o);
        ~$();
        T* operator ->();
        T* operator =(const $<T>& o);
    private:
        T* ptr;
        void inc();
        void dec();
};


template <class T>
$<T>::$(T* ptr){
    this->ptr = ptr;
    this->inc();
}

template <class T>
$<T>::$(){
    this->ptr = 0x0;
    //initialize counter.
}

template <class T>
$<T>::$(const $<T>& o){
    this->ptr = o.ptr;
    if (this->ptr != 0x0){
        this->inc();
    }
}

template <class T>
$<T>::~$(){
    if (this->ptr){
        this->dec();
    }
}

template <class T>
T* $<T>::operator ->(){
    return this->ptr;
}


template <class T>
T* $<T>::operator =(const $<T>& o){
    if (this->ptr != 0x0){
        this->dec();
    }
    this->ptr = o.ptr;
    if (this->ptr != 0x0){
        this->inc();
    }
}


template <class T>
void $<T>::inc() {
    rcounter* rcount = (rcounter*) (this->ptr - sizeof(rcounter));
    if (pthread_mutex_lock(&rcount->lock))
        throw GC_LOCK_AQUIRE_ERR;
    
    #if DEBUG
    cout << "Increment: " << this->ptr << endl;
    #endif
        
    rcount->count ++;
    
    if (pthread_mutex_unlock(&rcount->lock))
        throw GC_LOCK_RELEASE_ERR;
}

template <class T>
void $<T>::dec() {
    rcounter* rcount = (rcounter*) (this->ptr - sizeof(rcounter));
    if (pthread_mutex_lock(&rcount->lock))
        throw GC_LOCK_AQUIRE_ERR;
    
    #if DEBUG
    cout << "Decrement: " << this->ptr << endl;
    #endif
    
    if (--rcount->count == 0){
        #if DEBUG
        cout << "Destruct: " << this->ptr << endl;
        #endif
        
        if (pthread_mutex_unlock(&rcount->lock))
            throw GC_LOCK_RELEASE_ERR;
            
        delete this->ptr;
    } else {
        if (pthread_mutex_unlock(&rcount->lock))
            throw GC_LOCK_RELEASE_ERR;
    }
}

class Object {
    public:
        static void* operator new(size_t size);
        static void operator delete(void* ptr);
};

#endif
