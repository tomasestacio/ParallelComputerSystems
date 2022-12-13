#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <smmintrin.h>
#include <emmintrin.h> /* where intrinsics are defined */

#define CLOCK_RATE_GHZ 2.3e9

/* Time stamp counter from Lecture 2/17 */
static __inline__ unsigned long long RDTSC(void) {
    unsigned hi,lo;
    __asm__ volatile("rdtsc" : "=a"(lo),"=d"(hi));
    return ((unsigned long long) lo)| (((unsigned long long)hi) << 32);
}

int sum_naive( int n, int *a )
{
    int sum = 0;
    for( int i = 0; i < n; i++ )
        sum += a[i];
    return sum;
}

int sum_unrolled( int n, int *a )
{
    int sum = 0;

    /* do the body of the work in a faster unrolled loop */
    for( int i = 0; i < n/4*4; i += 4 )
    {
        sum += a[i+0];
        sum += a[i+1];
        sum += a[i+2];
        sum += a[i+3];
    }

    /* handle the small tail in a usual way */
    for( int i = n/4*4; i < n; i++ )   
        sum += a[i];
    
    return sum;
}

int sum_vectorized( int n, int *a )
{
    /* WRITE YOUR VECTORIZED CODE HERE */
    __m128i sum = _mm_setzero_si128();
    int i = 0;
    int total;

    for(i = 0; i < n/4*4 ; i+=4)
    {
      __m128i new_a = _mm_loadu_si128((__m128i*) &a[i]);
      sum = _mm_add_epi32(sum, new_a);
    }

    //at this point, sum has 4 ints that need to be summed to complete
    //I need to access each of the 32 bits inside the chunck of 128 bits
    //and sum it
    for(i = 0; i < 4; i++)
      total += _mm_extract_epi32(sum, i);
    
    //handle small tail, no need to vectorize this part
    for(i = n/4*4; i < n; i++)
      total += a[i];
    
    return total;
}

int sum_vectorized_unrolled( int n, int *a )
{
    /* UNROLL YOUR VECTORIZED CODE HERE*/
    __m128i sum = _mm_setzero_si128();
    __m128i aux;
    int i = 0;
    int total;
    
    for(i = 0; i < n/4*4; i+=16)
    {
      aux = _mm_loadu_si128((__m128i*) &a[i+0]);
      sum = _mm_add_epi32(sum, aux);

      aux = _mm_loadu_si128((__m128i*) &a[i+4]);
      sum = _mm_add_epi32(sum, aux);

      aux = _mm_loadu_si128((__m128i*) &a[i+8]);
      sum = _mm_add_epi32(sum, aux);

      aux = _mm_loadu_si128((__m128i*) &a[i+12]);
      sum = _mm_add_epi32(sum, aux);
    } 
    
    for(i = 0; i < 4; i++)
      total += _mm_extract_epi32(sum, i);

    for(i = n/4*4; i < n; i++)   
      total += a[i];
    
    return total;
}

void benchmark( int n, int *a, int (*computeSum)(int,int*), char *name )
{
    /* warm up */
    int sum = computeSum( n, a );

    /* measure */
    unsigned long long cycles = RDTSC();
    sum += computeSum( n, a );
    cycles = RDTSC()-cycles;
    
    double microseconds = cycles/CLOCK_RATE_GHZ*1e6;
    
    /* report */
    printf( "%20s: ", name );
    if( sum == 2*sum_naive(n,a) ) printf( "%.2f microseconds\n", microseconds );
    else	                  printf( "ERROR!\n" );
}

int main( int argc, char **argv )
{
    const int n = 7777; /* small enough to fit in cache */
    
    /* init the array */
    srand48( time( NULL ) );
    int a[n] __attribute__ ((aligned (16))); /* align the array in memory by 16 bytes */
    for( int i = 0; i < n; i++ ) a[i] = lrand48( );
    
    /* benchmark series of codes */
    benchmark( n, a, sum_naive, "naive" );
    benchmark( n, a, sum_unrolled, "unrolled" );
    benchmark( n, a, sum_vectorized, "vectorized" );
    benchmark( n, a, sum_vectorized_unrolled, "vectorized unrolled" );

    return 0;
}

