
#include<stdio.h>
#include<stdlib.h>
//#include<mpi.h>
#include<time.h>
#include<malloc.h>
#define ull unsigned long long 

// 根节点打印堆上的指针占用空间
void print_root_memory_used(void* ptr, int my_rank) {
	if (my_rank != 0) return;
	ull bytes = _msize(ptr);
	ull KB = 1024;
	ull MB = 1024 * 1024;
	ull GB = 1024 * 1024 * 1024;
	if (bytes <= KB) {
		printf("pointer size is %lld Bytes \n", bytes);
	}
	else if (bytes > KB && bytes <= (MB)) {
		printf("pointer size is %lld KBytes \n", bytes / KB);
	}
	else if (bytes > MB && bytes <= (GB)) {
		printf("pointer size is %lld MBytes \n", bytes / MB);
	}
	else if (bytes > GB) {
		printf("pointer size is %lld GBytes \n", bytes / GB);
	}
	return;



}
#define type int
int main(int argc, char* argv[])
{
	
	type* a = (type*)malloc(sizeof(type) * 102400000);
	print_root_memory_used(a, 0);
	return 0;
}
