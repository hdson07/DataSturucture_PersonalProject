#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
//줄안맞을 때 ctrl k f
#define TRUE 1
#define FALSE 0
typedef struct FILE_HEADER {//데이터 파일의 헤더; 헤더의 구조체 //24
    char pname[10];
    char version[10];
    long recent;
}DFH;

typedef struct MEMBER {
    int no;
    char userid[10];
    char passwd[10];
    char name[10];
    char gender[3];
    int age;
    char position[20];
    char company[30];
    char address[40];
    char zipcode[6];
    char tel[15];//000-0000-0000
    char cel[15];
}M_REC;

typedef struct INDEX {//데이터 파일 성능저하 스 위한 인덱스 스일 생성 >> 정렬이 필요
    char userid[10];
    long offset;//상대적인 위치 0~23(헤더의 위치) 24 191 358....... 한명의 데이타가 구조체 사이즈만큼생성
    //24=sizeof(DFH) + sizeof(M_REC)*dfh.recnt
    char flag;
}I_REC;

char *datafile = "member.dat"; //15바이트
char *idxfile = "member.idx";
void printTitle(void);
void menu(void);
void init_file(void);
void insert_member(void);
void save_data(M_REC*);
void qSort(FILE *, long left, long right);
void swap(FILE *, long, long);
int comp(FILE *, long, long);
void view_help(void);
void list_member(void);
long get_count(void);
void display_member(M_REC *);
void moidify_member(void);
int find_member(char *, M_REC *);
long fbin_search(char *, long, FILE*);//seekset이 롱타입 변수를 필요로 한다
long get_offset(char *);
void serch_member(void);
void delete_member(void);
int set_delete(char *);

void indexing(void);

int main(void) {
    char in, quit = FALSE;//종료하지 않겠다
    while (quit != TRUE) //참일동안 수행해라
    {
        system("cls");//화면삭제
        printTitle();
        menu();
        printf("메뉴선택");
        scanf("%c%*c", &in);
        printf("\n");
        switch (toupper(in)) {
            case 'I':
                insert_member();
                break;
            case 'M':
                moidify_member();
                break;
            case 'D':
                delete_member();
                break;
            case 'S':
                serch_member();
                break;
            case 'L':
                list_member();
                break;
            case 'N':
                init_file();
                break;
            case 'X':
                indexing();
                break;
            case 'H':
                view_help();
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
    
}

void indexing(void) {
    FILE *fpi, *fpd, *fpinew, *fpdnew;
    I_REC idx;
    M_REC m;
    DFH dfh = { "MMS","0.9",0 };
    dfh.version[3] = 0x1a; //데이터파일을 열었을때 더이상 보지 못하게 하겠다
    long nioff = 0l, ndoff = 0l, i, nrec, wcnt = 0;//인데스.데이터파일 오프세, 반복문,레코드갑 반복 / 남아있는 데이타 파일을 반복
    char *newidxfile = "nmember.dat";
    char *newdatafile = "nmember.dat";
    fpi = fopen(idxfile, "rb");
    if (fpi == NULL) {//파일 열기가 실패//
        printf("인덱스 파일(%s) 열기 실패!!\n", idxfile);
        exit(1);
    }
    fpd = fopen(datafile, "rb");
    if (fpd == NULL) {//파일 열기가 실패//
        printf("데이터 파일(%s) 열기 실패!!\n", datafile);
        exit(1);
    }
    fpinew = fopen(newidxfile, "wb+");
    if (fpinew == NULL) {//파일 열기가 실패//
        printf("인덱스 파일(%s) 열기 실패!!\n", newidxfile);
        exit(1);
    }
    fpdnew = fopen(newdatafile, "wb+");
    if (fpdnew == NULL) {//파일 열기가 실패//
        printf("epdlxk 파일(%s) 열기 실패!!\n", newdatafile);
        exit(1);
    }
    fseek(fpdnew, 0L, 0);
    fwrite(&dfh, sizeof(DFH), 1, fpdnew);
    nrec = get_count();
    for (i = 0; i < nrec; i++) {
        fseek(fpi, sizeof(I_REC)*i, 0);
        fread(&idx, sizeof(I_REC), 1, fpi);
        if (idx.flag != 'D') {//삭제된 데이터가 아닌경우 인덱스 데이터를 새파일로 넘겨준다
            fseek(fpd, idx.offset, 0);
            fread(&m, sizeof(M_REC), 1, fpd);
            ndoff = sizeof(DFH) + sizeof(M_REC)*wcnt;
            fseek(fpdnew, ndoff, 0);
            fwrite(&m, sizeof(M_REC), 1, fpdnew);
            idx.offset = ndoff;
            nioff = sizeof(I_REC)*wcnt;
            fseek(fpinew, nioff, 0);
            fwrite(&idx, sizeof(I_REC), 1, fpinew);
            wcnt++;//새로운 파일에 저장될 데이터 개수
        }
    }
    //데이터파일 헤더 갱신
    dfh.recent = wcnt;
    fseek(fpdnew, 0l, 0);
    fwrite(&dfh, sizeof(DFH), 1, fpdnew);
    fclose(fpi);
    fclose(fpd);
    fclose(fpinew);
    fclose(fpdnew);
    printf("파일정리중...\n");
    if (remove(idxfile) == 0)//stdlib 파일삭제
        printf("인덱스파일[%s]이 삭제되었습니다\n", idxfile);
    else
        perror("인덱스파일이 삭제 실패 되었습니다\n");//에러메세지 출력
    
    if (remove(datafile) == 0)//stdlib 파일삭제
        printf("데이터파일[%s]이 삭제되었습니다\n", datafile);
    else
        perror("데이터파일[%s]이 삭제 실패 되었습니다\n");//에러메세지 출력
    
    if (rename(newidxfile, idxfile) == 0)//stdlib 파일삭제
        printf("[%s]=>[%s]\n", newidxfile, idxfile);
    else
        perror("인덱스파일이 이름변경이 실패 되었습니다\n");//에러메세지 출력
    if (rename(newdatafile, datafile) == 0)//stdlib 파일삭제
        printf("[%s]=>[%s]\n", newdatafile, datafile);
    else
        perror("데이터파일이 이름변경이 실패 되었습니다\n");//에러메세지 출력
    printf("파일정리 완료");
    system("pause");
}
void delete_member(void) {
    M_REC m;
    char userid[10];
    int found;
    printf("\n 사용자 ID 입력 :");
    scanf("%s%*c", userid);
    found = find_member(userid, &m);
    if (!found) {
        printf("%s 사용자가 없다\n", userid);
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
            if (set_delete(userid)) {
                printf("%s가 삭제되었습니다.\n", userid);
            }
            else {
                printf("%s 삭제가 실패했습니다\n", userid);
            }
            break;
        default:
            printf("잘못선택했습니다\n");
    }
    system("pause");
    return;
}

int set_delete(char *userid) {
    FILE *fp;
    I_REC idx;
    long n;
    fp = fopen(idxfile, "rb+");
    if (fp == NULL) {//파일 열기가 실패//
        printf("인덱스 파일(%s) 열기 실패!!\n", idxfile);
        exit(1);
    }
    n = fbin_search(userid, get_count(), fp);
    if (n == EOF) {
        fclose(fp);
        return FALSE;
    }
    fseek(fp, sizeof(I_REC)*n, 0);
    fread(&idx, sizeof(I_REC), 1, fp);
    idx.flag = 'D';
    fseek(fp, sizeof(I_REC)*n, 0);//fread를 하고 난 후에 위치는 다음줄로 이동하기 때문에 위치를 다시 검색해야한다
    fwrite(&idx, sizeof(I_REC), 1, fp);
    fclose(fp);
    return TRUE;
}
void serch_member(void) {
    M_REC m;
    char userid[10];
    int found;
    printf("\n 사용자 ID 입력 :");
    scanf("%s%*c", userid);
    found = find_member(userid, &m);
    if (!found) {
        printf("%s 사용자가 없다\n", userid);
        system("pause");
        return;
    }
    display_member(&m);
    system("pause");
    return;
}
void moidify_member() {
    FILE *fp;
    M_REC m;
    char userid[10], field[40];//막연할땐 있는 변수중 가장 큰 배열로 잡는게 맞다
    int found, age, sel;
    printf("\n 사용자 ID 입력 :");
    scanf("%s%*c", userid);
    found = find_member(userid, &m);
    if (!found) {
        printf("%s 사용자가 없다\n", userid);
        system("pause");
        return;
    }
    display_member(&m);
    printf("수정항목 선택 :");
    scanf("%d%*C", &sel);
    printf("데이터 입력  :");
    if (sel == 4)
        scanf("%d", &age);//나이를 제외한 전부는 문자열
    else
        gets(field);
    switch (sel) {
        case 1:
            strcpy(m.passwd, field);
            //배열명은 포인터 상수이므로 할당연산자 사용불가
            break;
        case 2:
            strcpy(m.name, field);
            break;
        case 3:
            strcpy(m.gender, field);
            break;
        case 4:
            m.age = age;
            break;
        case 5:
            strcpy(m.position, field);
            break;
        case 6:
            strcpy(m.company, field);
            break;
        case 7:
            strcpy(m.address, field);
            break;
        case 8:
            strcpy(m.zipcode, field);
            break;
        case 9:
            strcpy(m.tel, field);
            break;
        case 10:
            strcpy(m.cel, field);
            break;
            
    }
    fp = fopen(datafile, "rb+"); // 파일열기 파일열때 단순히 열지말고 열린거 확인할 수 있도록 이것처럼 열기
    if (fp == NULL) {//파일 열기가 실패//
        printf("데이터 파일(%s) 열기 실패!!\n", datafile);
        exit(1);
    }
    fseek(fp, get_offset(userid), 0);//fseek 가운데값 롱 타입
    fwrite(&m, sizeof(M_REC), 1, fp);
    fclose(fp);
    return;
}
int find_member(char *userid, M_REC *m) {
    //인덱스파일에서 찾아서 옵셋값만 넘겨준다
    FILE *fp;
    I_REC idx;
    long n;
    fp = fopen(idxfile, "rb"); // 파일열기 파일열때 단순히 열지말고 열린거 확인할 수 있도록 이것처럼 열기
    if (fp == NULL) {//파일 열기가 실패//
        printf("인덱스 파일(%s) 열기 실패!!\n", idxfile);
        exit(1);
    }
    n = fbin_search(userid, get_count(), fp);//인덱스파일에서 데이터의 위치만 찾는다 idx.offset
    if (n == EOF) {
        fclose(fp);
        return FALSE;
    }
    fseek(fp, sizeof(I_REC)*n, 0);
    fread(&idx, sizeof(I_REC), 1, fp);
    fclose(fp);
    if (idx.flag == 'D') return FALSE;
    fp = fopen(datafile, "rb");
    if (fp == NULL) {//파일 열기가 실패//
        printf("데이터 파일(%s) 열기 실패!!\n", datafile);
        exit(1);
    }
    fseek(fp, idx.offset, 0);
    fread(m, sizeof(M_REC), 1, fp);
    fclose(fp);
    return TRUE;
}
long fbin_search(char *key, long n, FILE* fp) {
    long left = 0L, right = n, mid;
    I_REC idx;
    
    while (left <= right) {
        mid = (left + right) / 2;
        fseek(fp, sizeof(I_REC)*mid, 0);
        fread(&idx, sizeof(I_REC), 1, fp);
        if (strcmp(key, idx.userid) == 0)
            return mid;//문자열이기 때문에 논리함수 x 앞에게 한개라도 크면 1, 뒤에게 크면 -1
        else if (strcmp(key, idx.userid) > 0)
            left = mid + 1;
        else right = mid - 1;
    }
    return EOF;
}
long get_offset(char *userid) {
    FILE *fp;
    I_REC idx;
    long n;
    fp = fopen(idxfile, "rb"); // 파일열기 파일열때 단순히 열지말고 열린거 확인할 수 있도록 이것처럼 열기
    if (fp == NULL) {//파일 열기가 실패// get_count
        printf("데이터 파일(%s) 열기 실패!!\n", datafile);
        exit(1);
    }
    n = fbin_search(userid, get_count(), fp);
    if (n == EOF) {
        fclose(fp);
        return FALSE;//? EOF
    }
    fseek(fp, sizeof(I_REC)*n, 0);
    fread(&idx, sizeof(I_REC), 1, fp);
    fclose(fp);
    return idx.offset;
}
void list_member(void) {
    FILE *fpi, *fpd;
    I_REC idx;
    M_REC m;
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
    nrec = get_count();
    for (long i = 0; i < nrec; i++) {
        fseek(fpi, sizeof(I_REC)*i, 0);
        fread(&idx, sizeof(I_REC), 1, fpi);
        if (idx.flag != 'D') {
            fseek(fpd, idx.offset, 0);
            fread(&m, sizeof(M_REC), 1, fpd);
            display_member(&m);
            system("pause");
        }
    }
    fclose(fpi);
    fclose(fpd);
    return;
}
void display_member(M_REC *m) {
    printf("> 번 호   : %d \n", m->no);
    printf("> 회원ID  : %s \n", m->userid);
    printf(" 1. 비밀번호 : %s \n", m->passwd);
    printf(" 2. 이 름  : %s \n", m->name);
    printf(" 3. 성 별  : %s \n", m->gender);
    printf(" 4. 나 이  : %d \n", m->age);
    printf(" 5. 직 위  : %s \n", m->position);
    printf(" 6. 회사명  : %s \n", m->company);
    printf(" 7. 주 소  : %s \n", m->address);
    printf(" 8. 우편번호 : %s \n", m->zipcode);
    printf(" 9. 전화번호 : %s \n", m->tel);
    printf("10. 휴대폰  : %s \n", m->cel);
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
}
void view_help() {
    printf("----------------- HELP MESSAGE ---------------------------\n");
    printf("프로그램을 처음 사용하실 때는 색인 파일(member.idx)과\n");
    printf("데이터 파일(member.dat)이 존재하지 않습니다.\n");
    printf("따라서, 'N'을 눌러 시스템 초기화한 다음 사용하십시오.\n");
    printf("단, 데이터 파일이 존재할 경우에 이 작업을 하시면 기존의 데이터를\n");
    printf("잃어버리게 되므로 주의해야 합니다.\n");
    printf("저장 데이터가 많아지면 'X'를 눌러 자료를 정리하십시오.\n");
    printf("자료를 정리하시면 데이터 검색속도가 향상됩니다.\n");
    system("pause");
}
void insert_member(void) {
    //데이터 파일 열고, 헤어 읽고, count 1증가, 위치이동, 데이터파일 증가, 헤더파일 갱신, 인덱스파일로 이동, 인데스파일 저장, 정렬
    //멤버의 데이터 입력 받기
    M_REC m;
    printf("가입할 회원의 기본정보 입력 하시오\n");
    printf("\n\t>번호 : ");
    scanf("%d%*c", &m.no);
    printf("\n\t > 회원ID : ");
    scanf("%s", m.userid);
    printf("\n\t > 비밀번호 : ");
    scanf("%s", m.passwd);
    printf("\n\t > 이름 : ");
    scanf("%s", m.name);
    printf("\n\t > 성별 : ");
    scanf("%s", m.gender);
    printf("\n\t > 나이 : ");
    scanf("%d", &m.age);
    printf("\n\t > 직위 : ");
    scanf("%s", m.position);
    printf("\n\t > 회사명 : ");
    scanf("%s%*c", m.company);
    printf("\n\t > 주소 : ");
    gets(m.address);
    printf("\n\t > 우편번호 : ");
    scanf("%s", m.address);
    printf("\n\t > 전화번호 : ");
    scanf("%s", m.tel);
    printf("\n\t > 휴대폰 : ");
    scanf("%s%*c", m.cel);//%*C 이 위치에 나타나는 문자 하나를 없애버리겠다 (휴대폰치고의 enter가 다음문자 sel 들어가서 이걸로 씀
    printf("\n s]저장, m]메인");
    char sel, del[10];
    scanf("%c", &sel);
    switch (toupper(sel))
    {
        case 'S':
            save_data(&m);
            break;
    }
    gets(del);
}
void save_data(M_REC* m) {//선언할땐 타입만, 정의할땐 변수필요
    FILE *fp;
    DFH dfh;
    I_REC idx;
    long offset = 0L;//정수로 주면 인식 X 롱타입 0으로
    fp = fopen(datafile, "rb+");
    if (fp == NULL) {//파일 열기가 실패//
        printf("데이터 파일(%s) 열기 실패!!\n", datafile);
        exit(1);
    }
    fseek(fp, 0L, 0);//파일을 탐색하겠다. SEEK_SET 0처음부터,  SEEK_CUR 1, SEEK_END 2 끝에서
    fread(&dfh, sizeof(DFH), 1, fp);
    offset = sizeof(DFH) + sizeof(M_REC)*dfh.recent;
    fseek(fp, offset, SEEK_SET);
    fwrite(m, sizeof(M_REC), 1, fp);
    //데이터가 추가되었으므로 데이터개수를 갱신하여 헤더의 내용도 갱신해야한다
    dfh.recent++;
    fseek(fp, 0L, SEEK_SET);
    fwrite(&dfh, sizeof(DFH), 1, fp);
    fclose(fp);
    //인덱스 파일 처리
    strcpy(idx.userid, m->userid);
    idx.offset = offset;
    idx.flag = '\0';
    fp = fopen(idxfile, "rb+");
    if (fp == NULL) {//파일 열기가 실패//
        printf("데이터 파일(%s) 열기 실패!!\n", idxfile);
        exit(1);
    }
    fseek(fp, sizeof(I_REC)*(dfh.recent - 1), 0);
    fwrite(&idx, sizeof(I_REC), 1, fp);
    qSort(fp, 0, dfh.recent - 1);
    fclose(fp);
    printf("\n 데이터를 파일에 저장하였습니다.\n");
    return;
}
void init_file(void) {
    DFH dfh = { "MMS", "0.9", 0 };
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
int comp(FILE *fp, long i, long j) {//if (comp(fp,left,i)>0) 있으니 반환값이 있어야 된다. strcmp : 문자열 비교함수
    //앞(left) 뒤(right) left가 더 클경우 뒤바꾼다 : >0일때 뒤바꾼다.
    I_REC idx1, idx2;
    fseek(fp, i * sizeof(I_REC), 0);
    fread(&idx1, sizeof(I_REC), 1, fp);
    fseek(fp, j * sizeof(I_REC), 0);
    fread(&idx2, sizeof(I_REC), 1, fp);
    return strcmp(idx1.userid, idx2.userid);
}
void swap(FILE * fp, long i, long j) {//인덱스파일 정렬중 fp=인덱스 파일 열려있는걸로 봐야된다
    I_REC idx1, idx2;
    if (i == j) return; //함수를 만들땐 항상 종료조건을 알아야 한다.
    //fseek(i) i 위치를 찾은 후 읽는다, fseek(j) 위치를 찾은후 읽는다.
    //i,j 서로 바꿔서 저장 -> fseek(i) / fwrite(j) & fseek(j) fwrite(i) 두줄다 읽어내서 교차저장
    fseek(fp, i * sizeof(I_REC), 0);
    fread(&idx1, sizeof(I_REC), 1, fp); // i번째 레코드를 일근ㄴ다
    fseek(fp, j * sizeof(I_REC), 0);
    fread(&idx2, sizeof(I_REC), 1, fp); // j번째 레코드를 일근ㄴ다
    fseek(fp, i * sizeof(I_REC), 0);
    fwrite(&idx2, sizeof(I_REC), 1, fp); // i에 j를 넣는다
    fseek(fp, j * sizeof(I_REC), 0);
    fwrite(&idx1, sizeof(I_REC), 1, fp); // j에 i를 넣는다
    
    
}

void menu(void) {
    printf("I) 회원등록\n");
    printf("M) 회원정보 수정\n");
    printf("D) 회원정보 삭제\n");
    printf("S) 회원검색\n");
    printf("L) 회원목록\n");
    printf("N) 시스템초기화\n");
    printf("X) 자료정리\n");
    printf("H) 도움말\n");
    printf("Q) 종료\n \n");
}
void printTitle(void) {
    int i;
    char *t = "MMS(Membership Management System) Duck's Program";
    for (i = 0; i < 78; i++) putchar('=');
    printf(" \n%s \n", t);
    for (i = 0; i < 78; i++) putchar('=');
    printf(" \n \n");
    return;
}
