#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>

using namespace std;

template<typename T>
class thread_safe_vector
{
public:
	thread_safe_vector() = default;
	thread_safe_vector(const thread_safe_vector& other)
	{
		lock_guard<mutex> lm{ m };
		safe_vector = other;
	}
	void push_back(T& value)
	{
		lock_guard<mutex> lm{ m };
		safe_vector.push_back(value);
		cond_var.notify_one();
	}
	T pop_back()
	{
		unique_lock<mutex> um{ m };
		cond_var.wait(um, [this] { return !safe_vector.empty(); });
		T val = safe_vector.back();
		safe_vector.pop_back();
		return val;
	}
private:
	vector<T> safe_vector{};
	mutex m{};
	condition_variable cond_var{};
};

template<typename T>
void push(thread_safe_vector<T>& vec)
{
	for (int i = 0; i < 1000; ++i)
		vec.push_back(i);
}

template<typename T>
void pop(thread_safe_vector<T>& vec)
{
	for (int i = 0; i < 1000; ++i)
		vec.pop_back();
}

int main()
{
	thread_safe_vector<int> vec;
	thread th1{ pop<int>, ref(vec) };
	thread th2{ push<int>, ref(vec) };

	th1.join();
	th2.join();

}


