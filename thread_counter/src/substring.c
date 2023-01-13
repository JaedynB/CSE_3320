 /*
Execution Instructions:
gcc -o substringTh1 substringTh1.c -lpthread
./substringTh1 filename.txt
filename being whichever .txt file you want to run. personally tested with hamlet.txt and shakespeare.txt
*/
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define MAX 5000000
#define NUM_THREADS 4
int total = 0;
int n1,n2; 
char *s1,*s2;

FILE *fp;

int thread_count = 0;

pthread_mutex_t mutex;

int readf(char* filename)
{
    
    if((fp=fopen(filename, "r"))==NULL)
    {
        printf("ERROR: can’t open %s!\n", filename);
        return 0;
    }
    
    s1=(char *)malloc(sizeof(char)*MAX);
    
    if (s1==NULL)
    {
        printf ("ERROR: Out of memory!\n") ;
        return -1;
    }
    
    s2=(char *)malloc(sizeof(char) *MAX);
    
    if (s1==NULL)
    {
        printf ("ERROR: Out of memory\n") ;
        return -1;
    }
    
    /*read s1 s2 from the file*/
    
    s1=fgets(s1, MAX, fp);
    s2=fgets(s2, MAX, fp);
    n1=strlen(s1); /*length of s1*/
    n2=strlen(s2)-1; /*length of s2*/
    
    if( s1==NULL || s2==NULL || n1 < n2 ) /*when error exit*/
    {
        return -1;
    }
}
//have one set searching in the first half of the block of strings, have another set searching the last half of the block of strings
//int num_substring ( void )
void *num_substring (void *a)
{
    int i,j,k;
    int count;
    int thread_counter = thread_count++;//each time we go into this function, increment thread_counter for proper division of n1

    if(thread_count != NUM_THREADS - 1)//then we add the length of n2 to the upper bound, to catch cases where a word is split up
    {
        for(i = thread_counter * ((n1 - n2) / NUM_THREADS); i < (( thread_counter + 1 ) * ((n1 - n2) / NUM_THREADS)) + n2; i++)
        {
            count = 0;
            for(j = i ,k = 0; k < n2; j++,k++)// run up to length of string 2
            { /*search for the next string of size of n2*/
                if (*(s1 + j) != *(s2 + k)) //if the ascii of string1 + j isn't equal to ascii of string2 + k, break
                {
                  break;
                }
                else //otherwise, ascii was same
                {
                    count++;
                }
                if (count==n2)//if number of times s1+j matched s2+k up to n2 length, then length of word from s1 matched s2, assume match
                {
                    pthread_mutex_lock( &mutex );
                    total++;
                    pthread_mutex_unlock( &mutex );
                }

            }
        }
    }
    else
    {
        for(i = thread_counter * ((n1 - n2) / NUM_THREADS); i < ( thread_counter + 1 ) * ((n1 - n2) / NUM_THREADS); i++)
        {
            count = 0;
            for(j = i ,k = 0; k < n2; j++,k++)// run up to length of string 2
            { /*search for the next string of size of n2*/
                if (*(s1 + j) != *(s2 + k)) //if the ascii of string1 + j isn't equal to ascii of string2 + k, break
                {
                  break;
                }
                else //otherwise, ascii was same
                {
                    count++;
                }
                if (count == n2)//if number of times s1+j matched s2+k up to n2 length, then length of word from s1 matched s2, assume match
                {
                    pthread_mutex_lock( &mutex );
                    total++;
                    pthread_mutex_unlock( &mutex );
                }
                  
            }
        }
    }
}
    
int main(int argc, char *argv[])
{

    pthread_t tid[NUM_THREADS];//creating 4 to match our virtual CPU's.
    
    int count ;
    pthread_mutex_init( & mutex, NULL);

    if( argc < 2 )
    {
      printf("Error: You must pass in the datafile as a commandline parameter\n");
    }

    printf("Trying to read file: %s\n", argv[1]);
    readf ( argv[1] ) ;
    
    struct timeval start, end;
    float mtime; 
    int secs, usecs;    

    gettimeofday(&start, NULL);
    //creating 4 threads to run num_substring
    for(int i = 0; i < NUM_THREADS; i++){
    if(pthread_create(&tid[i], NULL, num_substring, NULL)){
        printf("\nError creating thread 1\n");
        exit(1);
    }
    }

    for(int i = 0; i < NUM_THREADS; i++){
    if(pthread_join(tid[i],NULL)){
        printf("\n ERROR joining thread");
        exit(1);
    }
    }

    count = total;

    gettimeofday(&end, NULL);

    secs  = end.tv_sec  - start.tv_sec;
    usecs = end.tv_usec - start.tv_usec;
    mtime = ((secs) * 1000 + usecs/1000.0) + 0.5;

    printf ("Number of threads: %d\n", NUM_THREADS);
    printf ("The number of substrings is : %d\n" , count) ;
    printf ("Elapsed time is : %f milliseconds\n", mtime );
    printf ("Number of words in file is : %d\n" , n1);

    if( s1 )
    {
      free( s1 );
    }

    if( s2 )
    {
      free( s2 );
    }

    return 0 ; 
}
