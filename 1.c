#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

int logFile;

int openLogFile();

void writeMessage(int, char*);
void sendProcessInfo(int);
void sendSignalInfo(int, siginfo_t);
void signalInfo(int signo, siginfo_t * signalInfo, void * ucontext);

int main(int argc, char * argv[]) {
  if(fork() > 0) {
    exit(EXIT_SUCCESS);
  }
  
  logFile = openLogFile();
  
  struct sigaction action, oldAction;
  
  sendProcessInfo(logFile);
  
  action.sa_sigaction = signalInfo;
  action.sa_flags = SA_SIGINFO;
  
  sigaction(SIGHUP, &action, &oldAction);
  
  while(1) {
    sleep(5);
	
    writeMessage(logFile, "Waiting for signals\n");
  }
  
  close(logFile);
  
  return 0;
}

void signalInfo(int signo, siginfo_t * signalInfo, void * ucontext) {
  sendSignalInfo(logFile, * signalInfo);
}

int openLogFile() {
  int fileToWrite;
  
  const char * fileName = "/home/students/kv9314/lab3/logFile.log";
  
  if((fileToWrite = open(fileName, O_WRONLY | O_APPEND | O_CREAT, 0644)) == -1) {
      perror("OPEN");
	  
      exit(EXIT_FAILURE);
  }
  
  return fileToWrite;
}

void writeMessage(int fileToWrite, char * message) {
  if(write(fileToWrite, message, sizeof(char) * strlen(message)) == -1) {
      perror("WRITE");
  }
}

void sendProcessInfo(int fileToWrite) {
  char * outMessage;
  
  asprintf(&outMessage, "Starting a process (PID: %d)\n", getpid());
  
  writeMessage(fileToWrite, outMessage);
  
  free(outMessage);
}

void sendSignalInfo(int fileToWrite, siginfo_t signalInfo) {
  char * outMessage;
  
  asprintf(&outMessage, "Signal signalInfo:"
           "\n\tsi_signo = %d, si_errno = %d, si_code = %d,"
           "\n\tsi_pid = %d, si_uid = %d, si_status = %d,"
           "\n\tsi_utime = %Lf, si_stime = %Lf, si_value = %d,"
           "\n\tsi_int = %d, si_band = %d, si_fd = %d\n", signalInfo.si_signo, signalInfo.si_errno, signalInfo.si_code, signalInfo.si_pid, signalInfo.si_uid,
		   signalInfo.si_status, signalInfo.si_utime, signalInfo.si_stime, signalInfo.si_value, signalInfo.si_int, signalInfo.si_band, signalInfo.si_fd);
		   
  writeMessage(fileToWrite, outMessage);
  
  free(outMessage);
}
