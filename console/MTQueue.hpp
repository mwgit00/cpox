#ifndef MT_QUEUE_HPP_
#define MT_QUEUE_HPP_

#include <queue>
#include <mutex>

template <typename T>
class MTQueue
{
public:
    
    MTQueue() {}
    virtual ~MTQueue() {};

    size_t size(void) const;
    bool empty(void) const;
    void clear(void);
    void push(const T& x);
    T pop(void);

private:
    
    std::mutex mx;
    std::queue<T> mq;
};

template <typename T>
size_t MTQueue<T>::size(void) const
{
    return mq.size();
}

template <typename T>
bool MTQueue<T>::empty(void) const
{
    return mq.empty();
}

template <typename T>
void MTQueue<T>::clear(void)
{
    std::lock_guard<std::mutex> lock(mx);
    mq = {};
}

template <typename T>
void MTQueue<T>::push(const T& x)
{
    std::lock_guard<std::mutex> lock(mx);
    mq.push(x);
}

template <typename T>
T MTQueue<T>::pop(void)
{
    std::lock_guard<std::mutex> lock(mx);
    T val = mq.front();
    mq.pop();
    return val;
}

#endif // MT_QUEUE_HPP_
