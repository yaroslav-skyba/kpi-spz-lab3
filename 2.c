#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>

#define MAX_LENGTH 100

struct sharedMemory {
  int processId;
  
  time_t timeMarker;
  
  char message[MAX_LENGTH];
};

void handleError(char *);
void printInfo(struct sharedMemory *);
void setNewInfo(struct sharedMemory *, char *);

int main(int argc, char * argv[]) {
  struct sharedMemory * sharedMemory;
  
  int fd;
  
  char message[MAX_LENGTH];
  
  if((fd = shm_open("/myregion", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) < 0) {
      handleError("shm_open");
  }

  if (ftruncate(fd, sizeof(struct sharedMemory)) < 0) {
      handleError("ftruncate");
  }
  
  sharedMemory = mmap(NULL, sizeof(struct sharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
  if(sharedMemory == MAP_FAILED) {
      handleError("ftruncate");
  }
  
  while(1) {
    printf("Input: ");
	
    fgets(message, MAX_LENGTH, stdin);
	
    printInfo(sharedMemory);
	
    setNewInfo(sharedMemory, message);
  }
  
  return 0;
}

void setNewInfo(struct sharedMemory * sharedMemory, char * message){
    sharedMemory->processId = getpid();
	
    time(&sharedMemory->timeMarker);
	
    strcpy(sharedMemory->message, message);
	
    if(msync(sharedMemory, sizeof(struct sharedMemory), MS_SYNC) < 0) {
      handleError("msync");
	}
}

void printInfo(struct sharedMemory * sharedMemory) {
  struct tm * timeInfo;
  
  char buffer [80];
  
  timeInfo = localtime(&sharedMemory->timeMarker);
  
  printf("[%02d:%02d:%02d] A process %d: %s", timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, sharedMemory->processId, sharedMemory->message);
}

void handleError(char * message) {
  perror(message);
  
  exit(EXIT_FAILURE);
}
