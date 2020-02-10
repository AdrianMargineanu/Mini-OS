#include "MiniOS.h"
#define MAX_MEM 3 * 1024 * 1024

int VidS(TStiva s) {
  if (s.vf == NULL) {
    return 0;
  }
  return 1;
}

TLista Pop(TStiva *s) {
  TLista aux = s->vf;
  if (s->vf->next_list != NULL) {
    s->vf = s->vf->next_list;
  } else {
    s->vf = NULL;
  }
  aux->next_list = NULL;
  return aux;
}

void Push(TStiva *s, TLista celula) {
  if (VidS(*s) == 0) {
    celula->next_list = NULL;
    s->vf = celula;
  } else {
    celula->next_list = s->vf;
    s->vf = celula;
  }
}

TLista Alloc_Memory(int number) {
  TLista aux;
  TMemory_Process *info;
  aux = (TLista)calloc(1, sizeof(TCelula));
  info = (TMemory_Process *)calloc(1, sizeof(TMemory_Process));
  info->info = number;
  aux->next_list = NULL;
  aux->info = (void *)info;
  return aux;
}

ASt Alloc_Stiva() {
  ASt stiva;
  stiva = (ASt)calloc(1, sizeof(TStiva));
  stiva->vf = NULL;
  return stiva;
}

TLista Alloc_Memory_Zone(unsigned short pid, int memory, int start) {
  TLista aux = NULL;
  TMemory_Zone *info;
  aux = (TLista)calloc(1, sizeof(TCelula));
  aux->next_list = NULL;
  info = (TMemory_Zone *)calloc(1, sizeof(TMemory_Zone));
  info->memory = memory;
  info->pid = pid;
  info->start_memory_zone = start;
  info->memory_used = 0;
  info->stiva_memory = Alloc_Stiva();
  aux->info = (void *)info;
  return aux;
}

void Defragmetation(TLista *memory) {
  if (*memory == NULL) {
    return;
  }
  TLista aux = NULL, before = NULL, start = *memory,
         after = (*memory)->next_list, aux_before, aux_list;

  while (start->next_list != NULL) {
    if (((TMemory_Zone *)start->info)->pid >
        ((TMemory_Zone *)after->info)->pid) {
      aux = after;
      if (start != NULL) {
        if (aux->next_list != NULL) {
          start->next_list = aux->next_list;
        } else {
          start->next_list = NULL;
        }
      } else {
        *memory = aux->next_list;
      }
      aux->next_list = NULL;
      aux_before = NULL;
      aux_list = *memory;
      while (aux_list != NULL &&
             ((TMemory_Zone *)aux->info)->pid >
                 ((TMemory_Zone *)aux_list->info)->pid) {
        aux_before = aux_list;
        aux_list = aux_list->next_list;
      }
      if (aux_list == NULL) {
        if (aux_before != NULL) {
          aux->next_list = *memory;
          *memory = aux;
        } else {
          aux_before->next_list = aux;
        }
      } else {
        if (aux_before == NULL) {
          aux->next_list = *memory;
          *memory = aux;
        } else {
          aux->next_list = aux_before->next_list;
          aux_before->next_list = aux;
        }
      }
    }
    before = start;
    start = after;
    after = after->next_list;
  }
  start = *memory;
  before = NULL;
  while (start->next_list != NULL) {
    if (before == NULL) {
      ((TMemory_Zone *)start->info)->start_memory_zone = 0;
    } else {
      ((TMemory_Zone *)start->info)->start_memory_zone =
          ((TMemory_Zone *)before->info)->start_memory_zone +
          ((TMemory_Zone *)before->info)->memory;
    }
    before = start;
    start = start->next_list;
  }
}

void Delete_Memory(TLista *memory, unsigned short pid, int *memory_used) {
  TLista start = *memory, before = NULL, aux = NULL, aux_stiva = NULL;
  while (start != NULL) {
    if (((TMemory_Zone *)start->info)->pid == pid) {
      aux = start;
      start = start->next_list;
      if (before == NULL) {
        *memory = start;
      } else {
        before->next_list = start;
      }
      aux->next_list = NULL;
      *memory_used = *memory_used - ((TMemory_Zone *)aux->info)->memory;
      while (VidS(*((TMemory_Zone *)aux->info)->stiva_memory) != 0) {
        aux_stiva = Pop(((TMemory_Zone *)aux->info)->stiva_memory);
        free(aux_stiva->info);
        free(aux_stiva);
      }
      free(((TMemory_Zone *)aux->info)->stiva_memory);
      free(aux->info);
      free(aux);
      return;
    } else {
      before = start;
      start = start->next_list;
    }
  }
}

TLista Memory_Zone_Finder(TLista memory, int memory_process) {
  TLista start = memory, after;
  if (start->next_list == NULL) {
    return memory;
  }
  after = start->next_list;
  while (after != NULL) {
    if (((TMemory_Zone *)after->info)->start_memory_zone -
            ((TMemory_Zone *)start->info)->start_memory_zone -
            ((TMemory_Zone *)start->info)->memory >=
        memory_process) {
      return start;
    }
    start = after;
    after = after->next_list;
  }
  if (after == NULL) {
    if (MAX_MEM - ((TMemory_Zone *)start->info)->start_memory_zone -
            ((TMemory_Zone *)start->info)->memory >=
        memory_process) {
      return start;
    }
  }

  return NULL;
}

void Freedom_Memory(TLista *memory) {
  TLista aux, aux_stiva;
  while (*memory != NULL) {
    aux = *memory;
    *memory = (*memory)->next_list;
    while (VidS(*((TMemory_Zone *)aux->info)->stiva_memory) != 0) {
      aux_stiva = Pop(((TMemory_Zone *)aux->info)->stiva_memory);
      free(aux_stiva->info);
      free(aux_stiva);
    }
    free(((TMemory_Zone *)aux->info)->stiva_memory);
    free(aux->info);
    free(aux);
  }
}

void Dell_Mem(FILE *in_file, FILE *out_file, TLista memory) {
  TLista aux;
  unsigned short pid;
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
  aux = Pop(((TMemory_Zone *)memory->info)->stiva_memory);
  ((TMemory_Zone *)memory->info)->memory_used =
      ((TMemory_Zone *)memory->info)->memory_used - 4;
  free(aux->info);
  free(aux);
}

void Save(FILE *in_file, FILE *out_file, TLista memory) {
  TLista aux;
  int info;
  unsigned short pid;
  fscanf(in_file, "%hu%i", &pid, &info);
  while (memory != NULL && pid != ((TMemory_Zone *)memory->info)->pid) {
    memory = memory->next_list;
  }
  if (memory == NULL) {
    fprintf(out_file, "PID %hu not found.\n", pid);
    return;
  }
  if (((TMemory_Zone *)memory->info)->memory_used + 4 >
      ((TMemory_Zone *)memory->info)->memory) {
    fprintf(out_file, "Stack overflow PID %hu.\n", pid);
    return;
  }
  aux = Alloc_Memory(info);
  Push(((TMemory_Zone *)memory->info)->stiva_memory, aux);
  ((TMemory_Zone *)memory->info)->memory_used =
      ((TMemory_Zone *)memory->info)->memory_used + 4;
}
