#include <stdio.h> 
#include <unistd.h> 
#include <omp.h>

__attribute__((target(mic))) void prim(){
	int primes = 0;
	int limit = 2000000;
	double start = omp_get_wtime();
#pragma omp parallel for schedule(dynamic) reduction(+: primes)
	for (int num = 1; num <= limit; num++) { 
		int i = 2; 
		while(i <= num) { 
		    if(num % i == 0)
			break;
		    i++; 
		}
		if(i == num)
		    primes++;
		fflush(0);
	}
	printf("[coprocessor] Calculated %d primes in %f s.\n", primes, omp_get_wtime()-start);

}

void prim_host(){
	int primes = 0;
	int limit = 2000000;
	double start = omp_get_wtime();
#pragma omp parallel for schedule(dynamic) reduction(+: primes)
	for (int num = 1; num <= limit; num++) { 
		int i = 2; 
		while(i <= num) { 
		    if(num % i == 0)
			break;
		    i++; 
		}
		if(i == num)
		    primes++;
		fflush(0);
	}
	printf("[host]        Calculated %d primes in %f s.\n", primes, omp_get_wtime()-start);

}

int main(){
	printf("Hello world! I'm the hosts. I have %ld logical cores.\n", sysconf(_SC_NPROCESSORS_ONLN ));
	const int nDevices = _Offload_number_of_devices();
	char sig[nDevices];
	for (int i=0; i<nDevices; i++){
#pragma offload target(mic: i) signal(&sig[i])
		{
		printf("Hello world! I'm a coprocessor. I have %ld logical cores.\n", sysconf(_SC_NPROCESSORS_ONLN ));
		prim();
		}
	}
	for (int i=0; i<nDevices; i++){
#pragma offload_wait target(mic: i) wait(&sig[i])
	}
	prim_host();
}
