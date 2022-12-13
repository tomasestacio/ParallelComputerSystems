#include <stdio.h>
#include <time.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>

#define INTERVALS 10000000

double a[INTERVALS], b[INTERVALS];

int main(int argc, char **argv)
{
  double time2;
  time_t time1 = clock();
  double *to = a;
  double *from = b;
  int    time_steps = 100;
  int procid, procnum, numbers_per_rank, my_first, my_last;

  /* Set up initial and boundary conditions. */
  from[0] = 1.0;
  from[INTERVALS - 1] = 0.0;
  to[0] = from[0];
  to[INTERVALS - 1] = from[INTERVALS - 1];

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &procid);
  MPI_Comm_size(MPI_COMM_WORLD, &procnum);
  
  //numbers_per_rank = floor(INTERVALS/procnum);

  //if(INTERVALS % procnum > 0) numbers_per_rank++;
  
  numbers_per_rank = floor((INTERVALS - 2)/procnum);

  if((INTERVALS - 2) % procnum > 0) numbers_per_rank++;
  
  if(procid == 0){
    my_first = 1;
    my_last = my_first + numbers_per_rank;
  }

  else if(procid == procnum - 1){
    my_first = procid * numbers_per_rank + 1;
    my_last = INTERVALS - 1; 
  }

  else{
    my_first = procid * numbers_per_rank + 1;
    my_last = my_first + numbers_per_rank;
  }
  
  //my_first = procid * numbers_per_rank;
  //my_last = my_first + numbers_per_rank;

  for(int i = my_first; i < my_last; i++){
    from[i] = 0.0;
    printf("PROCID: %d - from[%d] = %f - (my_last-my_first) = %d\n", procid, i, from[i], my_last-my_first);
  }
  
  // MPI_Allgather(&from[my_first], (my_last-my_first), MPI_DOUBLE, &(from[1]), (my_last-my_first), MPI_DOUBLE, MPI_COMM_WORLD);
  
  if(procid == 0) 
    printf("Number of intervals: %ld. Number of time steps: %d\n", INTERVALS, time_steps);

  /* Apply stencil iteratively. */
  while(time_steps-- > 0)
  { 
    
    for(int i = my_first; i < my_last ; i++)
      to[i] = from[i + 1] + 0.1*(from[i] - 2*from[i + 1] + from[i + 2]);

    // MPI_Allgather(&to[my_first], (my_last-my_first), MPI_DOUBLE, &(to[1]), (my_last-my_first), MPI_DOUBLE, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    if(procid == 0){
      double* tmp = from;
      from = to;
      to = tmp;
    }

    MPI_Bcast(from, INTERVALS, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(to, INTERVALS, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  }

  if(procid == 0){
    time2 = (clock() - time1) / (double) CLOCKS_PER_SEC;
    printf("Elapsed time (s) = %f\n", time2);

    for(long i = 2; i < 30; i += 2)
      printf("Interval %ld: %f\n", i, to[i]);
  }
  
  MPI_Finalize();

  return 0;

}                   
