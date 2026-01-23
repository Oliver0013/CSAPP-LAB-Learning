#include "cachelab.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

//1. 数据结构模拟cacheline
typedef struct{
    int valid;//有效位
    unsigned long tag;//标记位
    int time_stamp;//时间戳
}cacheLine;

//2. 二维数组模拟整个cache
cacheLine **cache;

//-------------------
//定义全局变量
//-------------------
//传入的参数
int s,E,b;
char* trace_file = NULL;//接受文件名
//组数
int S;
//计数器
int hit_cnt = 0; //命中
int miss_cnt = 0; //没命中
int eviction_cnt = 0; //替换
int cur_time = 0; //逻辑时间


//-------------------
//3. 给定地址处理cache
//-------------------
void accessCache(unsigned long addr){
    //addr: 标记 | 组索引 | 块偏移
    //   64-s-b位| s位    | b位
    //    63:s+b |s+b-1:b |  b-1:0
    //解析addr
    //标记
    unsigned long tag_cur = addr>>(s+b);
    //所属组，利用s位掩码
    int set_index = (addr>>b)&((1<<s)-1);

    //找对应的组
    cacheLine* target_set = cache[set_index];
    
    //组里遍历
    //---------------命中判断-----------------
    for (int i = 0; i < E; i++){
        cacheLine* cur_line = &target_set[i];
        //命中
        if (cur_line->valid == 1 && cur_line->tag == tag_cur){
            //更新时间戳
            cur_line->time_stamp = cur_time;
            hit_cnt++;
            return;
        }
    }
    //没命中
    miss_cnt++;

    //----------------找空位------------------
    //没命中？找空位
    //每组有E行
    for (int i = 0; i < E; i++){
        cacheLine* cur_line = &target_set[i];
        //找到空位
        if (cur_line->valid == 0){
            //直接填充
            cur_line->valid = 1;
            cur_line->tag = tag_cur;
            cur_line->time_stamp = cur_time;
            return;
        }
    }
    eviction_cnt++;

    //----------------替换--------------------
    //没空位？踢人, 采用LRU，将时间戳最小的踢走
    cacheLine* eviction_line = &target_set[0];
    for (int i = 0; i < E;i++){
        cacheLine* cur_line = &target_set[i];
        eviction_line = (cur_line->time_stamp < eviction_line->time_stamp)? cur_line:eviction_line;
    }
    //替换时间戳最小的
    eviction_line->tag = tag_cur;
    eviction_line->time_stamp = cur_time;
    eviction_line->valid = 1;
}

//4. 解析每一行，并且每行处理
int main(int argc, char* argv[]){
    //参数解析
    char opt;
    while ((opt = getopt(argc, argv, "s:E:b:t:v")) != -1){
        switch (opt){
            case 's': s = atoi(optarg); break;
            case 'E': E = atoi(optarg); break;
            case 'b': b = atoi(optarg); break;
            case 't': trace_file = optarg; break;

        }
    }
    //计算组数
    S = 1 << s;

    //--------初始化cache组----------
    cache = (cacheLine**)malloc(sizeof(cacheLine*) * S);
    //初始化每组的行
    for (int i = 0; i < S; i++){
        cache[i] = (cacheLine*)malloc(sizeof(cacheLine) * E);
        //遍历每行
        for (int j = 0; j < E; j++){
            cache[i][j].valid = 0;
            cache[i][j].tag = 0;
            cache[i][j].time_stamp = 0;
        }
    }

    //打开文件
    FILE* pfile = fopen(trace_file,"r");

    //检查文件是否为空
    if (pfile == NULL) {
        printf("打不开文件：%s\n", trace_file);
        return 1;
    }

    //------------循环读取文件-------------
    //准备容器接数据
    char operation;
    unsigned long addr;
    int size;

    while (fscanf(pfile, " %c %lx,%d", &operation, &addr, &size) >0){
        switch (operation){
            case 'I':
                break;
            case 'M':
                cur_time++;
                accessCache(addr);
                accessCache(addr);
                break;
            case 'L':
                cur_time++;
                accessCache(addr);
                break;
            case 'S':
                cur_time++;
                accessCache(addr);
                break;
        }
    }
    //关闭文件
    fclose(pfile);
    //---------------------------
    
    //打印结果
    printSummary(hit_cnt, miss_cnt, eviction_cnt);

    //释放内存
    //注意分配几层就要释放几层
    for (int i = 0; i<S; i++){
        free(cache[i]);
    }
    free(cache);
    
    return 0;
}
