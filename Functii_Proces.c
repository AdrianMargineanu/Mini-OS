#include "MiniOS.h"

int VidQ(TCoada c) {
  if (c.elem == NULL) {
    return 0;
  }
  return 1;
}

TLista ExtrQ(TCoada *c) {
  TLista aux;
  aux = c->elem;
  if (c->elem == c->sl) {
    c->elem = NULL;
    c->sl = NULL;
  } else {
    c->elem = c->elem->next_list;
  }
  aux->next_list = NULL;
  return aux;
}

AQV Alloc_Coada() {
  AQV coada;
  coada = (AQV)calloc(1, sizeof(TCoada));
  if (coada == NULL) {
    return NULL;
  }
  coada->sl = NULL;
  coada->elem = NULL;
  return coada;
}

void InsQ(TCoada *c, TLista proces) {
  if (c->elem == NULL) {
    c->elem = proces;
    c->elem->next_list = NULL;
  } else {
    c->sl->next_list = proces;
  }
  c->sl = proces;
}

void Inserare_ordonata(TCoada *c, TLista proces) {
  TCoada *aux_coada = Alloc_Coada();
  TLista aux_list = NULL;
  char verify = 0;
  if (VidQ(*c) == 0) {
    InsQ(c, proces);
    free(aux_coada);
    return;
  }
  while (VidQ(*c) != 0) {
    aux_list = ExtrQ(c);
    if (((TProces *)aux_list->info)->priority <
            ((TProces *)proces->info)->priority &&
        verify == 0) {
      InsQ(aux_coada, proces);
      verify = 1;
    }
    if (((TProces *)aux_list->info)->priority ==
            ((TProces *)proces->info)->priority &&
        verify == 0) {
      if (((TProces *)aux_list->info)->sec > ((TProces *)proces->info)->sec &&
          verify == 0) {
        InsQ(aux_coada, proces);
        verify = 1;
      }
      if (((TProces *)aux_list->info)->sec == ((TProces *)proces->info)->sec &&
          verify == 0) {
        if (((TProces *)aux_list->info)->pid > ((TProces *)proces->info)->pid &&
            verify == 0) {
          InsQ(aux_coada, proces);
          verify = 1;
        }
      }
    }
    InsQ(aux_coada, aux_list);
  }
  if (verify == 0) {
    InsQ(aux_coada, proces);
  }
  while (VidQ(*aux_coada) != 0) {
    aux_list = ExtrQ(aux_coada);
    InsQ(c, aux_list);
  }
  free(aux_coada);
}

TLista Alloc_Proces(unsigned short pid, unsigned short priority, long sec) {
  TLista aux = NULL;
  TProces *info;
  aux = (TLista)malloc(sizeof(TCelula));
  if (aux == NULL) {
    return NULL;
  }
  info = (TProces *)malloc(sizeof(TProces));
  info->pid = pid;
  info->priority = priority;
  info->sec = sec;
  info->sec_original = sec;
  aux->info = (void *)info;
  aux->next_list = NULL;
  return aux;
}

void Freedom_Coada(TCoada *c) {
  TLista aux = NULL;
  while (VidQ(*c) != 0) {
    aux = ExtrQ(c);
    free(aux->info);
    free(aux);
  }
  free(c);
}

void Finish(TCoada *process_wait, TCoada *process_finish,
            TCoada *procesor_running, FILE *out_file) {
  TLista aux_list;
  int finish_sec = 0;
  if (VidQ(*procesor_running) != 0) {
    aux_list = ExtrQ(procesor_running);
  } else {
    fprintf(out_file, "Total time: 0\n");
    return;
  }
  finish_sec = finish_sec + ((TProces *)aux_list->info)->sec;
  InsQ(process_finish, aux_list);
  while (VidQ(*process_wait) != 0) {
    aux_list = ExtrQ(process_wait);
    InsQ(process_finish, aux_list);
    finish_sec = finish_sec + ((TProces *)aux_list->info)->sec;
  }
  fprintf(out_file, "Total time: %d\n", finish_sec);
}

void Get(FILE *in_file, FILE *out_file, TCoada *process_wait,
         TCoada *proces_running, TCoada *process_finish) {
  AQV aux = Alloc_Coada();
  TLista aux_list;
  int verify = 0;
  unsigned short pid;
  fscanf(in_file, "%hu", &pid);
  if (VidQ(*proces_running) == 1) {
    aux_list = ExtrQ(proces_running);
    if (((TProces *)aux_list->info)->pid == pid) {
      fprintf(out_file, "Process %hu is running (remaining_time: %lu).\n", pid,
              ((TProces *)aux_list->info)->sec);
      verify = 1;
    }
    InsQ(proces_running, aux_list);
  }
  while (VidQ(*process_wait) != 0) {
    aux_list = ExtrQ(process_wait);
    InsQ(aux, aux_list);
    if (((TProces *)aux_list->info)->pid == pid) {
      fprintf(out_file, "Process %hu is waiting (remaining_time: %lu).\n", pid,
              ((TProces *)aux_list->info)->sec);
      verify = 1;
      break;
    }
  }
  while (VidQ(*process_wait) != 0) {
    aux_list = ExtrQ(process_wait);
    InsQ(aux, aux_list);
  }
  while (VidQ(*aux) != 0) {
    aux_list = ExtrQ(aux);
    InsQ(process_wait, aux_list);
  }
  while (VidQ(*process_finish) != 0) {
    aux_list = ExtrQ(process_finish);
    InsQ(aux, aux_list);
    if (((TProces *)aux_list->info)->pid == pid) {
      fprintf(out_file, "Process %hu is finished.\n", pid);
      verify = 1;
    }
  }
  while (VidQ(*aux) != 0) {
    aux_list = ExtrQ(aux);
    InsQ(process_finish, aux_list);
  }
  if (verify == 0) {
    fprintf(out_file, "Process %hu not found.\n", pid);
  }
  free(aux);
}