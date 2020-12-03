#include <iostream>
using namespace std;

const int SIZE = 8;


void print(__int8* arr) 
{
	for (int i = 0; i < SIZE; i++)	
		printf("%3d", arr[i]);
	cout << endl;
}

void print(__int16* arr)
{
	for (int i = 0; i < SIZE; i++)
		printf("%3d", arr[i]);
	cout << endl;
}

void answer(__int8* A, __int8* B, __int8* C, __int16* D, __int16* F)
{	
	for (int i = 0; i < SIZE; i++)
		F[i] = A[i] * C[i] + B[i] * D[i];	
}

void unpack_b_w(__int8* bytes, __int16* words)
{
	for (int i = 0; i < SIZE/8; i++) {

		__asm
		{
			mov eax, words;
			mov ecx, bytes;
			movq mm0, [ecx];
			pxor mm7, mm7;
			punpcklbw mm0, mm7;
			movq[eax], mm0;
			movq mm0, [ecx];
			punpckhbw mm0, mm7;
			movq[eax + 8], mm0;
		}
	}
}

void multiply_ws(__int16* operator1, __int16* operator2, __int16* result)
{
	__int16 op1[4];
	__int16 op2[4];
	__int16 temp_result[4];
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
	__int16 op1[4];
	__int16 op2[4];
	__int16 temp_result[4];
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
	__int8 A[SIZE]{ -1, -2, -3, -4, -5, -6, -7, -8 },
		B[SIZE]{ 1, 2, 3, 4, 5, 6, 7, 8 },
		C[SIZE]{ 1, 2, 3, 4, 5, 6,7,8 };
	__int16 D[SIZE]{1, 2, 3, 4, 5, 6, 7, 8 };
	__int16 Aw[SIZE], Bw[SIZE], Cw[SIZE], res[SIZE];
	unpack_b_w(A, Aw);
	unpack_b_w(B, Bw);
	unpack_b_w(C, Cw);
	__int16 AC[SIZE];
	__int16 BD[SIZE];
	multiply_ws(Aw, Cw, AC);
	multiply_ws(Bw, D, BD);
	add(AC, BD, res);
	cout << "A: ";
	print(A);
	cout << "B: ";
	print(B);
	cout << "C: ";
	print(C);
	cout << "D: ";
	print(D);
	cout << "Expected: ";
	__int16 answ[SIZE];
	answer(A, B, C, D, answ);
	print(answ);
	cout << "Got: ";
	print(res);

	
	return 0;
}
