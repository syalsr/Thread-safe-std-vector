#pragma once
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

template<typename T>
class thread_safe_vector
{
public:
    thread_safe_vector() = default;
    thread_safe_vector(const thread_safe_vector& other)
    {
        std::lock_guard<std::mutex> lm{ m };
        safe_vector = other;
    }
    void push_back(T& value)
    {
        std::lock_guard<std::mutex> lm{ m };
        safe_vector.push_back(value);
        cond_var.notify_one();
    }
    T pop_back()
    {
        std::unique_lock<std::mutex> um{ m };
        cond_var.wait(um, [this] { return !safe_vector.empty(); });
        T val = safe_vector.back();
        safe_vector.pop_back();
        return val;
    }
    std::iterator<std::random_access_iterator_tag, T> begin() { return safe_vector.begin(); }
    std::iterator<std::random_access_iterator_tag, T> end() { return safe_vector.end(); }
    size_t size() { return safe_vector.size(); }
    T* operator[](int i)
    {
        return safe_vector.data() + i;
    }
private:
    std::vector<T> safe_vector{};
    std::mutex m{};
    std::condition_variable cond_var{};
};

std::condition_variable cond_global_var{};
std::mutex mg;
template<typename T>
void push(thread_safe_vector<T>& vec)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    for (int i = 0; i < 1000; ++i)
        vec.push_back(i);
    cond_global_var.notify_one();
}

template<typename T>
void pop(thread_safe_vector<T>& vec)
{
    for (int i = 0; i < 100; ++i)
        vec.pop_back();
}

void examle()
{
    thread_safe_vector<int> vec;
    std::thread th1{ pop<int>, std::ref(vec) };
    std::thread th2{ push<int>, std::ref(vec) };

    th1.join();
    th2.join();

    std::unique_lock<std::mutex> um{ mg };
    cond_global_var.wait(um, [&vec] { return vec.size() == 900; });

    for (int i = 0; i < vec.size(); ++i)
        std::cout << *vec[i];
}