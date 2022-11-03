#ifndef OBJECT_POOL_ALLOCATOR_H
#define OBJECT_POOL_ALLOCATOR_H

#include <new>
#include <cstdint>
#include <vector>

/*
A ObjectAllocator reserves a block of memory and allocates objects from it.
The memory is created as an array, and the objects are allocated from the array.
It has no memory overhead for the objects, but it has a fixed size.
*/
template <typename T, size_t growSize>
class ObjectAllocator
{
public:
    ObjectAllocator()
    {
    }

    ~ObjectAllocator()
    {
        while (firstBuffer)
        {
            Buffer *buffer = firstBuffer;
            firstBuffer = buffer->next;
            delete buffer;
        }
    }

    template <class... U>
    T *make(U &&...u)
    {
        void *ptr = alloc();
        T *t = new (ptr) T(std::forward<U>(u)...);
        return t;
        //return new T(std::forward<U>(u)...);
    }

    void delete_object(T *obj_ptr)
    {
        obj_ptr->~T();
        free(obj_ptr);
        //delete obj_ptr;
    }

    void *alloc()
    {
        if (firstFreeBlock)
        {
            Block *block = firstFreeBlock;
            firstFreeBlock = block->next;
            return reinterpret_cast<T *>(block);
        }

        if (bufferedBlocks >= growSize)
        {
            firstBuffer = new Buffer(firstBuffer);
            bufferedBlocks = 0;
        }

        return firstBuffer->getBlock(bufferedBlocks++);
        //return malloc(sizeof(T));
    }

    void free(void *pointer)
    {
        Block *block = reinterpret_cast<Block *>(pointer);
        block->next = firstFreeBlock;
        firstFreeBlock = block;
       //::free(pointer);
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
        uint8_t data[blockSize * growSize];

    public:
        Buffer *const next;

        Buffer(Buffer *next) : next(next)
        {
        }

        void *getBlock(std::size_t index)
        {
            return (void *)&data[blockSize * index];
        }
    };

    Block *firstFreeBlock = nullptr;
    Buffer *firstBuffer = nullptr;
    std::size_t bufferedBlocks = growSize;

    /*
            union Block
            {
                Block *next;
                uint8_t space[sizeof(T)];
            };

            std::vector<Block*> block_list;
            */
    size_t n_allocated = 0;

    // Block *free_head;
};

#endif 
