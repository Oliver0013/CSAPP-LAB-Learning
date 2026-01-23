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
    if (N == 32 && M == 32){
        //行分块
        for (int i = 0; i<M; i = i+8){
            //列分块
            for (int j = 0; j<N; j = j+8){
                //处理每一块, 遍历行
                for (int k = i; k < i+8; k++){
                    //通过局部变量将量转移至寄存器
                    //避免直接映射产生的cache抖动
                    int a0 = A[k][j];
                    int a1 = A[k][j+1];
                    int a2 = A[k][j+2];
                    int a3 = A[k][j+3];  
                    int a4 = A[k][j+4];
                    int a5 = A[k][j+5];
                    int a6 = A[k][j+6];
                    int a7 = A[k][j+7];
                    //写入B
                    B[i][k] = a0;
                    B[i+1][k] = a1;
                    B[i+2][k] = a2; 
                    B[i+3][k] = a3;
                    B[i+4][k] = a4;  
                    B[i+5][k] = a5;
                    B[i+6][k] = a6;  
                    B[i+7][k] = a7;                                                                            
                }
            }
        }
    }
// 针对 64x64 矩阵的优化逻辑
    if (N == 64) {
        // 1. 依然保留 8x8 的外壳循环
        for (int j = 0; j < M; j += 8) {
            for (int i = 0; i < N; i += 8) {
                
                // 定义12个局部变量（寄存器）
                // a0-a7 存 A 的值，b0-b3 存 B 的值
                int a0, a1, a2, a3, a4, a5, a6, a7;
                int b0, b1, b2, b3;
                int k;

                // ==========================================================
                // 阶段一：处理 A 的上半部分 (Row i ~ i+3)
                // ==========================================================
                // 目标：
                // 1. 把 A 的左上(4x4) -> 搬到 B 的左上 (正确位置)
                // 2. 把 A 的右上(4x4) -> 搬到 B 的右上 (这是**临时存放**!)
                for (k = 0; k < 4; k++) {
                    // 一次读出 A 的一行 (8个)
                    // 此时 Cache 里装载了 A 的第 k 行
                    a0 = A[i+k][j+0]; a1 = A[i+k][j+1]; a2 = A[i+k][j+2]; a3 = A[i+k][j+3];
                    a4 = A[i+k][j+4]; a5 = A[i+k][j+5]; a6 = A[i+k][j+6]; a7 = A[i+k][j+7];

                    // 写到 B 的第 k 行 (Set 冲突风险区)
                    // 注意：此时 B 只有 Row j+k 被激活，没有涉及 j+k+4，所以很安全
                    B[j+k][i+0] = a0; B[j+k][i+1] = a1; B[j+k][i+2] = a2; B[j+k][i+3] = a3;
                    B[j+k][i+4] = a4; B[j+k][i+5] = a5; B[j+k][i+6] = a6; B[j+k][i+7] = a7; 
                }

                // ==========================================================
                // 阶段二：大挪移 (Row i+4 ~ i+7)
                // ==========================================================
                // 这里我们遍历 A 的列 (也就是 B 的行)，这是最骚的操作
                // k 代表当前处理的是 8x8 块内部的第几列
                for (k = 0; k < 4; k++) {
                    // 步骤 1: 读取 A 的下半部分 (竖着读，一次读一列的4个值)
                    // 疑问：竖着读 A 不会 Miss 吗？
                    // 解答：不会！因为 64x64 矩阵间隔 4 行才会冲突。
                    //       这里读的是 i+4, i+5, i+6, i+7，这4行互不冲突！
                    
                    // 取出 A 左下角的一列 (将来要去 B 的右上)
                    a0 = A[i+4][j+k]; a1 = A[i+5][j+k]; a2 = A[i+6][j+k]; a3 = A[i+7][j+k];
                    // 取出 A 右下角的一列 (将来要去 B 的右下)
                    a4 = A[i+4][j+k+4]; a5 = A[i+5][j+k+4]; a6 = A[i+6][j+k+4]; a7 = A[i+7][j+k+4];

                    // 步骤 2: 读取 B "临时仓库" 里的值 (右上角)
                    // 这些是阶段一存进去的 A 的右上角数据
                    b0 = B[j+k][i+4]; b1 = B[j+k][i+5]; b2 = B[j+k][i+6]; b3 = B[j+k][i+7];

                    // 步骤 3: 真正的写入 (最关键的一步)
                    
                    // 3.1 把 A 左下角的数据 (a0-a3)，填进 B 的右上角
                    //     (覆盖掉刚才读出来的 b0-b3，完成狸猫换太子)
                    B[j+k][i+4] = a0; B[j+k][i+5] = a1; B[j+k][i+6] = a2; B[j+k][i+7] = a3;

                    // 3.2 把 B 仓库的数据 (b0-b3)，搬到 B 的左下角 (它们真正的家)
                    //     注意：这里涉及了 B[j+k+4] (下半部分)，终于触发了 Set 切换
                    B[j+k+4][i+0] = b0; B[j+k+4][i+1] = b1; B[j+k+4][i+2] = b2; B[j+k+4][i+3] = b3;

                    // 3.3 把 A 右下角的数据 (a4-a7)，填进 B 的右下角
                    B[j+k+4][i+4] = a4; B[j+k+4][i+5] = a5; B[j+k+4][i+6] = a6; B[j+k+4][i+7] = a7;
                }
            }
        }
    }
    // 针对 61x67 不规则矩阵
    else {
        int i, j, row, col;
        int block_size = 16; // 经测试，16 或 17 都能过

        // 外层循环：遍历每一个块
        for (i = 0; i < N; i += block_size) {
            for (j = 0; j < M; j += block_size) {
                
                // 内层循环：遍历块内的每一个元素
                // ★★★ 关键点：必须加 boundary check (row < N 和 col < M)
                for (row = i; row < i + block_size && row < N; row++) {
                    for (col = j; col < j + block_size && col < M; col++) {
                        // 直接搬运即可，不需要复杂的局部变量缓存
                        // 这里的对角线冲突概率很低，直接赋值影响不大
                        B[col][row] = A[row][col];
                    }
                }
            }
        }
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

