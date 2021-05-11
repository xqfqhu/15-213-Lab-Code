#include <stdio.h>

int howManyBits(int x) { //binary search without control flow !!! --> shift + 111...111/000...000
  int b16, b8, b4, b2, b1, b0;
  int sign = x >> 31;
  
 
  
  x = (sign & ~x) | (~sign & x);
  
  b16 = !!(x >> 16) << 4;
  x = x >> b16;

  b8 = !!(x >> 8) << 3;
  x = x >> b8;

  b4 = !!(x >> 4) << 2;
  x = x >> b4;

  b2 = !!(x >> 2) << 1;
  x = x >> b2;

  b1 = !!(x >> 1);
  x = x >> b1;

  b0 = !!(x);

  

  return b16 + b8 + b4 + b2 + b1 + b0 + 1 ;
  

}



int main(){
    printf("%i\n",howManyBits(0)); 
    
    
}