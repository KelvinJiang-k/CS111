#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */

  u32 remaining_burst_time;
  bool first_execution;

  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end)
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data)
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }

  munmap((void *)data, size);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */

  struct process *current_process;
  struct process *arriving_process;

  // initialize process list
  for (u32 i = 0; i < size; i++) {
    current_process = &data[i];
    current_process->remaining_burst_time = current_process->burst_time;
    current_process->first_execution = false;
  }

  // initialize current time to minimum arrival time
  u32 current_time = data[0].arrival_time;
  for (u32 i = 0; i < size; i++) {
    current_process = &data[i];
    if (current_process->arrival_time < current_time) {
      current_time = current_process->arrival_time;
    }
  }

  u32 process_time = 0;
  bool active_process = false;
  bool done = false;
  // check for invalid quantum length
  if (quantum_length <= 0) done = true;
  while (!done) {

    // check for arriving processes and append to scheduling queue
    for (u32 i = 0; i < size; i++) {
      arriving_process = &data[i];
      if (current_time == arriving_process->arrival_time) {
        TAILQ_INSERT_TAIL(&list, arriving_process, pointers);
      }
    }

    // if process isn't complete & exceeds timeshare, deschedule process & append to scheduling queue
    if (process_time >= quantum_length) {
      TAILQ_INSERT_TAIL(&list, current_process, pointers);
      // deschedule process
      active_process = false;
      process_time = 0;
    }

    // if no process running & processes ready/in queue, run next process
    if (!active_process && !TAILQ_EMPTY(&list)) {
      current_process = TAILQ_FIRST(&list);
      TAILQ_REMOVE(&list, current_process, pointers);

      // if first runtime, calculate response time
      if (!current_process->first_execution) {
        current_process->first_execution = true;
        total_response_time += current_time - current_process->arrival_time;
      }

      active_process = true;
    }
    
    // ongoing process
    if (active_process) {
      current_process->remaining_burst_time--;
      process_time++;

      // if process completes, calculate wait time
      if (current_process->remaining_burst_time <= 0) {
        total_waiting_time += 1 + current_time - current_process->arrival_time - current_process->burst_time;
        // deschedule process
        active_process = false;
        process_time = 0;
      }
    }
    current_time++;

    // check if all processes finish running
    struct process *temp_process;
    done = true;
    for (u32 i = 0; i < size; i++) {
      temp_process = &data[i];
      if (temp_process->remaining_burst_time > 0) {
        done = false;
      }
    }
  }


  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}
