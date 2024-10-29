#ifndef __jackos_memorymanagement_H
#define __jackos_memorymanagement_H

#include <common/types.h>

namespace jackos {
    struct MemoryChunk {
        MemoryChunk* next;
        MemoryChunk* prev;
        bool allocated;
        jackos::common::size_t size;
    };

    class MemoryManager {
        private:
            MemoryChunk* first;
        public:
            static MemoryManager* activeMemoryManager;
            MemoryManager(jackos::common::size_t start, jackos::common::size_t size);
            ~MemoryManager();

            void* malloc(jackos::common::size_t size);
            void free(void* ptr);
    };
}

void* operator new(unsigned size);
void* operator new[](unsigned size);
void* operator new(unsigned size, void* ptr);
void* operator new[](unsigned size, void* ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);

#endif