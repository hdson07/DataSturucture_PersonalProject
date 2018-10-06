#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<time.h>



#define TRUE 1
#define FALSE 0

#define MaxAttraction 25

typedef struct FILE_HEADER {//데이터 파일의 헤더; 헤더의 구조체 //24
    char pname[20];
    char version[10];
    long recent;
}DFH;

typedef struct INDEX {
    int attractionNo;
    long offset;
    char flag;
}I_data;

typedef struct Operation_Time{ //자정기준, 분으로 저장 : [시간 * 60 + 분]
    int openTime;
    int closeTime;
}OP_Time;

typedef struct AttractionData {
    int no;
    char Aname[20];
    int StayTime;
    OP_Time WeekDay[7]; //0 : 일 ~ 6:토
}At_data;

int path[MaxAttraction][MaxAttraction];

char *datafile = "attraction.dat"; //15바이트
char *idxfile = "attraction.idx";
void printTitle(void);
void menu(void);
void swap(FILE * fp, long i, long j);
void qSort(FILE *fp, long left, long right);
int comp(FILE *fp, long i, long j);
void insert_attraction(void);
void list_attraction(void);
long get_count(void);
void display_member(At_data *a);
void init_file(void);
int saveTime(int intime){return(intime/100*60+intime%100);};
void showTime(int intime){printf("%d:%d",intime/60,intime%60);};
void show_attraction(void);
long fbin_search(int key, long n, FILE* fp);
int find_attraction(int attractionNo, At_data *a);
int set_delete(int attractionNo);
void delete_attraction(void);
int dayofweek(int);
void play_program(void);

int main(){
    char in, quit = FALSE;//종료하지 않겠다
    while (quit != TRUE) //참일동안 수행해라
    {
        //system("clear");
        printTitle();
        list_attraction();
        menu();
        printf("메뉴선택  :  ");
        scanf("%c%*c", &in);
        printf("\n");
        printf("\n");
        switch (toupper(in)) {
            case 'I':
                insert_attraction();
                break;
            case 'S':
                show_attraction();
                break;
            case 'N':
                init_file();
                break;
            case 'D':
                delete_attraction();
                break;
            case 'P':
                play_program();
                break;
            case 'Q':
                quit = TRUE;
                break;
            default:
                printf("메뉴 선택을 다시하시오!!\n");
        }
        
    }
    
    return 0;
}





void play_program(){
    
    //data_setting
    At_data wanna[10];
    At_data a;
    int i=0;
    int attractionNo;
    int found;
    printf("[가고싶은 관관지 : 최대 10 , 다 입력했을 경우 0입력\n");
    while(i<10){
        printf("가고싶은 관광지 번호를 입력하시오  : ");
        scanf("%d%*c", &attractionNo);
        if(attractionNo == 0)
            break;
        found = find_attraction(attractionNo, &a);
        
        if (!found) {
            printf("[%d] 해당 관광지가 없다\n", attractionNo);
            system("pause");
            return;
        }
        wanna[i]=a;
         i++;
    }
    int play_path[i][i];
    printf("=========가고싶은 관광지 목록=========\n");
    for(int j=0;j<i;j++){
        printf("관광지 번호 : %d 관광지 이름 : %s\n",wanna[j].no,wanna[j].Aname);
        for(int k=0;k<i;k++){
            play_path[j][k]=path[wanna[j].no][wanna[k].no];
        }
            
    }
    int day,time,startTime,startDay,finishTime,finishDay;
    printf("여행 시작 일자를 입력하시오 Ex. 20171005 \t :");
    scanf("%d%*c",&day);
    startDay=dayofweek(day);
    printf("여행 종료 일자를 입력하시오 Ex. 20171005 \t :");
    scanf("%d%*c",&day);
    finishDay=dayofweek(day);
    printf("여행 시작 시간을 입력하시오 Ex. 1020 \t :");
    scanf("%d%*c",&time);
    startTime=saveTime(time);
    printf("여행 종료 시간을 입력하시오 Ex. 2020 \t :");
    scanf("%d%*c",&time);
    finishTime=saveTime(time);
    
    //play
    
    
    
    
}//프로그램 실행

int dayofweek(int day){   int y, m, d;
    y=day/10000;
    m=(day%10000)/100;
    d=(day%10000)%100;
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= m < 3;
    return ((y + y/4 - y/100 + y/400 + t[m-1] + d) % 7);
}//20171006으로 입력한 정수를 날짜로 변환

int comp(FILE *fp, long i, long j) {//if (comp(fp,left,i)>0) 있으니 반환값이 있어야 된다. strcmp : 문자열 비교함수
    //앞(left) 뒤(right) left가 더 클경우 뒤바꾼다 : >0일때 뒤바꾼다.
    I_data idx1, idx2;
    fseek(fp, i * sizeof(I_data), 0);
    fread(&idx1, sizeof(I_data), 1, fp);
    fseek(fp, j * sizeof(I_data), 0);
    fread(&idx2, sizeof(I_data), 1, fp);
    
    if( idx1.attractionNo < idx2.attractionNo)
        return 1;
    else if( idx1.attractionNo > idx2.attractionNo)
        return -1;
    else return 0;
}

void qSort(FILE *fp, long left, long right) {
    long last, p;
    if (left >= right) return;//종료조건
    p = (left + right) / 2;//피벗구하기
    swap(fp, left, p);//파일에서 해야되니까 파일포인터 같이 넘겨준다
    last = left;
    for (long i = left + 1; i <= right; i++) {
        if (comp(fp, left, i) > 0)  //아이디를 비교하는 함수(comp)를 넣어야한다 (left 보다 i가 작다)
            //C에서 컴페어한다. strcmp(str1,str2) str1의 각 문자를 str2에 대응되는 문자에 빼기
            //==0 같다, >0 앞문자가 크다, <0 앞문자가 작다
            swap(fp, ++last, i);
    }
    swap(fp, left, last);
    qSort(fp, left, last - 1);
    qSort(fp, last + 1, right);
}

void swap(FILE * fp, long i, long j) {//인덱스파일 정렬중 fp=인덱스 파일 열려있는걸로 봐야된다
    I_data idx1, idx2;
    if (i == j) return; //함수를 만들땐 항상 종료조건을 알아야 한다.
    //fseek(i) i 위치를 찾은 후 읽는다, fseek(j) 위치를 찾은후 읽는다.
    //i,j 서로 바꿔서 저장 -> fseek(i) / fwrite(j) & fseek(j) fwrite(i) 두줄다 읽어내서 교차저장
    fseek(fp, i * sizeof(I_data), 0);
    fread(&idx1, sizeof(I_data), 1, fp); // i번째 레코드를 일근ㄴ다
    fseek(fp, j * sizeof(I_data), 0);
    fread(&idx2, sizeof(I_data), 1, fp); // j번째 레코드를 일근ㄴ다
    fseek(fp, i * sizeof(I_data), 0);
    fwrite(&idx2, sizeof(I_data), 1, fp); // i에 j를 넣는다
    fseek(fp, j * sizeof(I_data), 0);
    fwrite(&idx1, sizeof(I_data), 1, fp); // j에 i를 넣는다
    
    
}

int set_delete(int attractionNo) {
    FILE *fp;
    I_data idx;
    long n;
    fp = fopen(idxfile, "rb+");
    if (fp == NULL) {//파일 열기가 실패//
        printf("인덱스 파일(%s) 열기 실패!!\n", idxfile);
        exit(1);
    }
    n = fbin_search(attractionNo, get_count(), fp);
    if (n == EOF) {
        fclose(fp);
        return FALSE;
    }
    fseek(fp, sizeof(I_data)*n, 0);
    fread(&idx, sizeof(I_data), 1, fp);
    idx.flag = 'D';
    fseek(fp, sizeof(I_data)*n, 0);//fread를 하고 난 후에 위치는 다음줄로 이동하기 때문에 위치를 다시 검색해야한다
    fwrite(&idx, sizeof(I_data), 1, fp);
    fclose(fp);
    return TRUE;
} //index 에서 삭제

void delete_attraction(void) {
    At_data a;
    int attractionNo;
    int found;
    printf("\n 관광지 번호 입력 :");
    scanf("%d%*c", &attractionNo);
    found = find_attraction(attractionNo, &a);
    if (!found) {
        printf("[%d] 관광지가 없다\n", attractionNo);
        system("pause");
        return;
    }
    printf("삭제하시겠습니까?(Y/N :");
    char sel;
    scanf("%c%*c", &sel);
    switch (toupper(sel))
    {
        case'N':
            return;
        case'Y':
            if (set_delete(attractionNo)) {
                printf("[%d]가 삭제되었습니다.\n", attractionNo);
            }
            else {
                printf("[%d] 삭제가 실패했습니다\n", attractionNo);
            }
            break;
        default:
            printf("잘못선택했습니다\n");
    }
    system("pause");
    return;
} //해당 관광지를 찾아 set_delete를 호출하여 삭제

long fbin_search(int key, long n, FILE* fp) {
    long left = 0L, right = n, mid;
    I_data idx;
    
    while (left <= right) {
        mid = (left + right) / 2;
        fseek(fp, sizeof(I_data)*mid, 0);
        fread(&idx, sizeof(I_data), 1, fp);
        if (key == idx.attractionNo)
            return mid;//문자열이기 때문에 논리함수 x 앞에게 한개라도 크면 1, 뒤에게 크면 -1
        else if (key <idx.attractionNo)
            left = mid + 1;
        else right = mid - 1;
    }
    return EOF;
} //인덱스에서 탐색

int find_attraction(int attractionNo, At_data *a) {
    //인덱스파일에서 찾아서 옵셋값만 넘겨준다
    FILE *fp;
    I_data idx;
    long n;
    fp = fopen(idxfile, "rb"); // 파일열기 파일열때 단순히 열지말고 열린거 확인할 수 있도록 이것처럼 열기
    if (fp == NULL) {//파일 열기가 실패//
        printf("인덱스 파일(%s) 열기 실패!!\n", idxfile);
        exit(1);
    }
    n = fbin_search(attractionNo, get_count(), fp);//인덱스파일에서 데이터의 위치만 찾는다 idx.offset
    if (n == EOF) {
        fclose(fp);
        return FALSE;
    }
    fseek(fp, sizeof(I_data)*n, 0);
    fread(&idx, sizeof(I_data), 1, fp);
    fclose(fp);
    if (idx.flag == 'D') return FALSE;
    fp = fopen(datafile, "rb");
    if (fp == NULL) {//파일 열기가 실패//
        printf("데이터 파일(%s) 열기 실패!!\n", datafile);
        exit(1);
    }
    fseek(fp, idx.offset, 0);
    fread(a, sizeof(At_data), 1, fp);
    fclose(fp);
    return TRUE;
}//인덱스에서 위치를 찾아 데이터파일에서 접근

void show_attraction(){
    At_data a;
    int attractionNo;
    int found;
    printf("\n 관광지 번호 입력 :");
    scanf("%d%*c", &attractionNo);
    found = find_attraction(attractionNo, &a);
    if (!found) {
        printf("%d 사용자가 없다\n", attractionNo);
        system("pause");
        return;
    }
    printf("관광지 번호 : %d\n",a.no);
    printf("관광지 이름  : %s \n",a.Aname);
    printf("====================운영시간======================\n");
    printf("월요일 :  "); showTime(a.WeekDay[0].openTime); printf("   ~   "); showTime(a.WeekDay[0].closeTime); printf("\n");
    printf("화요일 :  "); showTime(a.WeekDay[1].openTime); printf("   ~   "); showTime(a.WeekDay[1].closeTime); printf("\n");
    printf("수요일 :  "); showTime(a.WeekDay[2].openTime); printf("   ~   "); showTime(a.WeekDay[2].closeTime); printf("\n");
    printf("목요일 :  "); showTime(a.WeekDay[3].openTime); printf("   ~   "); showTime(a.WeekDay[3].closeTime); printf("\n");
    printf("금요일 :  "); showTime(a.WeekDay[4].openTime); printf("   ~   "); showTime(a.WeekDay[4].closeTime); printf("\n");
    printf("토요일 :  "); showTime(a.WeekDay[5].openTime); printf("   ~   "); showTime(a.WeekDay[5].closeTime); printf("\n");
    printf("일요일 :  "); showTime(a.WeekDay[6].openTime); printf("   ~   "); showTime(a.WeekDay[6].closeTime); printf("\n");
    
    system("pause");
    return;
}//관광지 세부정보를 보여준다.

void display_member(At_data *a) {
    printf("> 번 호   : [%d]", a->no);
    printf("\t 회원ID  : %s \n", a->Aname);
    return;
}

long get_count(void) {
    FILE *fp;
    DFH dfh;
    fp = fopen(datafile, "rb");
    if (fp == NULL) {//파일 열기가 실패//
        printf("데이터 파일(%s) 열기 실패!!\n", datafile);
        exit(1);
    }
    fseek(fp, 0L, 0);//파일을 탐색하겠다. SEEK_SET 0처음부터,  SEEK_CUR 1, SEEK_END 2 끝에서
    fread(&dfh, sizeof(DFH), 1, fp);
    fclose(fp);
    return dfh.recent;
}//list 개수 파악

void list_attraction(void) {
    FILE *fpi, *fpd;
    I_data idx;
    At_data a;
    long nrec;
    fpi = fopen(idxfile, "rb"); // 파일열기 파일열때 단순히 열지말고 열린거 확인할 수 있도록 이것처럼 열기
    if (fpi == NULL) {//파일 열기가 실패//
        printf("데이터 파일(%s) 열기 실패!!\n", datafile);
        exit(1);
    }
    fpd = fopen(datafile, "rb");
    if (fpd == NULL) {//파일 열기가 실패//
        printf("데이터 파일(%s) 열기 실패!!\n", datafile);
        exit(1);
    }
    printf("[여]\t\t[행]\t\t[지]\t\t[목]\t\t\[록]\n");
    nrec = get_count();
    for (long i = 0; i < nrec; i++) {
        fseek(fpi, sizeof(I_data)*i, 0);
        fread(&idx, sizeof(I_data), 1, fpi);
        if (idx.flag != 'D') {
            fseek(fpd, idx.offset, 0);
            fread(&a, sizeof(At_data), 1, fpd);
            display_member(&a);
            
        }
    }
    
    fclose(fpi);
    fclose(fpd);
    for (int i = 0; i < 78; i++) putchar('=');
    printf(" \n");
    return;
} //관광지 리스트 보여주기

void init_file(void) {
    DFH dfh = { "MAG", "0.9", 0 };
    FILE *fp;
    dfh.version[3] = 0x1a;//파일의 끝인 EOF (26값=NULL)
    fp = fopen(datafile, "wb");
    if (fp == NULL) {//파일 열기가 실패//
        printf("데이터 파일(%s) 열기 실패!!\n", datafile);
        exit(1);
    }
    fwrite(&dfh, sizeof(DFH), 1, fp);
    //fwrite, fread : 이진파일의 입출력
    fclose(fp);
    fp = fopen(idxfile, "wb");//w(wrinte) 모드로 열어야 파일이 생성
    if (fp == NULL) {
        printf("데이터파일(%s) 열기 실패!!\n", idxfile);
        exit(1);
    }
    fclose(fp);
    printf("파일생성 완료!!\n");
    system("pause");
    return;
    
}//데이터파일 초기화

void save_data(At_data* a) {
    FILE *fp;
    DFH dfh;
    I_data idx;
    long offset = 0L;//정수로 주면 인식 X 롱타입 0으로
    fp = fopen(datafile, "rb+");
    if (fp == NULL) {//파일 열기가 실패//
        printf("데이터 파일(%s) 열기 실패!!\n", datafile);
        exit(1);
    }
    fseek(fp, 0L, 0);//파일을 탐색하겠다. SEEK_SET 0처음부터,  SEEK_CUR 1, SEEK_END 2 끝에서
    fread(&dfh, sizeof(DFH), 1, fp);
    offset = sizeof(DFH) + sizeof(At_data)*dfh.recent;
    fseek(fp, offset, SEEK_SET);
    fwrite(a, sizeof(At_data), 1, fp);
    //데이터가 추가되었으므로 데이터개수를 갱신하여 헤더의 내용도 갱신해야한다
    dfh.recent++;
    fseek(fp, 0L, SEEK_SET);
    fwrite(&dfh, sizeof(DFH), 1, fp);
    fclose(fp);
    //인덱스 파일 처리
    idx.attractionNo=a->no;
    idx.offset = offset;
    idx.flag = '\0';
    fp = fopen(idxfile, "rb+");
    if (fp == NULL) {//파일 열기가 실패//
        printf("데이터 파일(%s) 열기 실패!!\n", idxfile);
        exit(1);
    }
    fseek(fp, sizeof(I_data)*(dfh.recent - 1), 0);
    fwrite(&idx, sizeof(I_data), 1, fp);
    qSort(fp, 0, dfh.recent - 1);
    fclose(fp);
    printf("\n 데이터를 파일에 저장하였습니다.\n");
    return;
}//데이터를 저장

void insert_attraction(void) {
    //데이터 파일 열고, 헤어 읽고, count 1증가, 위치이동, 데이터파일 증가, 헤더파일 갱신, 인덱스파일로 이동, 인데스파일 저장, 정렬
    //멤버의 데이터 입력 받기
    At_data a;
    int intime;
    printf("가입할 관광지의 정보를 입력하시오\n");
    printf("\n\t>번호 : ");
    scanf("%d%*c", &a.no);
    printf("\n\t > 이름 : ");
    scanf("%s", a.Aname);
    printf("\n\t > 평균 관람시간 : ");
    scanf("%d%*c", &a.StayTime);
    printf("\n\t > 월요일 오픈시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[0].openTime=saveTime(intime);
    printf("\n\t > 월요일 마감시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[0].closeTime=saveTime(intime);
    printf("\n\t > 화요일 오픈시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[1].openTime=saveTime(intime);
    printf("\n\t > 화요일 마감시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[1].closeTime=saveTime(intime);
    printf("\n\t > 수요일 오픈시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[2].openTime=saveTime(intime);
    printf("\n\t > 수요일 마감시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[2].closeTime=saveTime(intime);
    printf("\n\t > 목요일 오픈시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[3].openTime=saveTime(intime);
    printf("\n\t > 목요일 마감시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[3].closeTime=saveTime(intime);
    printf("\n\t > 금요일 오픈시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[4].openTime=saveTime(intime);
    printf("\n\t > 금요일 마감시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[4].closeTime=saveTime(intime);
    printf("\n\t > 토요일 오픈시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[5].openTime=saveTime(intime);
    printf("\n\t > 토요일 마감시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[5].closeTime=saveTime(intime);
    printf("\n\t > 일요일 오픈시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[6].openTime=saveTime(intime);
    printf("\n\t > 일요일 마감시간 : ");
    scanf("%d%*c", &intime);
    a.WeekDay[6].closeTime=saveTime(intime);
        printf("\n s]저장, m]메인");
    
    char sel, del[10];
    scanf("%c", &sel);
    switch (toupper(sel))
    {
        case 'S':
            save_data(&a);
            break;
    }
    gets(del);
}//입력받을 데이서 세팅

void menu(void) {
    printf("I) 관광지 등록\n");
    printf("S) 관광지 세부정보 보기\n");
    printf("D) 관광지 삭제\n");
    printf("P) 프로그램 시작\n");
    printf("Q) 종료\n \n");
}//메뉴 출력

void printTitle(void) {
    int i;
    char *t = "My Tour Guide - Duck's Program";
    for (i = 0; i < 78; i++) putchar('=');
    printf(" \n%s \n", t);
    for (i = 0; i < 78; i++) putchar('=');
    printf(" \n \n");
    return;
}//제목출력

