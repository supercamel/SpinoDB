#ifndef OBJECT_POOL_ALLOCATOR_H
#define OBJECT_POOL_ALLOCATOR_H

#include <new>
#include <cstdint>
#include <vector>
#include <cstddef>

//#define SPINO_USE_SYSTEM_ALLOC

template <typename T, size_t growSize>
class ObjectAllocator
{
public:
    ObjectAllocator() : bufferedBlocks(0) {}

    ~ObjectAllocator()
    {
        while (firstBuffer)
        {
            Buffer *buffer = firstBuffer;
            firstBuffer = buffer->next;
            delete buffer;
        }
        // Optionally: assert(n_allocated == 0 && "Objects still in use");
    }

    template <class... U>
    T *make(U &&...u)
    {
#ifndef SPINO_USE_SYSTEM_ALLOC
        void *ptr = alloc();
        try {
            T *t = new (ptr) T(std::forward<U>(u)...);
            return t;
        } catch (...) {
            free(ptr);
            throw;
        }
#else 
        return new T(std::forward<U>(u)...);
#endif
    }

    void delete_object(T *obj_ptr)
    {
#ifndef SPINO_USE_SYSTEM_ALLOC
        obj_ptr->~T();
        free(obj_ptr);
#else
        delete obj_ptr;
#endif
    }

    void *alloc()
    {
        if (firstFreeBlock)
        {
            Block *block = firstFreeBlock;
            firstFreeBlock = block->next;
            n_allocated++;
            return reinterpret_cast<T *>(block);
        }

        if (bufferedBlocks >= growSize || !firstBuffer)
        {
            firstBuffer = new Buffer(firstBuffer);
            bufferedBlocks = 0;
        }

        n_allocated++;
        return firstBuffer->getBlock(bufferedBlocks++);
    }

    void free(void *pointer)
    {
        Block *block = reinterpret_cast<Block *>(pointer);
        block->next = firstFreeBlock;
        firstFreeBlock = block;
        n_allocated--;
    }

    size_t get_n_allocations() const
    {
        return n_allocated;
    }

private:
    struct Block
    {
        Block *next;
    };

    class Buffer
    {
        static const std::size_t blockSize =
            sizeof(T) > sizeof(Block) ? sizeof(T) : sizeof(Block);
        alignas(T) uint8_t data[blockSize * growSize];

    public:
        Buffer *const next;

        Buffer(Buffer *next) : next(next) {}

        void *getBlock(std::size_t index)
        {
            return (void *)&data[blockSize * index];
        }
    };

    Block *firstFreeBlock = nullptr;
    Buffer *firstBuffer = nullptr;
    std::size_t bufferedBlocks;
    size_t n_allocated = 0;
};

#endif
