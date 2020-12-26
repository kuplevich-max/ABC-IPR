
#include <iostream>
#include <mmintrin.h>
using namespace std;

const int SIZE = 8;


void print(__int8* arr)
{
	for (int i = 0; i < SIZE; i++)
		printf("%8d", arr[i]);
	cout << endl;
}

void print(__int16* arr)
{
	for (int i = 0; i < SIZE; i++)
		printf("%8d", arr[i]);
	cout << endl;
}

void answer(__int8* A, __int8* B, __int8* C, __int16* D, __int16* F)
{
	for (int i = 0; i < SIZE; i++)
		F[i] = A[i] * C[i] + B[i] * D[i];
}

void unpack_b_w(__int8* bytes, __int16* words)
{
	for (int i = 0; i < SIZE/8 ; i++) {

		__asm
		{

			mov eax, words;
			mov ecx, bytes;
			movq mm0, [ecx];
			pxor mm7, mm7;
			pcmpgtb mm7, mm0;
			punpcklbw mm0, mm7;
			movq[eax], mm0;
			movq mm0, [ecx];
			pcmpgtb mm7, mm0;
			punpckhbw mm0, mm7;
			movq[eax + 8], mm0;
		}
	}
}

void multiply_ws(__int16* operator1, __int16* operator2, __int16* result)
{
	
	for (int i = 0; i < SIZE / 4; i++) {
		__asm
		{
			mov eax, operator1;
			mov ecx, operator2;
			mov edi, result;
			movq mm0, [eax];
			movq mm1, [ecx];
			pmullw mm0, mm1;
			movq[edi], mm0;
		}
		operator1 += 4;
		operator2 += 4;
		result += 4;
	}
}

void add(__int16* operator1, __int16* operator2, __int16* result)
{
	
	for (int i = 0; i < SIZE / 4; i++) {

		__asm
		{
			mov eax, operator1;
			mov ecx, operator2;
			mov edi, result
				movq mm0, [eax];
			movq mm1, [ecx];
			paddw mm0, mm1;
			movq[edi], mm0;
		}
		operator1 += 4;
		operator2 += 4;
		result += 4;
	}
}

int main()
{
	__int8 testA[6][SIZE], testB[6][SIZE], testC[6][SIZE];
	__int16 testD[6][SIZE];
	

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < SIZE; j++)
		{
			testA[i][j] = rand() % 1000 - 500;
			testB[i][j] = rand() % 1000 - 500;
			testC[i][j] = rand() % 1000 - 500;
			testD[i][j] = rand() % 1000 - 500;
		}
		__int16 Aw[SIZE], Bw[SIZE], Cw[SIZE], res[SIZE];
		unpack_b_w(testA[i], Aw);
		unpack_b_w(testB[i], Bw);
		unpack_b_w(testC[i], Cw);
		__int16 AC[SIZE];
		__int16 BD[SIZE];
		multiply_ws(Aw, Cw, AC);
		multiply_ws(Bw, testD[i], BD);
		add(AC, BD, res);
		cout << "A: ";
		print(testA[i]);
		cout << "B: ";
		print(testB[i]);
		cout << "C: ";
		print(testC[i]);
		cout << "D: ";
		print(testD[i]);
		cout << "Expected: ";
		__int16 answ[SIZE];
		answer(testA[i], testB[i], testC[i], testD[i], answ);
		print(answ);
		cout << "Got: ";
		print(res);
		cout << endl;
	}

	return 0;
}

