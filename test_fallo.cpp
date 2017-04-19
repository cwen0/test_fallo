#include <linux/falloc.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string>

int main(int argc, char *argv[]) {
  int flags = O_CREAT | O_APPEND | O_WRONLY;
  int fd = open("testfile", flags, 0644);
  int alloc_status = fallocate(fd, FALLOC_FL_KEEP_SIZE, 0, 9192);
  std::string buf(5000, 'a');
  ssize_t done = write(fd, buf.data(), 5000);
  if (done != 5000) printf("Error\n");
  int r = ftruncate(fd, 5000);
  if (r == -1) printf("Error\n");
  r = fallocate(fd, FALLOC_FL_KEEP_SIZE | FALLOC_FL_PUNCH_HOLE, 5000, 4192);
  if (r == -1) printf("Error\n");
  close(fd);
}
