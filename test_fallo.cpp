//#include "io_posix.h"
#include <errno.h>
#include <fcntl.h>
#include <algorithm>
#include <linux/fs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/statfs.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>

//#include "posix_logger.h"
//#include "monitoring/iostats_context_imp.h"
//#include "port/port.h"
//#include "rocksdb/slice.h"
//#include "util/coding.h"
//#include "util/string_util.h"
//#include "util/sync_point.h"

#define IOSTATS_TIMER_GUARD(metric)
bool allow_fallocate_;

int main(int argc, char *argv[]) {
  //int flags = O_CREAT | O_APPEND | O_WRONLY;
  //int fd = open("testfile", flags, 0644);
  //int alloc_status = fallocate(fd, FALLOC_FL_KEEP_SIZE, 0, 9192);
  //std::string buf(5000, 'a');
  //ssize_t done = write(fd, buf.data(), 5000);
  //if (done != 5000) printf("Error\n");
  //int r = ftruncate(fd, 5000);
  //if (r == -1) printf("Error\n");
  //r = fallocate(fd, FALLOC_FL_KEEP_SIZE | FALLOC_FL_PUNCH_HOLE, 5000, 4192);
  //if (r == -1) printf("Error\n");
  //close(fd);
  //
  //int fd_;
  int flags = O_CREAT | O_APPEND | O_WRONLY;
  int fd_ = open("testfile", flags, 0644);
  int alloc_status = fallocate(fd_, FALLOC_FL_KEEP_SIZE, 0, 9192);
  std::string buf(5000, 'a');
  ssize_t done = write(fd_, buf.data(), 5000);
  if (done != 5000) printf("Error\n");
  int r = ftruncate(fd_, 5000);
  if (r == -1) printf("Error\n");
  struct stat file_stats;

  allow_fallocate_ = true;
  fstat(fd_, &file_stats);
  // After ftruncate, we check whether ftruncate has the correct behavior.
  // If not, we should hack it with FALLOC_FL_PUNCH_HOLE
  static bool buggy = false;

  if (!buggy && (file_stats.st_size + file_stats.st_blksize - 1) /
          file_stats.st_blksize !=
      file_stats.st_blocks / (file_stats.st_blksize / 512)) {
    fprintf(stderr,
            "WARNING: Your kernel is buggy (<= 4.0.x) and does not free"
            "preallocated blocks on truncate. Hacking around it, but you"
            "should upgrade!\n");
    buggy = true;
  }
  if (buggy) {
    IOSTATS_TIMER_GUARD(allocate_nanos);
    if (allow_fallocate_) {
      fallocate(fd_, FALLOC_FL_KEEP_SIZE | FALLOC_FL_PUNCH_HOLE, 5000, 4192);
    }
    printf("wrong\n");
  }else {
    printf("right\n");
  }
}
