/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  /* exactly one 1 = (~(both 1)) & (at least one 1) = (~(both 1)) & (~(both 0)) */
  int bothOne;
  int bothZero;
  bothOne = x & y;
  bothZero = (~x) & (~y);
  return (~bothOne) & (~bothZero);
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {

  return 1<<31;

}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  int cmpZero1 = !(~((x + 1) ^ x));
  int cmpZero2 = !(~x);
  return cmpZero1 ^ cmpZero2;
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int allEven8bit = 0x55;
  int allEven16bit = (allEven8bit << 8) + allEven8bit;
  int allEven32bit = (allEven16bit << 16) + allEven16bit;
  int allOne = x | allEven32bit;
  int allZero = ~allOne;
  return !allZero;
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
 
  return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) { 
  int lhs = !(x >> 6); //condition 1
  int rhs1 = !(((~x) + 1 + 0x39) >> 31); //condition 2

  int rhs2 = !((x + ~(0x30) + 1) >> 31); 
  return lhs & rhs1 & rhs2;

}

int isAsciiDigit2(int x) {
  int lhs = !(x >> 6); //condition 1
  int rhs1 = !((((~0) ^ x) + 1 + 0x39 ) >> 31); //condition 2

  int rhs2 = !!((((~0) ^ x) + 1 + 0x30 -1) >> 31); // be careful with the x-() = 0 case
  return lhs & rhs1 & rhs2;

}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */

int conditional(int x, int y, int z) {
  int _bool = (!x) + (~0);
  return (_bool & y) | (~_bool & z);
}

/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  /* return 1 if: 1. x * y <= 0, y >= 0; or 2. x * y > 0 and y - x >= 0 */
  int posx = !(x >> 31); // if x>=0, posx = 1; if x < 0, posx = 0
  int posy = !(y >> 31);// if y>=0, posy = 1; if y < 0, posy = 0
  int difSign = posx ^ posy; // if x * y <= 0, difsign = 1;
  int difSignLargeY = difSign & posy; //if y>=0, x <0, difSignLargeX = 1; condition 1
  int YminusX = (((~0) ^ x) + 1) + y; //y-x, no overflow if !difSign
  int YminusXpos = !(YminusX >> 31); //if y-x > 0, YminusXpos = 1, no overflow if !difSign
  int sameSignLargeY = (!difSign) & YminusXpos; // if x * y > 0 and y >= x,  sameSignLargeY = 1; condition 2
  return difSignLargeY | sameSignLargeY;
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  /* if -x == x, return 1, corner case: x == Tmin */
  int negX = ((~0) ^ x) + 1; // -x, overflow if x == Tmax
  int XdifNegX = (negX ^ x) >> 31; //if x == 0 or x == Tmin, XdifNegX = 0; otherwise XdifNegX = 111111
  int XdifNegXbool = 1 & (~XdifNegX); // if x==0 or x == Tmin, XdifNegXbool = 1; otherwise XdifNegXbool = 0
  int Tmin = 1 << 31;
  int TminBool = Tmin ^ x; // if x == Tmin, TminBool = 0; if x == 0, TminBool = 1000...000
  int TminzeroBool = (TminBool >> 31);
  return XdifNegXbool & TminzeroBool;
}


/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
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

  

  return b16 + b8 + b4 + b2 + b1 + b0 +1;
  

}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  
  unsigned s = uf & 0x80000000;
  unsigned exp = (uf & 0x7F800000);
  unsigned frac = uf & 0x007FFFFF;

  if(!(exp ^ 0x7F800000)) return uf; //special case
  else if (!(exp ^ 0x00000000)) frac = frac << 1; //denormalized

  else exp = exp + 0x00800000; //normalized

  return s | exp | frac;
  

}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  unsigned s = uf & 0x80000000;
  int exp = (uf & 0x7F800000);
  int frac = uf & 0x007FFFFF;


  if(!(exp ^ 0x7F800000)) return 0x80000000u; //special case
  else if (!(exp ^ 0x00000000)) return 0; //denormalized
  
  exp = exp >> 23;
  int bias = 0x0000007F;
  int E = exp - bias;
  int M = frac + 0x00800000;
  
  if(E < 0) return 0;
  else if(E < 23) M = M >> (23 - E);
  else if(E < 31) M = M << (E - 23);
  else{
    if(s == 0x80000000 && E == 31 && ((M << 8) == 0x80000000)) return 0x80000000;
    else return 0x80000000u;
  }

  if(s == 0x80000000) return -M;
  else return M;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {//!!!slow!
  /* x >= -126 --> noramlized, frac = 0, increment exp; x < -126 --> denormalized, frac starts from 1 and then >>; */

  if(x < -149) return 0; //out of bound for denorm
  else if (x > 127) return 0x7F800000; //out of bound for norm
  
  else if(x >= -126) return (x + 127) << 23;
  else return 0x400000 >> ( x + 127);
}
