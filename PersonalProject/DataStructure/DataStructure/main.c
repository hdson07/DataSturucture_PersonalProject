#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>


#define TRUE 1
#define FALSE 0

#define MaxAttraction 25

typedef struct FILE_HEADER {//데이터 파일의 헤더; 헤더의 구조체 //24
    char pname[10];
    char version[10];
    long recent;
}DFH;

typedef struct Operation_Time{ //자정기준, 분으로 저장 : [시간 * 60 + 분]
    int openTime;
    int closeTime;
}OP_Time;

typedef struct AttractionData {
    int no;
    char Aname[20];
    char StayTime[10];
    OP_Time WeekDay[7]; //0 : 월 ~ 6:일
}At_data;


int path[MaxAttraction][MaxAttraction];




int main(){




}
