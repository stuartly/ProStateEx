//
// Created by stly on 1/16/18.
//


#include <stdio.h>
#include <string.h>

int foo(int a){

    int b=a+3;

    return b;
}

int main()
{
    int input;
    scanf("%d",&input);

    int a=3;

    int second=input + 2;

    int &T3=input;

    int third=second + a;

    int four= third*4;

    int five=foo(four);

    if(second > 5){
        a=6;
    }

    int T6=a;

    int T7= a || second || four || five;

    int T8 = a && second;

    printf("%d\n", four);

    return 0;

}

