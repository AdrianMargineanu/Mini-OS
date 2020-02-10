#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct list {
  void *info;
  struct list *next_list;
} TCelula, *TLista, **AL;  // structura listei generice

typedef struct {
  TLista vf;     // varful stivei
} TStiva, *ASt;  // structura pentru stiva

typedef struct {
  TLista sl;     // adresa de sfarsit a listi
  TLista elem;   // adresa primului element din coada
} TCoada, *AQV;  // structura unei cozi

typedef struct {
  unsigned short pid;
  int start_memory_zone;  // inceputul zonei de memorie
  int memory;             // memoria totala a procesului
  int memory_used;        // memoria folosita de proces
  ASt stiva_memory;       // stiva de memorie a procesului
} TMemory_Zone;

typedef struct {
  unsigned short pid;
  unsigned short priority;  // prioritatea procesului
  long sec;                 // timpul ramas de executie
  long sec_original;        // timpul total de executie
} TProces;  // structura unui proces in coada de astepare, rulare sau finisare

typedef struct {
  int info;
} TMemory_Process;  // structura unei celule a stivei de memorie

ASt Alloc_Stiva();  // functia ce aloca un pointer de tip stiva
TLista Alloc_Memory_Zone(
    unsigned short pid, int memory,
    int start);      // functia aloca si scrie o celula de tipul TMemory_Zone
int VidQ(TCoada c);  // testeaza daca coada e vida daca da retuneaza 0 daca nu
                     // returneaza 1
TLista ExtrQ(TCoada *c);  // extrage un element din coda si il returneaza
void InsQ(TCoada *c, TLista proces);  // functia insereaza un element in coada
int VidS(TStiva s);  // testeaza daca stiva e vida daca da retuneaza 0 daca nu
                     // returneaza 1
void Push(TStiva *s, TLista celula);  // functia insereaza un element in stiva
TLista Pop(TStiva *s);  //// extrage un element din stiva si il returneaza
AQV Alloc_Coada();      // functia ce aloca un pointer de tip coada
TLista Alloc_Memory(int number);  // aloca o celulade tipul TMemory_Proces
TLista Alloc_Proces(unsigned short pid, unsigned short priority,
                    long sec);  // aloca o celula de tipul TProces
void Inserare_ordonata(TCoada *c, TLista proces);  // insereaza ordonat in coada
void Delete_Memory(TLista *memory, unsigned short pid,
                   int *memory_used);  // sterge o celula din zona de memorie
                                       // dupa terminarea procesului
void Defragmetation(TLista *memory);   // face defragmentarea memoriei
TLista Memory_Zone_Finder(TLista memory, int memory_process);  // Gaseste o zona
                                                               // de memorie
// libera in stiva
// sistemului de
// operare
void Freedom_Memory(
    TLista *memory);  // elibereaza zona de memorie a sistemului de operare
void Freedom_Coada(TCoada *c);  // elibereaza o coada
void Finish(TCoada *process_wait, TCoada *process_finish,
            TCoada *procesor_running,
            FILE *out_file);  // ruleaza procesele ramase in coada de asteptare
void Dell_Mem(
    FILE *in_file, FILE *out_file,
    TLista memory);  // sterge prima celula din stiva de memorie a procesului
void Save(
    FILE *in_file, FILE *out_file,
    TLista memory);  // salveaza informatia in stiva de memorie a procesului
void Get(FILE *in_file, FILE *out_file, TCoada *process_wait,
         TCoada *proces_running, TCoada *process_finish);  // cauta un proces
                                                           // dupa PID si anunta
                                                           // starea procesului