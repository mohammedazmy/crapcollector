
#include "gc.h"
#include <stdlib.h>
#include <pthread.h>
#include <typeinfo>

void* Object::operator new(size_t size){
    rcounter* obj = (rcounter*) malloc(size + sizeof(rcounter));
    obj->count = 0x0;
    if (pthread_mutex_init(&obj->lock, NULL)) {
        throw GC_LOCK_INIT_ERR;
    }
    return obj + 1;
}

void Object::operator delete(void* ptr) {
    rcounter* obj = ((rcounter*) ptr - 1);
    pthread_mutex_destroy(&obj->lock);
    free(obj);
}

unsigned long Object::hash() const {
    return (unsigned long) this;
}

bool Object::operator ==(const Object& o) const{
    return this->hash() == o.hash();
}

string Object::toString() const {
    string str = typeid(this).name();
    return str;
}
