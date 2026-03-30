// 1.1 Struct & typedef
// #include <stdio.h>


// typedef struct sPersong
// {
//     char *name;
//     int age;
// }t_Persong;


// int main(void)
// {
//     t_Persong p1;

//     p1.age = 10;
//     p1.name = "youness";

//     printf("age = %d , name = %s\n", p1.age, p1.name);


//     return (0);
// }


// 1.2 Pointer to Stuctur.
// #include <stdio.h>

// typedef struct sPersong
// {
//     char *name;
//     int age;
// }t_Persong;


// int main(void)
// {
//     t_Persong p1;


//     p1.age = 10;
//     p1.name = "youness";

//     t_Persong *ptr;
//     t_Persong **ptr2;
    
//     ptr = &p1;

//     ptr2 = &ptr;

//     printf("age = %d , name = %s\n", p1.age, p1.name);
//     printf("age = %d , name = %s\n", (*ptr).age, (*ptr).name);
//     printf("age = %d , name = %s\n", (*(*ptr2)).age, (*(*ptr2)).name);    ?   ?   ?   ?   ?


//     return (0);
// }


