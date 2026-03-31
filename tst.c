// PHASE 1: C Programming Fundamentals (Refresh)
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


// 1.3 Daynamic memory alloction

// #include <stdio.h>
// #include <stdlib.h>

// typedef struct sPersong
// {
//     char *name;
//     int age;
// }t_Persong;


// int main(void)
// {
//     // the [] , -> and * all of them they dereference


//     // Single Object
//     t_Persong *s_ptr;
    
//     s_ptr = malloc(sizeof(t_Persong));

//     (*s_ptr).age = 10;
//     (*s_ptr).name = "youness";

//     printf("name = %s, age = %d\n", s_ptr->name, s_ptr->age);

//     // Multiple Objects

//     t_Persong *m_ptr;
    
//     s_ptr = malloc(sizeof(t_Persong)*3);

//     (*(s_ptr+ 0)).age = 1;
//     (*(s_ptr+ 0)).name = "one";
    
//     (*(s_ptr+ 1)).age = 2;
//     (*(s_ptr+ 1)).name = "two";
    
//     (*(s_ptr+ 2)).age = 3;
//     (*(s_ptr+ 2)).name = "three";
//     printf("name = %s, age = %d\n", (s_ptr[0]).name, (s_ptr[0]).age);
//     printf("name = %s, age = %d\n", (s_ptr[1]).name, (s_ptr[1]).age);
//     printf("name = %s, age = %d\n", (s_ptr[2]).name, (s_ptr[2]).age);

// }






// PHASE 2: POSIX Threads (pthread) - Core Concepts

// 2.1 What is a Thread ?

