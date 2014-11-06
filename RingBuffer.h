#pragma once
#include <stdexcept>
#include <exception>
#include <array>
template <class T,std::size_t _size>
class RingBuffer 
{
    private:
        std::array<T,_size> Buffer;
        int HEAD;
        int TAIL;
        int ioTotal;
    public:
        RingBuffer()
        {
            HEAD=0;
            TAIL=0;
            ioTotal=0;
        }
        
        size_t size()
        {
            return ioTotal;
        }

        size_t capacity()
        {
            return Buffer.size();
        }

        bool empty()
        {
            return ioTotal==0;
        }

        bool full()
        {
            return ioTotal>=capacity();
        }

        void push_back(T element)
        {
            if (full()) 
            {
                TAIL=(TAIL+1)%capacity();
                ioTotal--;
            }
            Buffer[HEAD]=element;
            HEAD=(HEAD+1)%capacity();
            ioTotal++;
        }

        T pop_front()
        {
            //if (empty()) throw(std::range_error("Buffer is empty")); //bad luck, don't do this!
            T returnval=Buffer[TAIL];
            TAIL=(TAIL+1)%capacity();
            ioTotal--;
            return returnval;
        }
        void clear()
        {
            HEAD=0;
            TAIL=0;
            ioTotal=0;
        }
        void turn(int n) //Turn the head and tail 
        {
            HEAD=(HEAD+n)%capacity();
            TAIL=(TAIL+n)%capacity();
        }
        T at(int n)
        {
           n=n%capacity(); //Don't get overflow. You shouldn't pass that big a index anyway.
           return Buffer[n];
        }
        int tail() //return tail
        {
            return TAIL;
        }
        int head() //return head
        {
            return HEAD;
        }
};

