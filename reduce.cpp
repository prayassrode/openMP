#define _GLIBCXX_USE_CXX11_ABI 0
#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <string.h>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

  void generateReduceData (int* arr, size_t n);
  
#ifdef __cplusplus
}
#endif

void reduce(int *arr, int first, int last, int *result){
	int sum = 0;
	int threadNumber = omp_get_thread_num();
	// cout<<"first: "<<first<<" last: "<<last<<endl;
	for (int i = first; i < last; ++i)
	{
		sum += arr[i];
	}
	result[threadNumber] = sum;
	// cout<<"sum is: "<<sum<<endl;
	
}

void calculateRed(int * arr, int *result, int nbthreads, int granularity, int n){
  int first, last;
  for (int i = 0; i < n; i+=granularity)
  // #pragma omp parallel
  {
	first = i;
	last = i+granularity;
	if (last > n)
	{
		last = n;
	}
	// cout<<"in Parallel first: "<<first<<" last: "<<last<<endl;
	#pragma omp task
		reduce(arr, first, last, result);
  }
  //#pragma omp taskwait
}



int main (int argc, char* argv[]) {

  //forces openmp to create the threads beforehand
#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
  
  if (argc < 5) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <schedule> <granularity>"<<std::endl;
    return -1;
  }
  
  //write code here

  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
  int granularity = atoi(argv[4]);

  int * arr = new int [n];
  int * result = new int [nbthreads];

  for (int i = 0; i < nbthreads; ++i)
  {
  	result[i] = 0;
  }

  generateReduceData (arr, atoi(argv[1]));
  if (granularity < 0)
  {
  	granularity = 1;
  }
  if (granularity > n)
  {
  	granularity = n;
  }

  std::chrono::time_point<std::chrono::system_clock> start, end;

  omp_set_num_threads(nbthreads);

  start = std::chrono::system_clock::now();

  int * sum = new int [nbthreads];
  int reductionSum = 0;
 //  #pragma omp parallel
	// {
	// int result_t=0;
	//#pragma omp single
	// #pragma omp for schedule(runtime) private(result_t)
	// 		for (int i=0;i<atoi(argv[1]);i++)
	// 		#pragma omp task
	// 		{
	// 			result_t=result_t+arr[i];
	// 		}
	// 		reductionSum=reductionSum+result_t;
	// }


  #pragma omp parallel
  {
  	#pragma omp single
  	{
  		calculateRed(arr, result, nbthreads, granularity, n);
  	}
  }

  for (int i = 0; i < nbthreads; ++i)
  {
  	// cout<<i<<" : "<<result[i]<<endl;
  	reductionSum += result[i]; 
  }


  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  //cout<<"num of threads: "<<omp_get_num_threads()<<endl;
  std::cerr<<elapsed_seconds.count()<<endl;
  std::cout<<reductionSum<<endl;
  
  delete[] arr;

  return 0;

}