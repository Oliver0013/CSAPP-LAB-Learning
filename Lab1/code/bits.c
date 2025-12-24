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
  /*
  1. Truth Table
  ---
  x | y | answer
  0   0     0
  0   1     1
  1   0     1
  1   1     0
  ---
  2. Logical Expressions using And(&) Or(|) Not(~)
  ((~x)&y) | (x&(~y))
  3. | to & ~
  ~(~((~x)&y) & ~(x&(~y)))
  */
  return ~(~((~x)&y) & ~(x&(~y)));
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  /*
  min 0x80000000;
  but the max consant we can use is 0xFF;
  */
  return 0x80 << 24;
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
  /*
  !: integer->bool: 0->1; others->0;
  max: 0x7FFFFFFF;
  一方面不能直接用常量，另一方面又不能用移位符构造，故排除
  -x = ~x+1: 补码的相反数等于按位取反+1
  补码的溢出效应，最大+1 -> 最小； 最小-1 -> 最大
  Tmax + 1 = Tmin
  Tmax = ~Tmin
  Tmax + 1 = ~Tmax
  但是有例外
  such that ~x = -x+1, so x=-1
  将条件语句转化成逻辑表达式
  事件1：a==b -> !(a^a): 相等为真1，不等为假0
  事件2：x!=-1 -> !!(x^-1)/ !!(x+1)
  事件12同时成立才为真：1 & 2
  */
  return !((~x)^(x+1)) & !!(x ^ -1);
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
  /*
  只看偶数位：构造掩码0xAAAAAAAA
  掩码操作后与0xAAAAAAAA进行比较
  */
  int mask = 0XAA;
  mask = mask | mask<<8;//0xAAAA
  mask = mask | mask<<16;//0xAAAA AAAA
  return !((x&mask)^mask);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  /*
  -x = ~x+1;
  */
  return ~x+1;
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
  /*
  x-lowerBound >= 0;
  upperBound - x >=0;
  判断最高位是否为0
  掩码看最高位：掩码是0x8000 0000
  
  */
  int lowerBound = 0x30;
  int upperBound = 0x39;
  int mask = 1;
  mask = mask<<31;//8000 0000

  return !(((x+(~lowerBound+1))&mask)^0) & !(((upperBound+(~x+1))&mask)^0);
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  /*
  x=0 -> 生成0x00000000
  x!=0 -> 生成0xFFFFFFFF
  */
  int flag = !!x; //将x转成bool数
  int mask = (flag<<31)>>31; //将01左移到符号位，再借助算数右移构造全0和全1；
  //x=0: z; x=1: y
  return ((~mask)&z) | (mask&y);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  /*
  x-y <= 0;判断符号位, 0问题
  注意减法和加法可能会产生溢出
  只有xy异号会产生溢出
  */
  //1代表负，0代表正
  int sign_x = (x>>31)&1;
  int sign_y = (y>>31)&1;
  //same_sign:0代表同号，1代表异号
  int same_sign = sign_x ^ sign_y;

  
  int yMinsx = y+(~x+1);

  return (!same_sign & !((yMinsx>>31)^0)) | (same_sign & sign_x);
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
  /*
  0有个特征就是相反数就是自身
  意味着0和-0的标志位都是0
  其他则会有一个为1
  */
  //-x = ~x+1;
  int neg_x = ~x+1;
  //neg_x和x
  int sign = neg_x | x;
  //取出符号位,0: 输入为0；-1：输入为其他
  sign = sign>>31; 
  return sign+1;
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
int howManyBits(int x) {
  //正数是要找最高位的1，前面连续的0无用。负数是要找最高位的0，前面连续的1无用
  //1负数，0正数
  int sign = x>>31;
  //1. 统一正负，负数按位取反
  x = (sign&(~x)) | ((~sign)&x);
  //2. 二分查找最高位的1;
  //[16: 31]搜索:!!(x>>16)为1代表有，最少是16
  int b16 = !!(x>>16)<<4;
  //搜索到了，b16不为0，将高16移位至低16
  x = x>>b16;
  //[8, 15],搜索到了最少8
  int b8 = !!(x>>8)<<3;
  x = x>>b8;
  //[4,7], 搜索到了最少4
  int b4 = !!(x>>4)<<2;
  x = x>>b4;
  //[2,3], 搜索到了最少为2
  int b2 = !!(x>>2)<<1;
  x = x>>b2;
  //[1], 搜索到了最少为1
  int b1 = !!(x>>1);
  x = x>>b1;
  //[0]
  int b0 = x;

  return b16+b8+b4+b2+b1+b0+1;
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
  unsigned sign = uf&0x80000000u; //符号位
  //阶码域[23:30]
  unsigned exp = (uf>>23)&0xFF;
  //尾数[0:22]
  int frac = uf&0x7FFFFF;

  //1. 非规格化的值，exp全为0
  // 公式: (-1)^s * 0.frac * 2^(-126)
  if (!(exp)){
    //非规格化的值直接左移并保证符号位不改变
    return uf<<1 | sign;
  }

  //2. 特殊值exp全为1
  if(!(exp^0xFF)){
    return uf;
  }
  //3.正常规格值
  //公式: (-1)^s * 1.frac * 2^(exp-127)
  //不能整体左移，一方面frac左移不代表*2
  //另一方面exp不全为0也会改变exp
  exp = exp+1;
  if (!(exp^0xFF)){
    return sign|0x7F800000;
  }
  else{
    return sign|(exp<<23)| frac;
  }
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
  //提取符号位0-正 -1-负
  int sign = uf&0x80000000;
  //提取阶码exp[23,30]
  int exp = (uf>>23)&0xFF;
  //提取尾数frac[0,22]
  int frac = uf&0x7FFFFF;
  //将阶码转化为实际指数
  int E = exp-127;
  //1. 指数小于0代表结果0.0000，截断后结果为0(包括非规格数)
  if (E<0){
    return 0;
  }
  //2. 指数大于等于31, 因为32位int最多只能包括2^31-1
  if (E>=31) {
    return 0x80000000u;
  }
  //3. [0,30]
  //公式: (-1)^s * 1.frac * 2^(exp-127)
  //将隐藏的1还回去
  frac = frac | (1<<23);
  int M;
  //意味着要截断
  if (E<23){
    M = frac>>(23-E); 

  }else{
    M = frac<<(E-23);
  }
  //如果是负数的话，取相反数
  if (sign){
    M = -M;
  }
  return M;
  
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
 //注：需要将btest.c的TIMEOUT_LIMIT从10改为20
unsigned floatPower2(int x) {
  //阶码：128代表指数1
  int exp = 127+x;
  //太大了
  if (exp>=255){
    //返回正无穷
    return 0x7F800000u;
  }
  //太小了,浮点数最小只能2^(1-127)*2^(-23)
  //即0x00000001
  if (x<-149){
    return 0;
  }
  //非规格数[-149,-126]
  //exp域为0，靠尾数域凑齐
  // 公式：结果 = 1 << (x + 149)
  // 原理：非规格化值 V = M * 2^(-126)。我们要 V = 2^x。
  // 尾数 M = 2^x / 2^(-126) * 2^23 (因为M是整数形式) = 2^(x + 149)
  if (x<-126){
    return 1<<(x+149);
  }
  //规格化值V = 2^(exp-127)*1.frac; 而V=2^x
  //把exp移到阶码位即可
  return exp<<23;
}
