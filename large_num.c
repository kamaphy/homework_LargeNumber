#include "large_num.h"

extern inline int isNum(int c);
extern inline int lNcmp(const struct largeNum *arg1, const struct largeNum *arg2);
extern inline void jumpLine();
extern inline void discardZero(struct largeNum * num);

void initMulTable()
{
    unsigned char i,j;
    for( i = 0; i < 10; ++i)
    {
        for(j = 0; j < 10; ++j)
        {
            g_mulTable[i][j] = i*j;
        }
    }
}

int readInNum(struct largeNum *num)
{
    int len;
    int flag = 1;//flag for the num is negtive(-1)/positive(1)
    int charactor;
    int maybeZero = 0;

    charactor = getchar();
    if(charactor == -1)
    {
        return 0;//输入结束
    }
    else
    {
        while(charactor == ' ' || charactor == '\n')
        {
            charactor = getchar();//去除多余空格和回车
        }
        if(charactor == '-')
        {
            flag = -1;//negative
            charactor = getchar();
        }
        else if(charactor == '+')
        {
            flag = 1;//positive
            charactor = getchar();
        }
        while(charactor == '0')
        {
            maybeZero = 1;
            charactor = getchar();//去除多余0
        }
        num->num = (unsigned char*)malloc((LEN_MAX+1)*sizeof(char));
        if(num->num == NULL)
        {
            printf("Memory run out.");
            return 0;
        }
        memset(num->num, 0, LEN_MAX+1);
        for(len = 0; len < LEN_MAX; ++len)
        {
            if(!isNum(charactor))
            {
                if(charactor == '\n')
                {
                    break;//行尾
                }
                else
                {
                    free(num->num);
                    jumpLine();
                    return ERROR_OCCUR;//非法字符
                }
            }
            else
            {
                num->num[len] = charactor-'0';

            }
            charactor = getchar();
        }
        if(len == 0 && maybeZero == 0)
        {
            free(num->num);
            return ERROR_OCCUR;
        }
        else if(len == LEN_MAX)
        {
            free(num->num);
            jumpLine();
            return ERROR_OCCUR;
        }
        else if(maybeZero == 1 && len == 0)
        {
            num->len = 1;
            return num->len;
        }
        else
        {
            num->len = flag*(len);
            return len;
        }
    }
}

inline int printNum(const struct largeNum *num)
{
    int i = 0;
    int len = num->len;
    if(len < 0)
    {
        putchar('-');
        len = -len;
    }

    for(; len > 0; ++i,--len)
    {
        putchar('0' + num->num[i]);
    }
    return 0;
}

int putsNum(const struct largeNum *num)
{
    if(num == NULL)
    {
        printf("error input!\n");
        return -1;
    }
    printNum(num);
    putchar('\n');
    return 0;
}

int print2Num(const struct largeNum *num1, const struct largeNum *num2)
{
    if(num1 == NULL || num2 == NULL)
    {
        printf("error input!\n");
        return -1;
    }
    printNum(num1);
    putchar(' ');
    printNum(num2);
    putchar('\n');
    return 0;
}

struct largeNum * add(struct largeNum *arg1, struct largeNum *arg2)
{
    int flag1 = arg1->len>0?1:-1;
    int flag2 = arg2->len>0?1:-1;
    int len1  = flag1 * arg1->len;
    int len2  = flag2 * arg2->len;
    int len_ans = len1 > len2 ? len1 : len2;
    struct largeNum *ans = NULL;
    unsigned char carrier;
    unsigned char current;
    if(flag1 != flag2)
    {
        arg2->len = -arg2->len;
        return sub(arg1, arg2);
    }
    else
    {
        ans = (struct largeNum*)malloc(sizeof(struct largeNum));
        if(ans == NULL)
        {
            return NULL;
        }
        ans->num = (unsigned char *)malloc(sizeof(char)*(len_ans + 2));
        if(ans->num == NULL)
        {
            free(ans);
            return NULL;
        }

        memset(ans->num, 0, len_ans + 2);
        ans->len = len_ans;
        --len1, --len2;
        carrier = 0;
        while(len1 >= 0 || len2 >= 0)
        {
            current = carrier;
            if(len1 >= 0)
            {
                current += arg1->num[len1];
            }
            if(len2 >= 0)
            {
                current += arg2->num[len2];
            }

            if(current < 10)
            {
                ans->num[len_ans] = current;
                carrier = 0;
            }
            else
            {
                carrier = 1;
                ans->num[len_ans] = current - 10;
            }
            --len1, --len2, --len_ans;
        }
        if(carrier == 1)
        {
            ans->num[len_ans] = 1;
            ++(ans->len);
        }
        else
        {
            discardZero(ans);
        }
        ans->len = flag1 * ans->len;

        return ans;
    }
}

struct largeNum * sub(struct largeNum *arg1, struct largeNum *arg2)
{
    int flag1 = arg1->len>0?1:-1;
    int flag2 = arg2->len>0?1:-1;
    int len1  = flag1 * arg1->len;
    int len2  = flag2 * arg2->len;
    int len_ans = len1 > len2 ? len1 : len2;
    int cont_zeros = 0;
    int cmp_result = lNcmp(arg1, arg2);
    struct largeNum *ans = NULL;
    unsigned char carrier;
    unsigned char current;
    unsigned char undersub;

    if(flag1 != flag2)
    {
        arg2->len = -arg2->len;
        return add(arg1, arg2);
    }
    if(cmp_result == 0)
    {
        ans = (struct largeNum *) malloc(sizeof(struct largeNum));
        ans->len = 1;
        ans->num = (unsigned char *)malloc(sizeof(char)*2);
        ans->num[0] = 0;
        ans->num[1] = 0;
        return ans;
    }
    if((flag1 > 0) ^ (cmp_result > 0))
    {
        arg1->len = -arg1->len;
        arg2->len = -arg2->len;
        return sub(arg2, arg1);//保证绝对值大的减绝对值小的
    }
    else
    {
        ans = (struct largeNum *) malloc(sizeof(struct largeNum));
        if(ans == NULL)
        {
            return NULL;
        }
        ans->num = (unsigned char *)malloc(sizeof(char) * (len_ans + 1));
        if(ans->num == NULL)
        {
            free(ans);
            return NULL;
        }

        memset(ans->num, 0, len_ans + 1);
        ans->len = len_ans;
        --len1,--len2,--len_ans;
        carrier = 0;
        while(len1 >= 0)
        {
            current = arg1->num[len1];
            undersub = len2 >= 0? arg2->num[len2]:0;
            if(current < undersub + carrier)
            {
                current = 10 + current - carrier - undersub;
                carrier = 1;
            }
            else
            {
                current = current - carrier - undersub;
                carrier = 0;
            }
            ans->num[len_ans] = current;
            if(current == 0)
            {
                ++cont_zeros;
            }
            else
            {
                cont_zeros = 0;
            }

            --len1, --len2, --len_ans;
        }
        ans->len -= cont_zeros;
        discardZero(ans);
        ans->len = flag1 * ans->len;
        return ans;
    }
}

struct largeNum * mul(struct largeNum *arg1, struct largeNum *arg2)
{
    int flag2 = arg2->len>0?1:-1;
    int len2  = flag2 * arg2->len;
    struct largeNum *ans, *a, *b;
    int e = 0;

    --len2;
    a = mul_s(arg1, arg2->num[len2], e);

    ans = a;
    ++e, --len2;

    while(len2 >= 0)
    {
        b = mul_s(arg1, arg2->num[len2], e);
        ans = add(a, b);

        free(a->num);
        free(a);
        free(b->num);
        free(b);

        a = ans;
        ++e, --len2;
    }
    ans->len = ans->len * flag2;
    return ans;
}

struct largeNum * mul_s(struct largeNum *arg, int k, int e)
{
    struct largeNum *ans;
    int flag = arg->len > 0? 1: -1;
    int len = flag * arg->len;
    int len_ans = len + 1;
    unsigned char current;
    unsigned char carrier;

    ans = (struct largeNum *)malloc(sizeof(struct largeNum));
    if(ans == NULL)
    {
        return NULL;
    }

    if(k == 0 || (len == 1 && arg->num[0] == 0))
    {
        ans->num = (unsigned char *)malloc(sizeof(char)*2);
        if(ans->num == NULL)
        {
            free(ans);
            return NULL;
        }
        ans->len = 1;
        ans->num[0] = ans->num[1] = 0;
        return ans;
    }

    ans->num = (unsigned char *)malloc(sizeof(char)*(len + 2 + e));
    if(ans->num == NULL)
    {
        free(ans);
        return NULL;
    }
    memset(ans->num, 0, (len+2+e)*sizeof(char));

    --len, --len_ans;
    ans->len = len_ans + e;
    carrier = 0;
    while(len >= 0)
    {
        current = g_mulTable[k][arg->num[len]] + carrier;
        carrier = current / 10;
        ans->num[len_ans] = current % 10;
        --len, --len_ans;
    }
    if(carrier != 0)
    {
        ans->num[len_ans] = carrier;
        ++ans->len;
    }
    else
    {
        discardZero(ans);
    }
    ans->len = ans->len * flag;
    return ans;
}

struct largeNum * divL(struct largeNum *arg1, struct largeNum *arg2, struct largeNum **_remain)
{
    if(arg1->len < 0 || arg2->len < 0 || (arg2->len == 1 && arg2->num[0] == 0))
    {
        return NULL;
    }
    int len1  = arg1->len;
    int len2  = arg2->len;
    int i;//loop variant
    struct largeNum *ans;
    struct largeNum *remain;
    struct largeNum *(table_div[10]);

    for(i = 0; i < 10; ++i)
    {
        table_div[i] = mul_s(arg2, i, 0);
        if(table_div[i] == NULL)
        {
            while(i-->0)
            {
                free(table_div[i]->num);
                free(table_div[i]);
            }
            return NULL;
        }
    }
    ans = (struct largeNum *)malloc(sizeof(struct largeNum));
    if(ans == NULL)
    {
        for(i = 0; i < 10; ++i)
        {
            free(table_div[i]->num);
            free(table_div[i]);
        }
        return NULL;
    }
    struct largeNum * dividend = lNumcopy(arg1);
    struct largeNum * divisor_e;
    if(lNcmp(dividend, arg2) < 0)
    {
        remain = lNumcopy(dividend);
        ans->num = (unsigned char *)malloc(2*sizeof(char));
        if(ans->num == NULL)
        {
            for(i = 0; i < 10; ++i)
            {
                free(table_div[i]->num);
                free(table_div[i]);
            }
            free(dividend->num);
            free(remain->num);
            free(dividend);
            free(remain);
            free(ans);
            return NULL;
        }
        ans->len = 1;
        ans->num[0] = ans->num[1] = 0;
        *_remain = remain;
        return ans;
    }

    ans->len = 0;
    int e = len1 - len2;
    ans->num = (unsigned char *)malloc((e + 1));
    if(ans->num == NULL)
    {
        for(i = 0; i < 10; ++i)
        {
            free(table_div[i]->num);
            free(table_div[i]);
        }
        free(dividend->num);
        free(dividend);
        free(ans);
        return NULL;
    }
    memset(ans->num, 0, e + 1);

    for(; e >= 0; --e)
    {
        dividend->len -= e;
        for(i = 0; lNcmp(dividend, table_div[i]) >= 0; ++i);
        if(i == 1)
        {
            dividend->len += e;
            if(e != len1 - len2)
            {
                ans->num[ans->len] = 0;
                ans->len++;
            }
            continue;
        }
        else
        {
            --i;
            dividend->len += e;
            divisor_e = LNexp(table_div[i], e);
            remain = sub(dividend, divisor_e);

            free(dividend->num);
            free(divisor_e->num);
            free(dividend);
            free(divisor_e);

            dividend = remain;
            *_remain = remain;
            ans->num[ans->len] = i;
            ans->len++;
        }
    }
    for(i = 0; i < 10; ++i)
    {
        free(table_div[i]->num);
        free(table_div[i]);
    }
    return ans;

}

struct largeNum * LNexp(struct largeNum * num, int e)
{
    int flag = num->len > 0? 1:-1;
    int len = num->len * flag;
    if(num == 0 || (num->len == 1 && num->num[0] == 0))
    {
        return NULL;
    }

    struct largeNum *ans = (struct largeNum *) malloc(sizeof(struct largeNum));
    if(ans == NULL)
    {
        return NULL;
    }
    ans->num = (unsigned char *)malloc((len + e + 1)* sizeof(char));
    if(ans->num == NULL)
    {
        free(ans);
        return NULL;
    }

    memset(ans->num, 0, (unsigned)(len+e+1));
    memcpy(ans->num, num->num, (unsigned)len);
    ans->len = len + e;
    ans->len = ans->len * flag;
    return ans;
}

struct largeNum * lNumcopy(const struct largeNum * source)
{
    int flag = source->len > 0 ? 1:-1;
    int len = source->len * flag;
    struct largeNum *dst = (struct largeNum *)malloc(sizeof(struct largeNum));
    if(dst == NULL)
    {
        return NULL;
    }
    dst->num = (unsigned char *)malloc(sizeof(char) * (unsigned)(len + 1));
    if(dst->num == NULL)
    {
        free(dst);
        return NULL;
    }
    memset(dst->num, 0, (unsigned)(len+1));
    memcpy(dst->num, source->num, (unsigned) len);
    dst->len = source->len;
    return dst;
}
