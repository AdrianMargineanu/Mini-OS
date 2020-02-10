#include "MiniOS.h"
#define MAX_MEM 3 * 1024 * 1024

unsigned short PID_Finder(TLista memory) {  // gaseste PID-ul liber
  unsigned short pid = 1, old_pid = 1;
  TLista start = memory;
  while (old_pid == 1) {
    old_pid = 0;
    start = memory;
    while (start != NULL) {
      if (((TMemory_Zone *)start->info)->pid == pid) {
        old_pid = 1;
        // pid++;
      }
      start = start->next_list;
    }
    if (old_pid == 1) {
      pid++;
    } else {
      return pid;
    }
  }
  return pid;
}

void Run(TCoada *process_wait, TCoada *process_finish, TCoada *procesor_running,
         TLista *memory, long sec, int T, int *memory_used) {
  TLista aux_list, aux_wait;
  AQV aux = Alloc_Coada();
  long cuanta;
  static long cuanta_remain = 0;

  while (sec > 0) {
    
    if (cuanta_remain == 0) {
      if (T <= sec) {
        cuanta = T;
        cuanta_remain = 0;
      } else {
        cuanta = sec;
        cuanta_remain = T - cuanta;
      }
    } else {
      if (cuanta_remain <= sec) {
        cuanta = cuanta_remain;
        cuanta_remain = 0;
      } else {
        cuanta = sec;
        cuanta_remain = cuanta_remain - cuanta;
      }
    }
    if (VidQ(*procesor_running) != 0) {
      aux_list = ExtrQ(procesor_running);
    } else {
      sec = 0;
      continue;
    }
    if (((TProces *)aux_list->info)->sec > cuanta) {
      ((TProces *)aux_list->info)->sec =
          ((TProces *)aux_list->info)->sec - cuanta;
    } else {
      cuanta_remain = 0;
      cuanta = ((TProces *)aux_list->info)->sec;
      ((TProces *)aux_list->info)->sec = 0;
    }
    sec = sec - cuanta;
    if (((TProces *)aux_list->info)->sec > 0) {
      aux_list->next_list = NULL;
      if (VidQ(*process_wait) == 0) {
        InsQ(procesor_running, aux_list);
      } else {
        if (cuanta_remain == 0) {
          aux_wait = ExtrQ(process_wait);
          InsQ(procesor_running, aux_wait);
          Inserare_ordonata(process_wait, aux_list);
        } else {
          InsQ(procesor_running, aux_list);
        }
      }
    } else {
      InsQ(process_finish, aux_list);
      if (VidQ(*process_wait) != 0) {
        aux_wait = ExtrQ(process_wait);
        InsQ(procesor_running, aux_wait);
      } else {
        sec = 0;
      }
      Delete_Memory(memory, ((TProces *)aux_list->info)->pid, memory_used);
      aux_list = NULL;
    }
  }
  free(aux);
}

void Print(FILE *in_file, FILE *out_file, TCoada *process_wait,
           TCoada *process_finish, TLista memory) {
  char argument[9];
  AQV aux_coad;
  ASt aux_stiva;
  TLista aux_list;
  unsigned short pid;
  fscanf(in_file, "%s", argument);
  if (strcmp(argument, "stack") == 0) {
    fscanf(in_file, "%hu", &pid);
    while (memory != NULL && pid != ((TMemory_Zone *)memory->info)->pid) {
      memory = memory->next_list;
    }
    if (memory == NULL) {
      fprintf(out_file, "PID %hu not found.\n", pid);
      return;
    }
    if (VidS(*((TMemory_Zone *)memory->info)->stiva_memory) == 0) {
      fprintf(out_file, "Empty stack PID %hu.\n", pid);
      return;
    }
    aux_stiva = Alloc_Stiva();
    while (VidS(*((TMemory_Zone *)memory->info)->stiva_memory) != 0) {
      aux_list = Pop(((TMemory_Zone *)memory->info)->stiva_memory);
      Push(aux_stiva, aux_list);
    }
    fprintf(out_file, "Stack of PID %hu:", pid);
    while (VidS(*aux_stiva) != 0) {
      aux_list = Pop(aux_stiva);
      Push(((TMemory_Zone *)memory->info)->stiva_memory, aux_list);
      fprintf(out_file, " %d", ((TMemory_Process *)aux_list->info)->info);
    }
    fprintf(out_file, ".\n");
    free(aux_stiva);
  }
  if (strcmp(argument, "waiting") == 0) {
    aux_coad = Alloc_Coada();
    fprintf(out_file, "Waiting queue:\n[");
    while (VidQ(*process_wait) != 0) {
      aux_list = ExtrQ(process_wait);
      InsQ(aux_coad, aux_list);
      fprintf(out_file, "(%hu: priority = %hu, remaining_time = %lu)",
              ((TProces *)aux_list->info)->pid,
              ((TProces *)aux_list->info)->priority,
              ((TProces *)aux_list->info)->sec);
      if (VidQ(*process_wait) != 0) {
        fprintf(out_file, ",\n");
      }
    }
    fprintf(out_file, "]\n");
    while (VidQ(*aux_coad) != 0) {
      aux_list = ExtrQ(aux_coad);
      InsQ(process_wait, aux_list);
    }
    free(aux_coad);
  }
  if (strcmp(argument, "finished") == 0) {
    aux_coad = Alloc_Coada();
    fprintf(out_file, "Finished queue:\n[");
    while (VidQ(*process_finish) != 0) {
      aux_list = ExtrQ(process_finish);
      InsQ(aux_coad, aux_list);
      fprintf(out_file, "(%hu: priority = %hu, executed_time = %lu)",
              ((TProces *)aux_list->info)->pid,
              ((TProces *)aux_list->info)->priority,
              ((TProces *)aux_list->info)->sec_original);
      if (VidQ(*process_finish) != 0) {
        fprintf(out_file, ",\n");
      }
    }
    fprintf(out_file, "]\n");
    while (VidQ(*aux_coad) != 0) {
      aux_list = ExtrQ(aux_coad);
      InsQ(process_finish, aux_list);
    }
    free(aux_coad);
  }
}

void Add_Procces(FILE *in_file, FILE *out_file, TLista *memory, TCoada *proces,
                 TCoada *running, int *memory_used) {
  int memory_process, memory_process_start;
  long sec;
  unsigned short priority, pid = 1;
  TLista aux, before = NULL, new_proces;
  fscanf(in_file, "%d%ld%hd", &memory_process, &sec, &priority);
  pid = PID_Finder(*memory);
  if (*memory_used + memory_process > (int)MAX_MEM) {
    fprintf(out_file, "Cannot reserve memory for PID %hu.\n", pid);
    return;
  }
  if (*memory == NULL) {
    memory_process_start = 0;
    before = NULL;
  } else {
    if (((TMemory_Zone *)(*memory)->info)->start_memory_zone >=
        memory_process) {
      before = NULL;
      memory_process_start = 0;
    } else {
      before = Memory_Zone_Finder(*memory, memory_process);
      if (before == NULL) {
        Defragmetation(memory);
        before = Memory_Zone_Finder(*memory, memory_process);
      }
      memory_process_start = ((TMemory_Zone *)before->info)->start_memory_zone
                                + ((TMemory_Zone *)before->info)->memory;
    }
  }
  aux = Alloc_Memory_Zone(pid, memory_process, memory_process_start);
  if (before == NULL) {
    aux->next_list = *memory;
    *memory = aux;
  } else {
    aux->next_list = before->next_list;
    before->next_list = aux;
  }
  new_proces = Alloc_Proces(pid, priority, sec);
  if (VidQ(*running) == 0) {
    InsQ(running, new_proces);
  } else {
    Inserare_ordonata(proces, new_proces);
  }
  fprintf(out_file,
          "Process created successfully: PID: %hu, Memory starts at 0x%x.\n",
          pid, memory_process_start);
  *memory_used = *memory_used + memory_process;
}

int main(int argc, char *argv[]) {
  FILE *in_file, *out_file;
  unsigned long T;
  long sec;
  int memory_used = 0;
  char operation[7];
  AQV procesor_in_work = NULL, procesor_finished = NULL,
      procesor_running = NULL;
  TLista memory = NULL;

  if (argc < 3) {
    printf("ERROR, not enough files!\n");
    return 0;
  }
  in_file = fopen(argv[1], "r");
  if (in_file == NULL) {
    printf("ERROR, can not open %s file\n", argv[1]);
    return 0;
  }
  out_file = fopen(argv[2], "w");
  if (out_file == NULL) {
    printf("ERROR, can not open %s file\n", argv[2]);
    return 0;
  }
  fscanf(in_file, "%lu", &T);

  procesor_in_work = Alloc_Coada();
  if (procesor_in_work == NULL) {
    printf("OUT OF MEMORY\n");
    return 0;
  }
  procesor_finished = Alloc_Coada();
  if (procesor_finished == NULL) {
    printf("OUT OF MEMORY\n");
    return 0;
  }
  procesor_running = Alloc_Coada();
  if (procesor_running == NULL) {
    printf("OUT OF MEMORY\n");
    return 0;
  }
  while (fscanf(in_file, "%s", operation) != EOF) {
    // printf("%s\n", operation);
    if (strcmp(operation, "add") == 0) {
      Add_Procces(in_file, out_file, &memory, procesor_in_work,
                  procesor_running, &memory_used);
    }
    if (strcmp(operation, "get") == 0) {
      Get(in_file, out_file, procesor_in_work, procesor_running,
          procesor_finished);
    }
    if (strcmp(operation, "push") == 0) {
      Save(in_file, out_file, memory);
    }
    if (strcmp(operation, "pop") == 0) {
      Dell_Mem(in_file, out_file, memory);
    }
    if (strcmp(operation, "print") == 0) {
      Print(in_file, out_file, procesor_in_work, procesor_finished, memory);
    }
    if (strcmp(operation, "run") == 0) {
      fscanf(in_file, "%ld", &sec);
      Run(procesor_in_work, procesor_finished, procesor_running, &memory, sec,
          T, &memory_used);
    }
    if (strcmp(operation, "finish") == 0) {
      Finish(procesor_in_work, procesor_finished, procesor_running, out_file);
    }
  }
  Freedom_Memory(&memory);
  Freedom_Coada(procesor_running);
  Freedom_Coada(procesor_finished);
  Freedom_Coada(procesor_in_work);
  return 0;
}