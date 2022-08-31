#include <error.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define BRIDGE 0xC0000000
#define BRIDGE_SPAN 0x20

struct alu
{
  uint64_t a;
  uint64_t b;
  uint64_t cntl;
  uint64_t result;
};

int main(int argc, char **argv)
{

  struct alu *map = NULL; /* mmapped array of struct med */

  int fd = 0;
  int result = 0;

  if (argc != 4)
  {
    perror("Only 2 numbers should be passed followed by a mode: \n\tadd\n\tsub");
    return -1;
  }

  fd = open("/dev/mem", O_RDWR | O_SYNC);

  if (fd < 0)
  {
    perror("Couldn't open /dev/mem\n");
    return -2;
  }

  map = (struct alu *)mmap(NULL, BRIDGE_SPAN, PROT_READ | PROT_WRITE,
                           MAP_SHARED, fd, BRIDGE);

  if (map == MAP_FAILED)
  {
    perror("mmap failed.");
    close(fd);
    return -3;
  }

  map[0].a = atoi(argv[1]);
  map[0].b = atoi(argv[2]);
  map[0].cntl = atoi(argv[3]);

  // if (argv[3] == "add")
  // {
  //   map[0].cntl = 0b11000000;
  // }
  // else if (argv[3] == "sub")
  // {
  //   map[0].cntl = 0b00000100;
  // }

  // printf("%" PRIu64 "\n", (long long unsigned int)map[0].a);
  // printf("a: 0x%llx %lld \n", map[0].a, map[0].a);
  // printf("b: 0x%llx %lld \n", map[0].b, map[0].b);
  // printf("cntl: 0x%llx %lld \n", map[0].cntl, map[0].cntl);
  printf("result: 0x%llx %lld \n", map[0].result, map[0].result);

  result = munmap(map, BRIDGE_SPAN);

  if (result < 0)
  {
    perror("Couldnt unmap bridge.");
    close(fd);
    return -4;
  }

  close(fd);
  return 0;
}