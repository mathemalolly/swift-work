#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N (1024 * 1024 * 1)
void leader_kmt(int rank, int size)
{	long int rand_mine, rand_boss, new_number;
	//int rank_next=(rank+1) % size;
	//int rank_prev = rank == 0 ? size -1 : rank-1;
	struct timeval start, end;
	long int new_numbers[size-1];


	srandom(time(NULL) + rank);
	rand_mine = random()/ (RAND_MAX/100);
	printf("%d: my random number is %ld\n", rank, rand_mine);

	if (rank ==0){
	   gettimeofday(&start,NULL);
           printf("%d: sending %ld to all the other %d processors\n", rank, rand_mine, size-1);
	   for (int i=1; i<size;i++){
	       MPI_Send((void *)&rand_mine, 1,MPI_LONG, i, 1, MPI_COMM_WORLD);
	   }
	   printf("%d: My random number is sent to all and sundry\n", rank);
	   for (int i=1; i<size;i++){
		   MPI_Recv((void *)&new_numbers[i-1],1, MPI_LONG, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		   printf("%d:received %ld from Processor %d\n", rank, new_numbers[i-1],i);
	   }
	   int k=0;
	   long sum=0;
	   while(k< size){
		   sum= sum+new_numbers[k];
		   k++;
	   }
	   printf("%d: sum of numbers is %ld\n", rank, sum);
	   gettimeofday(&end,NULL);
           printf("%f\n",(end.tv_sec*1000000.0 + end.tv_usec -
                        start.tv_sec*1000000.0 - start.tv_usec) / 1000000.0);
	}	
       else {
	  MPI_Recv((void *)&rand_boss, 1, MPI_LONG, 0,1,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	  printf("%d: received %ld from leader %d\n", rank,rand_boss ,0); 
	  new_number= rand_mine +rand_boss;
          MPI_Send((void *)&new_number, 1, MPI_LONG,0,1,MPI_COMM_WORLD);
	  printf("%d: sent %ld to the boss %d\n",rank, new_number,0);

       }
	
}
int main(int argc, char *argv[])
{
  int size, rank;
 // struct timeval start, end;
  char hostname[256];
  int hostname_len;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_processor_name(hostname,&hostname_len);

  // Allocate a 1 MiB buffer
 // char *buffer = malloc(sizeof(char) * N);

  // Communicate along the ring
  printf("%d: hello (we have %d number of processors)\n",rank, size);
  leader_kmt(rank, size);
  printf("%d: goodbye\n", rank);

  

  MPI_Finalize();
  return 0;
}
