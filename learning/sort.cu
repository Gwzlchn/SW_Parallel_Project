#include <stdio.h>
#include <time.h>
#define N 10

float max(float *timer, int n){
	int i = 0;
	float maxTimer=0.0;
	for( ; i < n ; i ++){
		if(timer[i] > maxTimer)maxTimer = timer[i];
	}
	return maxTimer;
}

void flush(float *a, int n){
	int i = 0;
	for( ; i < n ; i++){
		a[i] = 0.0;
	}
}

float sum(float *a, int n){
	float s = 0.0;
	int i = 0;
	for( ; i < N ; i++){
		s += a[i];
	}
	return s;	
}


__global__ void Sort(float *a, int n, float *ts, float *tx, int *signal){
	clock_t start0, finish0;
	clock_t start1, finish1;
	clock_t start2, finish2;

	int blockId = blockIdx.z * (gridDim.x * gridDim.y)
		    + blockIdx.y * gridDim.x
		    + blockIdx.x;
	
	start0 = clock();
	int threadId = blockId * (blockDim.x * blockDim.y * blockDim.z)
		    + threadIdx.z * (blockDim.x * blockDim.y)
		    + threadIdx.y * blockDim.x
		    + threadIdx.x;
	finish0 = clock();
	ts[threadId] = finish0 - start0;

	float tmp = 0.0;
	float swap = 0.0;
	//even
	if(*signal > 0){
		if(threadId < n){
			if(threadId % 2 == 0){
				start1 = clock();
				tmp = a[threadId] - a[threadId + 1];
				finish1 = clock();
				swap = a[threadId + 1];
				if(tmp <= 0){
					start2 = clock();
					a[threadId + 1] = swap;
					finish2 = clock();
				}
				else{
					a[threadId + 1] = a[threadId];
					start2 = clock();
					a[threadId] = swap;
					finish2 = clock();
				}
				tx[threadId] = (finish2 - start2) + (finish1 - start1);
			}
		}
	}
	//odd
	if(*signal < 0){
		if(threadId + 1 < n){
			if(threadId % 2 == 1){
				start1 = clock();
				tmp = a[threadId] - a[threadId + 1];
				finish1 = clock();
				swap = a[threadId + 1];
				if(tmp <= 0){
					start2 = clock();
					a[threadId + 1] = swap;
					finish2 = clock();
				}
				else{
					a[threadId + 1] = a[threadId];	
					start2 = clock();
					a[threadId] = swap;
					finish2 = clock();
				}
				tx[threadId] = (finish2 - start2) + (finish1 - start1);
			}
		}
	}
}

int main(){
	float overhead = 0.0, TxSum = 0.0, TsSum = 0.0;
	int *signal = (int*)malloc(sizeof(int) * 1);
	float *a = (float*)malloc(sizeof(float) * N);
	float *Tx = (float*)malloc(sizeof(float) * N);
	float *Ts = (float*)malloc(sizeof(float) * N);
	float *ts = (float*)malloc(sizeof(float) * N);
	float *tx = (float*)malloc(sizeof(float) * N);
	signal[0] = 1;
	//intialization array a, Tx, Ts, tx, ts
	int i = 0, x = N-1;
	for( ; i < N ; i++){
		a[i] = (float)x;
		x--;
		Tx[i] = 0.0;
		Ts[i] = 0.0;
		tx[i] = 0.0;
		ts[i] = 0.0;
	}
	
	float *d_a, *d_ts, *d_tx;
	int *d_signal;	
	cudaMalloc(&d_a, sizeof(float) * N);
	cudaMalloc(&d_tx, sizeof(float) * N);
	cudaMalloc(&d_ts, sizeof(float) * N);
	cudaMalloc(&d_signal, sizeof(int) * 1);

	cudaMemcpy(d_a, a, sizeof(float)*N, cudaMemcpyHostToDevice);
	cudaMemcpy(d_tx, tx, sizeof(float)*N, cudaMemcpyHostToDevice);
	cudaMemcpy(d_ts, ts, sizeof(float)*N, cudaMemcpyHostToDevice);
	cudaMemcpy(d_signal, signal, sizeof(int)*1, cudaMemcpyHostToDevice);

	dim3 grid(1,1,1);
	dim3 block(N,1,1);
	//GPU 'warm up'
	Sort<<<grid, block>>>(d_a, N, d_ts, d_tx, d_signal);
	
	cudaMemcpy(d_a, a, sizeof(float)*N, cudaMemcpyHostToDevice);
	cudaMemcpy(d_tx, tx, sizeof(float)*N, cudaMemcpyHostToDevice);
	cudaMemcpy(d_ts, ts, sizeof(float)*N, cudaMemcpyHostToDevice);
	cudaMemcpy(d_signal, signal, sizeof(int) * 1, cudaMemcpyHostToDevice);
	
	i=0;
	int j = N - 1;	
	for( ; i < N ; i++, j--){

		Sort<<<grid, block>>>(d_a, N, d_ts, d_tx, d_signal);
		
		cudaMemcpy(ts, d_ts, sizeof(float) * N, cudaMemcpyDeviceToHost);
		Ts[j] = max(ts, N);
		flush(ts, N);
		cudaMemcpy(d_ts, ts, sizeof(float) * N, cudaMemcpyHostToDevice);	

		cudaMemcpy(tx, d_tx, sizeof(float) * N, cudaMemcpyDeviceToHost);
		Tx[j] = max(tx, N);
		flush(tx, N);
		cudaMemcpy(d_tx, tx, sizeof(float) * N, cudaMemcpyHostToDevice);

		*signal = *signal * (-1);
		cudaMemcpy(d_signal, signal, sizeof(int) * 1, cudaMemcpyHostToDevice);	
	}
	
	cudaMemcpy(a, d_a, sizeof(float) * N, cudaMemcpyDeviceToHost);
/*
	i = 0;
	for( ; i < N ; i ++){
		printf("a[%d] = %f ; ",i,a[i]);
		printf("\n");
	}
	
	i = 0;
	for( ; i < N ; i ++){
		printf("Tx[%d] = %f ; ",i,Tx[i]);
		printf("\n");
	}
	
	i = 0;
	for( ; i < N ; i ++){
		printf("Ts[%d] = %f ; ",i,Ts[i]);
		printf("\n");
	}
*/
	TxSum = sum(Tx, N);
	TsSum = sum(Ts, N);
	overhead = TxSum + TsSum;
	printf("TxSum = %f, TsSum = %f, overhead = %f \n", TxSum, TsSum, overhead);
	free(a); free(signal); free(Tx); free(Ts); free(ts); free(tx);
	cudaFree(d_a); cudaFree(d_ts); cudaFree(d_tx); cudaFree(d_signal);
	
		
	return 0;
}	




















