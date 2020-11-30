#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

unsigned long long timeval(const struct timeval time) {
   return time.tv_sec * 1000000llu + time.tv_usec;
}

unsigned long long getnow() {
   struct timeval timeNow;
   gettimeofday(&timeNow, NULL);
   return timeval(timeNow);
}

void linebuf_on() {
   struct termios termiosStruct;
   tcgetattr(STDIN_FILENO, &termiosStruct);
   termiosStruct.c_lflag |= (ICANON | ECHO);
   tcsetattr(STDIN_FILENO, TCSANOW, &termiosStruct);
}

void linebuf_off() {
   struct termios termiosStruct;
   tcgetattr(STDIN_FILENO, &termiosStruct);
   termiosStruct.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(STDIN_FILENO, TCSANOW, &termiosStruct);
}

void* sleepLoop(void* interval) {
   unsigned long long increment = *((unsigned long long*)interval);
   unsigned long long started = getnow();
   unsigned long long current;
   unsigned long long next = started + increment;

   while (1) {
      current = getnow();
         next += increment;

            current = getnow();
            fprintf(stdout, "\r%0.1fs", (current - started) / 1000000.0);
            fflush(stdout);
         }
   return NULL;
}

int main() {
   unsigned long long interval = 100000;
   pthread_t printThread;

   linebuf_off();

   if(pthread_create(&printThread, NULL, sleepLoop, &interval)) {
      fprintf(stderr, "Error: Can't Create Thread\n");

      exit(EXIT_FAILURE);
   }

   pthread_join(printThread, NULL);

   linebuf_on();

   return EXIT_SUCCESS;
}
