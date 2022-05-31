#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;


double uniform(double a, double b, long int * seed)
{
       double t;
       *seed = (2045 * (*seed) + 1) % 1048576;
       t = (*seed)/1048576.0;
       t = a + (b - a) * t; //生成(a,b)开区间范围内的浮点数
       return (t);
}

void generateA1(int N){
    char metachar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"; 
    int num_file = 1;
    
    
    FILE *fp;
    srand((unsigned) time(0));
    long int seed = rand();

    if ((fp = fopen("data/A1.txt", "w")) == NULL){
        printf("文件打开失败。\n"); 
    }
    int num = 0;    //每次生成的随机整数
    for (int i = 0; i < N; i++) { 
        num = uniform(0, 53, &seed);//生成在开区间（0,2）上均匀分布的随机数, 取整后得到均匀分布的0/1
        char c = metachar[num];

        cout<<c;
        fprintf(fp,"%c",c);
    }
    cout<<endl << "A1"<< "写入成功"<<endl;

    fclose(fp);
}
void genetate_others(int num_file, int N){
    FILE *fpread, *fpwrite;
    char metachar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"; 

    char Apre[N];
    for (int f = 2; f <= num_file; f++){ // 写Ai
        char fname_read[20];
        char fname_write[20];
        sprintf(fname_read,"data/A%d.txt",f-1); //读取A i-1
        sprintf(fname_write,"data/A%d.txt",f); //写A i
        fpread = fopen(fname_read,"r");
        fpwrite = fopen(fname_write,"w");
        //随机选择需要替换的位置
        long int seed = rand();
        int index = uniform(0, N+1, &seed);    //生成在开区间（0,N+1）上均匀分布的随机数, 取整后得到均匀分布的0/1
        
        ////随机生成一个字符 用于替换
        int num = uniform(0, 53, &seed);
        char replace_c = metachar[num];    
        
        //读取前一个A
        for(int i = 0; i < N; i++){
            fscanf(fpread, "%c", &Apre[i]);
            
            //替换选取的字符
            if(i == index){
                Apre[index] = replace_c;
            }
            //printf("%c", Apre[i]);
            fprintf(fpwrite,"%c",Apre[i]);
        }
        //cout<<endl<<"index:" << index <<"  " << Apre[index]<<endl;
        fclose(fpread);
        fclose(fpwrite);
        
    }
}

void test_jaccard(){
    FILE *fpread, *fpread2;
    char metachar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"; 
    fpread = fopen("data_1/A1.txt","r");
    fpread2 = fopen("data_1/A2.txt","r");
    int N = 500;
    char Apre[N];
    char Anow[N];
    int jiao=0, bing = 0;
    for(int i = 0 ; i <N; i++){
        Apre[i] = 'v';
        fscanf(fpread, "%c", &Apre[i]); 
        fscanf(fpread2, "%c", &Anow[i]); 
        if(Apre[i] == Anow[i]){
            jiao++;
        }else{
            bing++;
            printf("index of different char : %d   %c \n", i , Apre[i]);
        }
        
    }
    bing = jiao + bing;
    printf("jaccard:%f\n",jiao/(bing*1.0));

        
}
int main() {
    int N =500;
    int num_file = 1000;
    // test_jaccard();
    
    generateA1(N);
    genetate_others(num_file,N);
}

