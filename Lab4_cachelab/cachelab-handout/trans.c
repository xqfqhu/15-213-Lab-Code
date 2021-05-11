/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void transpose_32by32(int A[32][32], int B[32][32]);
void transpose_64by64(int A[64][64], int B[64][64]);
void transpose_67by61(int A[67][61], int B[61][67]);
/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
  if (M == 32 && N == 32) transpose_32by32(A, B);
  else if (M == 64 && N == 64) transpose_64by64(A, B);
  else if (M == 61 && N == 67) transpose_67by61(A, B);
}

void transpose_32by32(int A[32][32], int B[32][32])
{
  int i, j, il, jl, diag, diagIDX, diagFlag = 0;
  for(i = 0; i < 32; i += 8){
    for(j = 0; j < 32; j += 8){
      for(il = i; il < 8 + i; il++){
        for(jl = j; jl < 8 + j; jl++){
          if(il == jl) {
            diag = A[il][jl];
            diagIDX = il;
            diagFlag = 1;
          }
          else B[jl][il] = A[il][jl];
        }
        
        if(diagFlag == 1){
          B[diagIDX][diagIDX] = diag; 
          diagFlag = 0;
        }
      }
    }
  }
}





void transpose_64by64(int A[64][64], int B[64][64])
{
  int i, j, il, jl, v0, v1, v2, v3, v4, v5, v6, v7;
  for (j = 0; j < 64; j += 8){
    for (i = 0; i < 64; i += 8){
      if ((i + 16 < 64) && ((i + 16 < j) || (i + 8 >= j)) && ((i + 8 < j) || (i >= j))){
        for (il = i; il < i + 8; il++){
          v0 = A[il][j];
          v1 = A[il][j+1];
          v2 = A[il][j+2];
          v3 = A[il][j+3];
          v4 = A[il][j+4];
          v5 = A[il][j+5];
          v6 = A[il][j+6];
          v7 = A[il][j+7];

          /* want to store at higher half of B -> store at next block after the next block if possible */
          B[j+4][il+16] = v0;
          B[j+5][il+16] = v1;
          B[j+6][il+16] = v2;
          B[j+7][il+16] = v3;
          
          /* want to store at lower half of B -> store at next block if possible */
          B[j+4][il+8] = v4;
          B[j+5][il+8] = v5;
          B[j+6][il+8] = v6;
          B[j+7][il+8] = v7;
        }
      }
      else if ((i + 8 < 64) && ((i + 8 < j) || (i >= j))){
        for (il = i; il < i + 8; il++){
          v4 = A[il][j+4];
          v5 = A[il][j+5];
          v6 = A[il][j+6];
          v7 = A[il][j+7];

          /* want to store at lower half of B -> store at next block if possible */
          B[j+4][il+8] = v4;
          B[j+5][il+8] = v5;
          B[j+6][il+8] = v6;
          B[j+7][il+8] = v7;
        }
        for (il = i; il < i + 4; il++){
          v0 = A[il][j];
          v1 = A[il][j + 1];
          v2 = A[il][j + 2];
          v3 = A[il][j + 3];

          B[j][il] = v0;
          B[j + 1][il] = v1;
          B[j + 2][il] = v2;
          B[j + 3][il] = v3;
        }
        for (il = i + 4; il < i + 8; il++){
          v0 = A[il][j];
          v1 = A[il][j + 1];
          v2 = A[il][j + 2];
          v3 = A[il][j + 3];

          B[j][il] = v0;
          B[j + 1][il] = v1;
          B[j + 2][il] = v2;
          B[j + 3][il] = v3;
        }
      }
      else if ((i + 16 < 64) && ((i + 16 < j) || (i + 8 >= j))){
        for (il = i; il < i + 8; il++){
          v0 = A[il][j];
          v1 = A[il][j + 1];
          v2 = A[il][j + 2];
          v3 = A[il][j + 3];

          /* want to store at higher half of B -> store at next block after the next block if possible */
          B[j + 4][il + 16] = v0;
          B[j + 5][il + 16] = v1;
          B[j + 6][il + 16] = v2;
          B[j + 7][il + 16] = v3;
        }
        for (il = i + 4; il < i + 8; il++){
          v4 = A[il][j + 4];
          v5 = A[il][j + 5];
          v6 = A[il][j + 6];
          v7 = A[il][j + 7];

          B[j + 4][il] = v4;
          B[j + 5][il] = v5;
          B[j + 6][il] = v6;
          B[j + 7][il] = v7;
        }
        for (il = i; il < i + 4; il++){
          v4 = A[il][j + 4];
          v5 = A[il][j + 5];
          v6 = A[il][j + 6];
          v7 = A[il][j + 7];

          B[j + 4][il] = v4;
          B[j + 5][il] = v5;
          B[j + 6][il] = v6;
          B[j + 7][il] = v7;
        }
      }
      else{
        for (il = i + 4; il < i + 8; il++){
          v4 = A[il][j + 4];
          v5 = A[il][j + 5];
          v6 = A[il][j + 6];
          v7 = A[il][j + 7];
          
          B[j + 4][il] = v4;
          B[j + 5][il] = v5;
          B[j + 6][il] = v6;
          B[j + 7][il] = v7;
        }
        for (il = i; il < i + 4; il++){
          v4 = A[il][j + 4];
          v5 = A[il][j + 5];
          v6 = A[il][j + 6];
          v7 = A[il][j + 7];
          
          B[j + 4][il] = v4;
          B[j + 5][il] = v5;
          B[j + 6][il] = v6;
          B[j + 7][il] = v7;
        }
        for (il = i + 4; il < i + 8; il++){
          v0 = A[il][j];
          v1 = A[il][j + 1];
          v2 = A[il][j + 2];
          v3 = A[il][j + 3];
          
          B[j][il] = v0;
          B[j + 1][il] = v1;
          B[j + 2][il] = v2;
          B[j + 3][il] = v3;
        }
        for (il = i; il < i + 4; il++){
          v0 = A[il][j];
          v1 = A[il][j + 1];
          v2 = A[il][j + 2];
          v3 = A[il][j + 3];
          
          B[j][il] = v0;
          B[j + 1][il] = v1;
          B[j + 2][il] = v2;
          B[j + 3][il] = v3;
        } 
      }
      if ((i + 8 < 64) && ((i + 8 < j) || (i >= j))){
        /* put lower half of B back */
        for (il = i; il < i + 8; il++){
          for (jl = j + 4; jl < j + 8; jl++){
            B[jl][il] = B[jl][il + 8];
          }
        }
      }
      if ((i + 16 < 64) && ((i + 16 < j) || (i + 8 >= j))){
        /* put higher half of B back */
        for (il = i; il < i + 8; il++){
          for (jl = j; jl < j + 4; jl++){
            B[jl][il] = B[jl+4][il + 16];
          }
        }
      }
    }
  }
}


void transpose_67by61(int A[67][61], int B[61][67])
{
  int i, j, il, v0, v1, v2, v3, v4, v5, v6, v7;
  for (i = 0; i < 64; i += 8){
    for (j = 0; j < 56; j += 8){
      for (il = i; il < i + 8; il++){
        v0 = A[il][j];
        v1 = A[il][j + 1];
        v2 = A[il][j + 2];
        v3 = A[il][j + 3];
        v4 = A[il][j + 4];
        v5 = A[il][j + 5];
        v6 = A[il][j + 6];
        v7 = A[il][j + 7];

        B[j][il] = v0;
        B[j + 1][il] = v1;
        B[j + 2][il] = v2;
        B[j + 3][il] = v3;
        B[j + 4][il] = v4;
        B[j + 5][il] = v5;
        B[j + 6][il] = v6;
        B[j + 7][il] = v7;
      }
    }
    for (il = i; il < i + 8; il++){
      v0 = A[il][56];
      v1 = A[il][57];
      v2 = A[il][58];
      v3 = A[il][59];
      v4 = A[il][60];

      B[56][il] = v0;
      B[57][il] = v1;
      B[58][il] = v2;
      B[59][il] = v3;
      B[60][il] = v4;
    }
  }
  for (j = 0; j < 56; j += 8){
    for (il = 64; il < 67; il++){
      v0 = A[il][j];
      v1 = A[il][j + 1];
      v2 = A[il][j + 2];
      v3 = A[il][j + 3];
      v4 = A[il][j + 4];
      v5 = A[il][j + 5];
      v6 = A[il][j + 6];
      v7 = A[il][j + 7];

      B[j][il] = v0;
      B[j + 1][il] = v1;
      B[j + 2][il] = v2;
      B[j + 3][il] = v3;
      B[j + 4][il] = v4;
      B[j + 5][il] = v5;
      B[j + 6][il] = v6;
      B[j + 7][il] = v7;
    }
  }
  for (il = 64; il < 67; il++){
    v0 = A[il][56];
    v1 = A[il][57];
    v2 = A[il][58];
    v3 = A[il][59];
    v4 = A[il][60];

    B[56][il] = v0;
    B[57][il] = v1;
    B[58][il] = v2;
    B[59][il] = v3;
    B[60][il] = v4;
  }
}
/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}






/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
    
    

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

