//
// Created by Anshul Gowda on 2/24/24.
//

#ifndef ODYSSEYGAMESERVER_THREADSAFEDATA_H
#define ODYSSEYGAMESERVER_THREADSAFEDATA_H

#include <shared_mutex>

using namespace std;

class ThreadSafeData {
public:
    ThreadSafeData() = default;
    virtual ~ThreadSafeData() = default;

    // delete both the copy constructor and copy assignment operator to prevent copying
    ThreadSafeData(const ThreadSafeData&) = delete;
    ThreadSafeData& operator=(const ThreadSafeData&) = delete;

protected:
    mutable shared_mutex mutex;

    template<typename Action>
    auto readAction(Action action) const -> decltype(action()) {
        shared_lock<shared_mutex> lock(mutex);
        return action();
    }

    template <typename Action>
    void writeAction(Action action) {
        unique_lock<shared_mutex> lock(mutex);
        action();
    }
};


#endif //ODYSSEYGAMESERVER_THREADSAFEDATA_H
