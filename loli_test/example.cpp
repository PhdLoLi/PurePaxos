#include <iostream>
#include <vector>
#include <chrono>
#include <string>

#include "ThreadPool.h"

std::string test(int i) {
  std::this_thread::sleep_for(std::chrono::seconds(10));
  std::cout <<  i << std::endl;
  return std::to_string(i*i);
}

int main()
{
    
    ThreadPool pool(4);
    std::vector< std::future<std::string> > results;

    for(int i = 0; i < 8; ++i) {
        results.emplace_back(pool.enqueue(test, i));
    }

    std::cout << "HERE" << std::endl;

    for(auto && result: results)
        std::cout <<"result:" << result.get() << std::endl;
    
    return 0;
}
