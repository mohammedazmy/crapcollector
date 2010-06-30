
#include "gc.h"
#include <stdlib.h>
#include <pthread.h>

void* Object::operator new(size_t size){
    rcounter* obj = (rcounter*) malloc(size + sizeof(rcounter));
    obj->count = 0x0;
    if (pthread_mutex_init(&obj->lock, NULL)) {
        throw GC_LOCK_INIT_ERR;
    }
    long paddress = (long) obj;
    return (void*) (paddress + sizeof(rcounter));
}

void Object::operator delete(void* ptr) {
    rcounter* obj = (rcounter*) ptr - sizeof(rcounter);
    pthread_mutex_destroy(&obj->lock);
    free(obj);
}
