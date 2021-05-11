#include<stdio.h>
void transpose_64(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, il, jl, diag, diagIDX, diagFlag = 0, diag4, diag5, diag6, diag7, tmp;
  for (j = 0; j < M; j += 8){
    for (i = 0; i < N; i += 8){
      for (il = i; il < i + 8; il++){
        for (jl = j; jl < j + 8; jl++){
          tmp = A[il][jl];
          if (il == jl){
            /* diagnal */
            if (il == i + 4) diag4 = tmp;
            else if (il == i + 5) diag5 = tmp;
            else if (il == i + 6) diag6 = tmp;
            else if (il == i + 7) diag7 = tmp;
            else {
              diag = tmp;
              diagIDX = il;
              diagFlag = 1;
            } 
          }
          else if (jl >= j + 4){
            /* want to store at lower half of B */
            if ((il + 8 < 64) && ((il + 8 < j) || (il + 8 >= j + 8))) B[jl][il + 8] = tmp;
            else if ((il + 16 < 64) && (il + 8 >= j) && (il + 8 < j + 8)) B[jl][il + 16] = tmp;
            else B[jl][il] = tmp;
          }
          else{
            /* want to store at higher half of B */
            B[jl][il] = tmp;
          }
        }
        if (diagFlag == 1){
          /* put diagnal in higher half of B back */
          B[diagIDX][diagIDX] = diag;
          diagFlag = 0;
        }
      }
      if ((i + 8 < 64) && ((i + 8 < j) || (i >= j))){
        /* put lower half of B back */
        for (il = i; il < i + 8; il++){
          for (jl = j + 4 ; jl < j + 8; jl++){
            if (jl != il) B[jl][il] = B[jl][il + 8];
          }
        }
      }
      else if ((i + 16 < 64) && (i + 8 >= j) && (i < j )){
        /* put lower half of B back */
        for (il = i; il < i + 8; il++){
          for (jl = j + 4 ; jl < j + 8; jl++){
            if (jl != il) B[jl][il] = B[jl][il + 16];
          }
        }
      }
      if (i == j){
        /* put diagnal in lower half of B back */
        B[i + 4][i + 4] = diag4;
        B[i + 5][i + 5] = diag5;
        B[i + 6][i + 6] = diag6;
        B[i + 7][i + 7] = diag7;
      }
    }
  }
}

void transpose_64_2(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, il, jl, diag, diagIDX, diagFlag = 0, diag4, diag5, diag6, diag7, tmp;
  for (i = 0; i < N; i += 8){
    for (j = 0; j < M; j += 8){
      for (il = i; il < i + 8; il++){
        for (jl = j; jl < j + 8; jl++){
          tmp = A[il][jl];
          if (il == jl){
            /* diagnal */
            if (il == i + 4) diag4 = tmp;
            else if (il == i + 5) diag5 = tmp;
            else if (il == i + 6) diag6 = tmp;
            else if (il == i + 7) diag7 = tmp;
            else {
              diag = tmp;
              diagIDX = il;
              diagFlag = 1;
            } 
          }
          else if ((il >= i + 4) && (jl < j + 4)){
            /* want to store lower half of A at higher half of B */
            if ((il + 16 < 64) && ((il + 8 < j) || (il >= j))) B[jl][il + 16] = tmp;
            else if ((il + 24 < 64) && (il + 8 >= j) && (il < j)) B[jl][il + 24] = tmp;
            else B[jl][il] = tmp;
          }
          else if (jl >= j + 4){
            /* want to store at lower half of B */
            if ((il + 8 < 64) && ((il + 8 < j) || (il >= j))) B[jl][il + 8] = tmp;
            else if ((il + 16 < 64) && (il + 8 >= j) && (il < j)) B[jl][il + 16] = tmp;
            else B[jl][il] = tmp;
          }
          else{
            /* want to store at higher half of B at lower half of B */
            B[jl][il] = tmp;
          }
        }
        if (diagFlag == 1){
          /* put diagnal in higher half of B back */
          B[diagIDX][diagIDX] = diag;
          diagFlag = 0;
        }
      }
      if ((i + 16 < 64) && ((i + 8 < j) || (i >= j))){
        /* put higher half of B  which stores lower half of A back */
        for (il = i + 4; il < i + 8; il++){
          for (jl = j; jl < j + 4; jl++){
            if (jl != il) {
                B[jl][il] = B[jl][il + 16];
                //printf("area 1-1: B[%d][%d] = %d\n", jl, il, B[jl][il]);
            }
          }
        }
      }
      else if ((i + 24 < 64) && (i + 8 >= j) && (i < j)){
        /* put higher half of B  which stores lower half of A back */
        for (il = i + 4; il < i + 8; il++){
          for (jl = j; jl < j + 4; jl++){
            if (jl != il) {
                B[jl][il] = B[jl][il + 24];
                //printf("area 1-2: j = %d, i = %d, B[%d][%d] = %d\n", j, i, jl, il, B[jl][il]);
            }
          }
        }
      }
      if ((i + 8 < 64) && ((i + 8 < j) || (i >= j))){
        /* put lower half of B back */
        for (il = i; il < i + 8; il++){
          for (jl = j + 4 ; jl < j + 8; jl++){
            if (jl != il) {
                B[jl][il] = B[jl][il + 8];
                //printf("area 2-1: B[%d][%d] = %d\n", jl, il, B[jl][il]);
            }
          }
        }
      }
      else if ((i + 16 < 64) && (i + 8 >= j) && (i < j )){
        /* put lower half of B back */
        for (il = i; il < i + 8; il++){
          for (jl = j + 4 ; jl < j + 8; jl++){
            if (jl != il) {
                B[jl][il] = B[jl][il + 16];
                //printf("area 2-2: B[%d][%d] = %d\n", jl, il, B[jl][il]);
            }
          }
        }
      }
      if (i == j){
        /* put diagnal in lower half of B back */
        B[i + 4][i + 4] = diag4;
        B[i + 5][i + 5] = diag5;
        B[i + 6][i + 6] = diag6;
        B[i + 7][i + 7] = diag7;
      }
    }
  }
}


void transpose_64_3(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, il, jl, diag, diagIDX, diagFlag = 0, tmp;
  for (i = 0; i < N; i += 8){
    for (j = 0; j < M; j += 8){
      for (il = i; il < i + 4; il++){
        for (jl = j; jl < j + 4; jl++){
          tmp = A[il][jl];
          if (il == jl){
            diag = tmp;
            diagIDX = il;
            diagFlag = 1;
          }
          else B[jl][il] = tmp;
        }
        if (diagFlag == 1){
            B[diagIDX][diagIDX] = diag;
            diagFlag = 0;
        }
      }


      for (il = i + 4; il < i + 8; il++){
        for (jl = j; jl < j + 8; jl++){
          tmp = A[il][jl];
          if (il == jl){
            diag = tmp;
            diagIDX = il;
            diagFlag = 1;
          }
          else B[jl][il] = tmp;
        }
        if (diagFlag == 1){
            B[diagIDX][diagIDX] = diag;
            diagFlag = 0;
        }
      }
    

      for (il = i; il < i + 8; il++){
        for (jl = j + 4; jl < j + 8; jl++){
          tmp = A[il][jl];
          if (il == jl){
            diag = tmp;
            diagIDX = il;
            diagFlag = 1;
          }
          else B[jl][il] = tmp;
        }
        if (diagFlag == 1){
            B[diagIDX][diagIDX] = diag;
            diagFlag = 0;
        }
      }
    }
  }
}


void transpose_64_4(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, il, jl, v0, v1, v2, v3, v4, v5, v6, v7;
  for (j = 0; j < M; j += 8){
    for (i = 0; i < N; i += 8){
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
                for (jl = j; jl < j + 4; jl++){
                    B[jl][il] = A[il][jl];
                }
            }
            for (il = i + 4; il < i + 8; il++){
                for (jl = j; jl < j + 4; jl++){
                    B[jl][il] = A[il][jl];
                }
            }
        }
        else if ((i + 16 < 64) && ((i + 16 < j) || (i + 8 >= j))){
            for (il = i; il < i + 8; il++){
                v0 = A[il][j];
                v1 = A[il][j+1];
                v2 = A[il][j+2];
                v3 = A[il][j+3];

                /* want to store at higher half of B -> store at next block after the next block if possible */
                B[j+4][il+16] = v0;
                B[j+5][il+16] = v1;
                B[j+6][il+16] = v2;
                B[j+7][il+16] = v3;
            }
            for (il = i; il < i + 4; il++){
                for (jl = j + 4; jl < j + 8; jl++){
                    B[jl][il] = A[il][jl];
                }
            }
            for (il = i + 4; il < i + 8; il++){
                for (jl = j + 4; jl < j + 8; jl++){
                    B[jl][il] = A[il][jl];
                }
            }
        }
        else{
            for (il = i; il < i + 4; il++){
                for (jl = j; jl < j + 4; jl++){
                    B[jl][il] = A[il][jl];
                }
            }
            for (il = i + 4; il < i + 8; il++){
                for (jl = j; jl < j + 4; jl++){
                    B[jl][il] = A[il][jl];
                }
            }
            for (il = i; il < i + 4; il++){
                for (jl = j + 4; jl < j + 8; jl++){
                    B[jl][il] = A[il][jl];
                }
            }
            for (il = i + 4; il < i + 8; il++){
                for (jl = j + 4; jl < j + 8; jl++){
                    B[jl][il] = A[il][jl];
                }
            }
        }
      if ((i + 8 < 64) && ((i + 8 < j) || (i >= j))){
        /* put lower half of B back */
        for (il = i; il < i + 8; il++){
          for (jl = j + 4; jl < j + 8; jl++){
            B[jl][il] = B[jl][il + 8];
            //printf("area 1-1: B[%d][%d] = %d\n", jl, il, B[jl][il]);
          }
        }
      }
      if ((i + 16 < 64) && ((i + 16 < j) || (i + 8 >= j))){
        /* put higher half of B back */
        for (il = i; il < i + 8; il++){
          for (jl = j; jl < j + 4; jl++){
            B[jl][il] = B[jl+4][il + 16];
            //printf("area 2-1: B[%d][%d] = %d\n", jl, il, B[jl][il]);
          }
        }
      }
    }
  }
}


void transpose_64_5(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, il, jl, v0, v1, v2, v3, v4, v5, v6, v7;
  for (j = 0; j < M; j += 8){
    for (i = 0; i < N; i += 8){
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
          if ((i + 16 < 64) && ((i + 16 < j) || (i + 8 >= j))){
            B[j+4][il+16] = v0;
            B[j+5][il+16] = v1;
            B[j+6][il+16] = v2;
            B[j+7][il+16] = v3;
          }
          else{
            for (jl = j; jl < j + 4; jl++){

            }
          }
          /* want to store at lower half of B -> store at next block if possible */
          if ((i + 8 < 64) && ((i + 8 < j) || (i >= j))){
            B[j+4][il+8] = v4;
            B[j+5][il+8] = v5;
            B[j+6][il+8] = v6;
            B[j+7][il+8] = v7;
          }
          else{
            B[j+4][il] = v4;
            B[j+5][il] = v5;
            B[j+6][il] = v6;
            B[j+7][il] = v7;
          }
        }
      if ((i + 8 < 64) && ((i + 8 < j) || (i >= j))){
        /* put lower half of B back */
        for (il = i; il < i + 8; il++){
          for (jl = j + 4; jl < j + 8; jl++){
            B[jl][il] = B[jl][il + 8];
            //printf("area 1-1: B[%d][%d] = %d\n", jl, il, B[jl][il]);
          }
        }
      }
      if ((i + 16 < 64) && ((i + 16 < j) || (i + 8 >= j))){
        /* put higher half of B back */
        for (il = i; il < i + 8; il++){
          for (jl = j; jl < j + 4; jl++){
            B[jl][il] = B[jl+4][il + 16];
            //printf("area 2-1: B[%d][%d] = %d\n", jl, il, B[jl][il]);
          }
        }
      }
    }
  }
}

void transpose_64_6(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, il, jl, v0, v1, v2, v3, v4, v5, v6, v7;
  for (j = 0; j < M; j += 8){
    for (i = 0; i < N; i += 8){
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
                for (jl = j; jl < j + 4; jl++){
                    B[jl][il] = A[il][jl];
                }
            }
            for (il = i + 4; il < i + 8; il++){
                for (jl = j; jl < j + 4; jl++){
                    B[jl][il] = A[il][jl];
                }
            }
        }
        else if ((i + 16 < 64) && ((i + 16 < j) || (i + 8 >= j))){
            for (il = i; il < i + 8; il++){
                v0 = A[il][j];
                v1 = A[il][j+1];
                v2 = A[il][j+2];
                v3 = A[il][j+3];

                /* want to store at higher half of B -> store at next block after the next block if possible */
                B[j+4][il+16] = v0;
                B[j+5][il+16] = v1;
                B[j+6][il+16] = v2;
                B[j+7][il+16] = v3;
            }
            for (il = i + 4; il < i + 8; il++){
                for (jl = j + 4; jl < j + 8; jl++){
                    B[jl][il] = A[il][jl];
                }
            }
            for (il = i; il < i + 4; il++){
                for (jl = j + 4; jl < j + 8; jl++){
                    B[jl][il] = A[il][jl];
                }
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
            //printf("area 1-1: B[%d][%d] = %d\n", jl, il, B[jl][il]);
          }
        }
      }
      if ((i + 16 < 64) && ((i + 16 < j) || (i + 8 >= j))){
        /* put higher half of B back */
        for (il = i; il < i + 8; il++){
          for (jl = j; jl < j + 4; jl++){
            B[jl][il] = B[jl+4][il + 16];
            //printf("area 2-1: B[%d][%d] = %d\n", jl, il, B[jl][il]);
          }
        }
      }
    }
  }
}

void transpose_61by67(int A[61][67], int B[67][61])
{
  int i, j, il, jl, v0, v1, v2, v3, v4, v5, v6, v7;
  for (i = 0; i < 56; i += 8){
    for (j = 0; j < 64; j += 8){
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
      v0 = A[il][64];
      v1 = A[il][65];
      v2 = A[il][66];

      B[64][il] = v0;
      B[65][il] = v1;
      B[66][il] = v2;
    }
  }
  for (j = 0; j < 64; j += 8){
    for (il = 56; il < 61; il++){
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
  for (il = 56; il < 61; il++){
    v0 = A[il][64];
    v1 = A[il][65];
    v2 = A[il][66];

    B[64][il] = v0;
    B[65][il] = v1;
    B[66][il] = v2;
  }
}

int main(){
    int A[61][67];
    int tmp = 0;
    for (int i = 0; i < 61; i++){
        for (int j = 0; j < 67; j++){
            A[i][j] = tmp;
        }
        tmp++;
    }
    int B[67][61];
    transpose_61by67(A, B);
    for (int i = 0; i < 61; i++){
        for (int j = 0; j < 67; j++){
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    for (int i = 0; i < 67; i++){
        for (int j = 0; j < 61; j++){
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }
}