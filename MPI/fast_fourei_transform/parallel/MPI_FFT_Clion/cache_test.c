//
// Created by Zelin on 20/05/06.
//

#include <random>
#include <iostream>
#include <ctime>
#include <algorithm>

#define  ull unsigned long long
#define KB(x) ((ull) (x) << 10)
#define len 18
using namespace std;


int main()
{
    //需要测试的数组大小
    ull sizes_KB[len];
    int i=0;
    for(i=0;i<len;i++){
        size_KB[i] = 1 << i;
    }



    for (size_t size: sizes_KB)
    {
        
        uniform_int_distribution<> dis(0, KB(size)-1);
        vector<char> memory(KB(size));//创建一个指定大小的连续内存块
        fill(memory.begin(), memory.end(), 1);

        int dummy = 0;//用这个变量避免编译器对下面的循环进行优化

        //在内存块上进行大量的随机访问，并计时
        clock_t begin = clock();
        for (int i = 0; i < (1<<25); i++){dummy += memory[dis(gen)];}
        clock_t end = clock();

        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        cout << size << " KB, " << elapsed_secs << "secs, dummy:" << dummy << endl;
    }
    return 0;
}