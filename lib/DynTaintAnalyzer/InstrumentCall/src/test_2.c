#include <stdio.h>
#include <string.h>
//#include "library.h"



int global=0;
//void* retAddr;

struct A{
    int a;
    float b;
};


int foo(){

    //UpdateTaintFunSet(&global,4,"foo");
    return global;
}

int foo2(int b){

    int a=b+3;


    //retAddr=&a;
//UpdateTaintFunSet(&a,4,"foo2");
    return a;
}



int main()
{
    int input=0;
    scanf("%d",&input);

    //instr
//
//
//    int a=3;
//
//    int second=input - 2;
//    //instr
//
//
//    int third=second + a;
//
//
//    int four= third*4;
//
//    int *five=&four;
//
//    int six=*five;
//
//    int serven;
//    memcpy(&serven, &six, sizeof(input));
//
//    int eight;
//    memmove(&eight, &serven, sizeof(serven));
//
//
    global=input;
//
//    int ten=global;
//
//    int ele;
//    strcpy(&ele, &input);
//
//    int T12;
//    memset(&T12,&input,sizeof(input));
//
//    int array[3];
//    array[1]=input;
//
//    struct A Astruct;
//    Astruct.a=input;
//
//    int T13=Astruct.a;

    int T14=foo();

    int T15=foo2(input);

    //UpdateTaintSetByFun(&T15,4,"foo2");



    printf("%d\n", T15);
    //UpdateTaintFunSet(&input,4,"main");

    return input;

}