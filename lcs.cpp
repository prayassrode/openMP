#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <omp.h>
#include <chrono>

using namespace std;


#ifdef __cplusplus
extern "C" {
#endif

  
#ifdef __cplusplus
}
#endif


static const char al[] = "abcdefghijklmnopqrstuvwxyz";

void randString(char* a,int l)

{
    #pragma omp parallel for
    for(int i=0;i<l;i++){
        a[i]=al[rand() % 26];
    }
}

void lcs(char* s1,char* s2, int m, int n ,int** l){
	int sum=1;
	int c=0;
	
	for(int i=1;i<m+n;i++){
		sum+=1;
		if(i<=m)
			c = min(i,n);
		else
			c = n - (i - m);

		#pragma omp parallel for
		for(int j=0; j<c; j++){
			int a = min(i,m) - j;
			int b = sum - a;
			if(s1[a-1] == s2[b-1])
				l[a][b] = l[a-1][b-1]+1;
			else
				l[a][b] = max(l[a-1][b],l[a][b-1]);
		}
		
	}

}


int main (int argc, char* argv[]) {

	if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <m> <nbthreads>"<<std::endl;
    return -1;
    }

	int n = atoi(argv[1]);
	int m = atoi(argv[2]);
	int nbthreads = atoi(argv[3]);

	char* s1 = new char[m];
	char* s2 = new char[n];

	omp_set_num_threads(nbthreads);
    
    //exchange m and n if m is less than n
	if(m < n){
		int temp = m;
		m = n;
		n = temp;
	}

	int** l;

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
  
  if (argc < 2) {
    std::cerr<<"Usage: "<<argv[0]<<"<nbthreads> <s1> <s2>"<<std::endl;
    return -1;
  }
  //write code here

	randString(s1,m);
	randString(s2,n);
	l = (int**) malloc(sizeof(int*) * (m + 1));
	
	for(int i = 0; i <= m; i++) {
		l[i] = (int*) malloc(sizeof(int) * (n + 1));
	}

	#pragma omp parallel for
	for(int i=0;i<=m;i++){
		l[i][0] = 0;
	}

	#pragma omp parallel for
	for(int i=0;i<=n;i++){
		l[0][i] = 0;
	}
	
	
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	lcs(s1,s2,m,n,l);
	
	
	end = std::chrono::system_clock::now();
	
	std::chrono::duration<double> elapsed_seconds = end-start;    
	std::cerr<<elapsed_seconds.count()<<std::endl;
     	int index = l[m][n];

	char lc[index+1];
 	lc[index] = '\0';

  	int i = m;
  	int j = n;
	while (i > 0 && j > 0) {
	    if (s1[i-1] == s2[j-1]) {
	      lc[index-1] = s1[i-1]; 
	      i--;
	      j--;
	      index--;
	    }

	    else if (l[i-1][j] >= l[i][j-1])
	      i--;
	    else
	      j--;
	  }
	cout<<lc<<endl;
  
  free(l);
  delete[] s1;
  delete[] s2;
  return 0;
}
