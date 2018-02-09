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

int *tmp;

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

void merge(int a[], int l, int h, int m, int n)
{

  int i, j, k;
  i = l;
  k = l;
  j = m + 1;
 
  while (i <= m && j <= h)
  {
    if (a[i] < a[j])
    {
      tmp[k] = a[i];
      k++;
      i++;
    }
    else
    {
      tmp[k] = a[j];
      k++;
      j++;
    }
  }

  while (i <= m)
  {
    tmp[k] = a[i];
    k++;
    i++;
  }

  while (j <= h)
  {
    tmp[k] = a[j];
    k++;
    j++;
  }
 
  for (i = l; i <= h; i++)
  {
    a[i] = tmp[i];
  }

}


void mergesort(int a[], int l, int h, int n)
{
  int m;
  if (l < h && (h - l)>10000)
  {
    m=(l+h)/2;

    #pragma omp task
    mergesort(a, l, m, n);

    #pragma omp task
    mergesort(a, m+1, h, n);

    #pragma omp taskwait
    
    merge(a, l, h, m,n);
    
  }
  else if (l < h){
    m=(l+h)/2;
    
    mergesort(a, l, m, n);
    
    mergesort(a, m+1, h, n);
    
    merge(a, l, h, m,n);
  }
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
  
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }
  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);

  int * arr = new int [atoi(argv[1])];

  generateMergeSortData (arr, atoi(argv[1]));
  tmp = new int [atoi(argv[1])];
  for(int i = 0; i < n; i++)
    tmp[i] = arr[i];
  
  //write code here
  // int granularity = atoi(argv[4]);

  std::chrono::time_point<std::chrono::system_clock> start, end;

  omp_set_num_threads(nbthreads);


  // int *tempArr = new int [atoi(argv[1])];
  // tempArr = (int*)malloc(n*sizeof(int));
  start = std::chrono::system_clock::now();

  #pragma omp parallel
  {
     #pragma omp single
     mergesort(arr, 0, n-1, n);
  }

  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  
  checkMergeSortResult (arr, atoi(argv[1]));
  //cout<<"num of threads: "<<omp_get_num_threads()<<endl;
  std::cerr<<elapsed_seconds.count()<<endl;
  // std::cout<<sum<<endl;
  
  delete[] arr;

  return 0;
}
