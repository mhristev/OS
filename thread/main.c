#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

char *read_input() {
  int buff_size = 1024;
  int i = 0;
  char *buf = malloc(sizeof(char) * buff_size);
  char c;

  if(!buf) {
    perror("malloc");
  }

  while (1) 
  {
    c = getchar(); 
    if (c == 'c') {
      exit(1); 
    }
    if (c == '\n') {
        return buf;
    } else {
      buf[i] = c;
    }
    i++;

    if (i == buff_size) {
      buff_size *= 2;
      buf = realloc(buf, buff_size);

      if(!buf) {
        perror("realloc");
      }
    }
  }
}



int check(int num) {
    int i = 0;
    int flag = 0;

    for (i = 2; i <= num / 2; ++i) {
        if (num % i == 0) {
            flag = 1;
            break;
        }
    }
    
    if (flag == 0)
        return 1;
    else
        return 0;
}




void* isprime(void* N) {
    int n = *((int*)N);
    printf("Prime calculation started for N=%d\n", n);
    int k = 0;
    for (int i = 2; i < n; i++) {
        if(check(i))
            k++;
    }
    printf("Number of primes for N=%d is %d\n", n, k);
    return NULL;
}

int main() {
    char* input;
    int* N = malloc(sizeof(*N));
    while(1) {
        input = read_input();
        if (input[0] == 'p') {
            int count = 0;
            for (int i = 2; input[i] != '\0'; i++) {
                count++;
            }
            char caster[count];
            for (int i = 2; input[i] != '\0'; i++) {
                caster[i-2] = input[i];
            }
            *N = atoi(caster);
            //printf("caster = %d\n", N);
            if (N == 0) {
                continue;
            }
        } else {
            continue;
        }

        pthread_t thread;
        void *status;
        pthread_create(&thread, NULL, isprime, (void*)N);
        pthread_join(thread, status);
        return 0;
    }
}