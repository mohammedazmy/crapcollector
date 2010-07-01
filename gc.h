
#if !defined GC_H
#define GC_H
#include <stdlib.h>
#include <pthread.h>

#define DEBUG 0

#if DEBUG
#include <iostream>
using namespace std;
#endif

#include <string>
using namespace std;

static const int GC_LOCK_INIT_ERR = 1000;
static const int GC_LOCK_AQUIRE_ERR = 1001;
static const int GC_LOCK_RELEASE_ERR = 1002;
static const int GC_TYPE_ERR = 1003;
static const int GC_NULL_POINTER_ERR = 1004;

struct rcounter {
    long count;
    pthread_mutex_t lock;
};

class Object {
    public:
        static void* operator new(size_t size);
        static void operator delete(void* ptr);
        
        virtual unsigned long hash() const;
        virtual bool operator ==(const Object& o) const;
        virtual string toString() const;
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
        bool operator ==(const $<T>& o);
        bool operator ==(const T* o);
        operator T*();
    private:
        T* ptr;
        void inc();
        void dec();
};


template <class T>
$<T>::$(T* ptr){
    if (!dynamic_cast<Object*>(ptr)){
        throw GC_TYPE_ERR;
    }
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
    if (!this->ptr) {
        throw GC_NULL_POINTER_ERR;
    }
    return this->ptr;
}

template <class T>
$<T>::operator T*(){
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
bool $<T>::operator ==(const $<T>& o){
    if (this->ptr){
        return this->ptr->operator==(*o.ptr);
    } else {
        return this->ptr == o.ptr;
    }
}

template <class T>
bool $<T>::operator ==(const T* o){
    if (this->ptr) {
        return this->ptr->operator==(*o);
    } else {
        return this->ptr == o;
    }
}
        
template <class T>
void $<T>::inc() {
    rcounter* rcount = ((rcounter*) this->ptr) - 1;
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
    rcounter* rcount = ((rcounter*) this->ptr) - 1;
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



#endif
