#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define is_digit(ch) ('0' <= (ch) && (ch) <= '9')
#define MIN(X, Y) (((X) <= (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))


extern void addition(char* x, char* y, char* result, long x_size, long y_size);
extern void subtraction(char* x, char* y, char* result, long x_size, long y_size);
extern void multiply(char* x, char* y, char* result, long x_size, long y_size);


// A structure to represent a bignum
typedef struct bignum { 
    long number_of_digits;
    long size;
    int sign;
    char *digit;
} bignum;

void init(bignum* num, long initial_size){
    num->digit=(char *)malloc(initial_size * sizeof(char));
    num->number_of_digits = 0;
    num->sign = 0;
    num->size=initial_size;
}

void copy(bignum* num, char* digits, int input_sign){
    num->digit = digits;
    num->sign = input_sign;
    long i=0;
    while(digits[i]!=0) //counting the digits
        ++i;
    num->number_of_digits = i;
    num->size = i;
}

void insert_digit(bignum* num, char element) { 
  if (num->number_of_digits == num->size) {
    num->size *= 2;
    num->digit = (char *)realloc(num->digit, num->size * sizeof(char));
  }
  num->digit[num->number_of_digits++] = element;
}

void free_num(bignum *num) {
  free(num->digit);
  num->digit = NULL;
  num->number_of_digits = num->size = 0;
  free(num);
}

long compare(bignum *num1, bignum *num2){
    long i = 0, j = 0, len1, len2;
    while(num1->digit[i]=='0')
        ++i;
    while(num2->digit[j]=='0')
        ++j;
    len1 = num1->number_of_digits-i;
    len2 = num2->number_of_digits-j;
    if(len1 != len2)
        return len2-len1;
    while(num1->digit[i]&&num2->digit[j]){
        if(num1->digit[i]<num2->digit[j])
            return 1;
        else if(num1->digit[i]>num2->digit[j])
            return -1;
        ++i;
        ++j;
    }
    return 0;
}

//------------------------------------------

// A structure to represent a stack
struct StackNode{
    bignum* data;
    struct StackNode* next;
};

void push(struct StackNode** root, bignum* data){
    struct StackNode* stackNode =
              (struct StackNode*) malloc(sizeof(struct StackNode));
    stackNode->data = data;
    stackNode->next = *root;
    *root = stackNode;
}
 
bignum* pop(struct StackNode** root){
    struct StackNode* temp = *root;
    *root = (*root)->next;
    bignum* popped = temp->data;
    free(temp);
    temp = NULL;
    return popped;
}
 
bignum* peek(struct StackNode* root){
    return root->data;
}

int isEmpty(struct StackNode* root){
    return !root;
}

//------------------------------------------

long resultSize(char* result){ //returns size of result without leading zeroes
    long i=0,size=0;
    while(result[i]=='0')
        ++i;
    while(result[i]!=0){
        ++size;
        ++i;
    }
    return size;
}
    
long compareC(char* first, char* second){ 
    if(resultSize(second)!=resultSize(first))
        return (resultSize(second)-resultSize(first));
    long zeroes=0;
    long zeroes2=0;
    while(first[zeroes]=='0')
        ++zeroes;
    while(second[zeroes2]=='0')
        ++zeroes2;
    for(long i=zeroes, j=zeroes2; i<resultSize(first)+zeroes; ++i, ++j)
        if(first[i]!=second[j])
            return (second[j]-first[i]);
    return 0;
}

//---------------------------------------------------------

int main(int argc, char **argv) {

  struct StackNode* calcStack = NULL; //global stack for calculations
  char ch;
  int wasNum = 0;
  bignum* number ;
  
  while(((ch = fgetc(stdin)) != EOF)&&(ch!='q')){ 
    if((is_digit(ch))||(ch=='_')){
        if(wasNum==0){
            number = (bignum*) malloc(sizeof(bignum));
            init(number,1024);//for store the input 1024 digits in begining
            wasNum=1;
            if(ch!='_')
                insert_digit(number,ch);
            else
                number->sign = 1;
        }else if(wasNum==1)
            insert_digit(number,ch);
    }else if(ch=='c'){
        wasNum = 0;
        while(!isEmpty(calcStack)){
            bignum* popped = pop(&calcStack);
            free_num(popped);
            popped = NULL;
        }
    }else if(ch=='p'){
        wasNum = 0;
        if(!isEmpty(calcStack)){
            char* toPrint = peek(calcStack)->digit;    
            while(*toPrint=='0')
                ++toPrint;
            if(*toPrint==0) 
                printf("0\n");
            else {
                if(peek(calcStack)->sign)
                    printf("-");
                printf("%s\n", toPrint);
            }
        }else
            printf("Empty Stack\n");
    }else if(ch<=32){
        if(wasNum){
            insert_digit(number,0);
            number->number_of_digits--;
            push(&calcStack, number);//push bignum to stack
            wasNum = 0;
        }
    }else{//one of four operators
        wasNum = 0;
        int swapped = 0;
        bignum* x = pop(&calcStack);
        bignum* y = pop(&calcStack);
        if(compare(x,y)>=0){
            bignum *tmp=x;
            x=y;
            y=tmp;
            swapped = 1;
        }
        char* first = x->digit;
        char* second = y-> digit;
        long firstZeroes=0,secondZeroes=0;
        while(first[firstZeroes]=='0')
            ++firstZeroes;
        if(first[firstZeroes]==0)
            --firstZeroes;
        while(second[secondZeroes]=='0')
            ++secondZeroes;
        if(second[secondZeroes]==0)
            --secondZeroes;
        first = first+firstZeroes;
        second = second+secondZeroes;
        long first_size=x->number_of_digits-firstZeroes;
        long second_size=y->number_of_digits-secondZeroes;
        char * result;
        int output_sign = 0;
        switch(ch){
            case('+'):
                result = (char *) malloc( (2 + first_size) * sizeof(char));
                if(x->sign == 1)
                    output_sign = 1;
                if(x->sign==y->sign)
                    addition(first, second, result, first_size, second_size);
                else
                    subtraction(first, second, result, first_size, second_size);
                break;
            case('-'):
                result = (char *)malloc( (2 + first_size) * sizeof(char));
                if(swapped == x->sign)
                    output_sign = 1;
                if((x->sign)!=(y->sign))
                    addition(first, second, result, first_size, second_size);
                else 
                    subtraction(first, second, result, first_size, second_size);
                break;
            case('*'):
                result = (char *) calloc( (1 + first_size + second_size) , sizeof(char));
                if(x->sign != y->sign)
                    output_sign = 1;
                multiply(first, second, result, first_size, second_size);
                break;
            case('/'):
                result = (char *) calloc( (1 + first_size + second_size) , sizeof(char));
                memset(result,48,(1 + first_size + second_size) * sizeof(char));
                result[first_size + second_size]=0;
                if(x->sign != y->sign)
                    output_sign = 1;
                if((!swapped)||(!strcmp(second, "0")))
                    result[first_size + second_size-1]='0';
                else if(compareC(first, second)==0)
                    result[first_size + second_size-1]='1';
                else{
                    struct StackNode* divStack = NULL; //stack for division
                    long counter = 0;
                    char* resTwo = (char*) calloc((1 + first_size + second_size) , sizeof(char));
                    bignum* two = (bignum*) malloc(sizeof(bignum));
                    init(two,2);
                    insert_digit(two,'2');
                    insert_digit(two,0);
                    memset(result,0,(1 + first_size + second_size) * sizeof(char));
                    multiply(second, two->digit, result, second_size, 1);
                    char* sech = (char*) calloc((second_size+1) , sizeof(char));
                    memcpy(sech,second,second_size+1);
                    bignum* sec = (bignum*) malloc(sizeof(bignum));
                    copy(sec,sech,0);  
                    push(&divStack, sec);//push result to stack
                    while(compareC( first, result)<=0){ //while result less than first
                        char* tmp=result;
                        while(tmp[0]=='0'&&tmp[1]!=0)
                            ++tmp;
                        char* numb = (char*) calloc((resultSize(result)+1) , sizeof(char));
                        memcpy(numb,tmp,resultSize(result));
                        bignum* output2 = (bignum*) malloc(sizeof(bignum));
                        copy(output2,numb,0);  
                        push(&divStack, output2);//push result to stack
                        memset(result,0,(1 + first_size + second_size) * sizeof(char));
                        multiply(numb, two->digit, result, resultSize(numb) , 1);       
                        ++counter;
                    }
                    if(counter==0){
                        memset(result,48,(1 + first_size + second_size) * sizeof(char));
                        result[first_size + second_size-1]='1';
                        result[first_size + second_size]=0;
                        while(!isEmpty(divStack)){
                            bignum* subPopped = pop(&divStack);
                            free_num(subPopped);
                            subPopped = NULL;
                        }
                    }else{ 
                        struct StackNode* powerStack = NULL; //stack for saving power of 2
                        memset(result,0,(1 + first_size + second_size) * sizeof(char));
                        memset(resTwo,48,( first_size + second_size) * sizeof(char));
                        resTwo[first_size + second_size-1]='1';
                        bignum* one = (bignum*) malloc(sizeof(bignum));
                        init(one,2);
                        insert_digit(one,'1');
                        insert_digit(one,0);
                        push(&powerStack, one);//push result to stack
                        for(long i=0;i<counter;++i){//adding powers of 2 to stack
                            char* tmp=resTwo;
                            while(tmp[0]=='0'&&tmp[1]!=0)
                                ++tmp;
                            memset(result,0,(1 + first_size + second_size) * sizeof(char));
                            multiply(tmp, two->digit, result, resultSize(resTwo) , 1);
                            memcpy(resTwo, result,(1 + first_size + second_size));
                            char* numb = (char*) calloc((resultSize(resTwo)+1) , sizeof(char));
                            char* tmp2=result;
                            while(tmp2[0]=='0'&&tmp2[1]!=0)
                                ++tmp2;
                            memcpy(numb,tmp2,resultSize(resTwo));
                            bignum* output2 = (bignum*) malloc(sizeof(bignum));
                            copy(output2,numb,0);  
                            push(&powerStack, output2);//push result to stack
                        }
                        bignum* toSub = pop(&divStack);
                        char* tmp=toSub->digit;
                        while(tmp[0]=='0'&&tmp[1]!=0)
                            ++tmp;
                        char* tmp2=first;
                        while(tmp2[0]=='0'&&tmp2[1]!=0)
                            ++tmp2;
                        subtraction(tmp2, tmp, result, resultSize(tmp2), resultSize(tmp));
                        free_num(toSub);
                        memcpy(resTwo, result,(1 + first_size + second_size));
                        struct StackNode* power2Stack = NULL; //stack for power
                        bignum* p = pop(&powerStack);
                        push(&power2Stack, p);//push result to stack
                        while(!isEmpty(divStack)){
                            bignum* subPopped = pop(&divStack);
                            p = pop(&powerStack);
                            tmp=subPopped->digit;
                            while(tmp[0]=='0'&&tmp[1]!=0)
                                ++tmp;
                            char* tmp3=resTwo;
                            while(tmp3[0]=='0'&&tmp3[1]!=0)
                                ++tmp3;
                            if(compareC(resTwo,tmp)<=0){
                                subtraction(tmp3, tmp, result, resultSize(resTwo), resultSize(tmp));
                                memcpy(resTwo, result,(1 + first_size + second_size));
                                push(&power2Stack, p);//push result to stack
                            }else{
                                free_num(p);
                            }
                            free_num(subPopped);
                            subPopped = NULL;
                        }
                        memset(result,0,(1 + first_size + second_size) * sizeof(char));
                        memcpy(resTwo, result,(1 + first_size + second_size));
                        while(!isEmpty(power2Stack)){
                            bignum* subPopped = pop(&power2Stack);
                            char* tmp4=resTwo;
                            while(tmp4[0]=='0'&&tmp4[1]!=0)
                                ++tmp4;
                            char* tmp5=subPopped->digit;
                            while(tmp5[0]=='0'&&tmp5[1]!=0)
                                ++tmp5;
                            addition(tmp5, tmp4, result, resultSize(tmp5), resultSize(tmp4));
                            memcpy(resTwo, result,(1 + first_size + second_size));
                            free_num(subPopped);
                            subPopped = NULL;
                        }
                        long i=0;
                        while(is_digit(result[i]))
                            ++i;
                        result[i]=0;
                    }  
                    free(resTwo);
                    free_num(two);
                }
                break;
        }
        free_num(x);//free the first big num and its array
        free_num(y);//free the second big num and its array
        bignum* output = (bignum*) malloc(sizeof(bignum));
        copy(output,result,output_sign);  
        push(&calcStack, output);//push result to stack
    }
  }//end while
  while(!isEmpty(calcStack)){
    bignum* popped = pop(&calcStack);
    free_num(popped);
    popped = NULL;
  }
  return 0;
}



