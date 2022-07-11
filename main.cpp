#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>

#include "stl_thread_safe.hpp"

using namespace std;



int main()
{
	thread_safe_vector<int> vec;
	thread th1{ pop<int>, ref(vec) };
	thread th2{ push<int>, ref(vec) };

	th1.join();
	th2.join();

	unique_lock<mutex> um{ mg };
	cond_global_var.wait(um, [&vec] { return vec.size() == 900; });

	for (int i = 0; i < vec.size(); ++i)
		cout << *vec[i];
}


