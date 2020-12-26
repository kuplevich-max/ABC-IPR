

#include "omp.h"
#include <stdio.h>
#include <iostream>
#include<ctime>


#define N 10000000
#define CHUNKSIZE 10000

using namespace std;



int main()
{
	srand(time(NULL));
	int i, chunk;
	__int64 *a = new __int64[N], *b = new __int64[N], *c = new __int64[N], sum = 0;
	

	for (i = 0; i < N; i++)
	{		
		a[i] = rand() % 10;
		b[i] = rand() % 10;
	}
	chunk = CHUNKSIZE;
	
	unsigned int t1 = clock();
	
		#pragma omp parallel for default(shared) private(i) schedule(static, chunk) reduction(+:sum)
		for (i = 0; i < N; i++)
			sum = sum + a[i] * b[i];
	
	unsigned int t2 = clock();
	cout << sum << endl << t2 - t1 << endl;
	t1 = clock();
	sum = 0;
	for (i = 0; i < N; i++)
		sum = sum + a[i] * b[i];
	t2 = clock();
	cout << sum << endl << t2 - t1 << endl;
}

