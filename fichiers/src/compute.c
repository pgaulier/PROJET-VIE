
#include "compute.h"
#include "graphics.h"
#include "debug.h"
#include "ocl.h"

#include <stdbool.h>
#define YELLOW -65281
unsigned version = 0;

void first_touch_v1 (void);
void first_touch_v2 (void);

unsigned compute_v0 (unsigned nb_iter);
unsigned compute_v1 (unsigned nb_iter);
unsigned compute_v2 (unsigned nb_iter);
unsigned compute_v3 (unsigned nb_iter);

void_func_t first_touch [] = {
  first_touch_v1,
  first_touch_v1,
  first_touch_v1,
  first_touch_v1,
};

int_func_t compute [] = {
  compute_v0,
  compute_v1,
  compute_v2,
  compute_v3,
};

char *version_name [] = {
  "Séquentielle",
  "OpenMP",
  "OpenMP zone",
  "OpenCL",
};

unsigned opencl_used [] = {
  0,
  0,
  0,
  1,
};

///////////////////////////// Version séquentielle simple

unsigned count_v0(int j, int i){
  unsigned S=0;

if (j == 0 && i == 0)
{
  if (cur_img(j, i+1) != 0)
    S++;
  if (cur_img(j+1, i) != 0)
    S++;
  if (cur_img(j+1, i+1) != 0)
    S++;
}
else if (j == 0)
{
  if (cur_img(j, i-1) != 0)
    S++;
  if (cur_img(j, i+1) != 0)
    S++;
  if (cur_img(j+1, i-1) != 0)
    S++;
  if (cur_img(j+1, i) != 0)
    S++;
  if (cur_img(j+1, i+1) != 0)
    S++;
}
else if (i == 0)
{
  if (cur_img(j-1, i) != 0)
    S++;
  if (cur_img(j-1, i+1) != 0)
    S++;
  if (cur_img(j, i+1) != 0)
    S++;
  if (cur_img(j+1, i) != 0)
    S++;
  if (cur_img(j+1, i+1) != 0)
    S++;
}
else
{
  if (cur_img(j-1, i-1) != 0)
    S++;
  if (cur_img(j-1, i) != 0)
    S++;
  if (cur_img(j-1, i+1) != 0)
    S++;
  if (cur_img(j, i-1) != 0)
    S++;
  if (cur_img(j, i+1) != 0)
    S++;
  if (cur_img(j+1, i-1) != 0)
    S++;
  if (cur_img(j+1, i) != 0)
    S++;
  if (cur_img(j+1, i+1) != 0)
    S++;
}









/*

  if (j!=0){
    if (cur_img(j-1, i) != 0){
      S++;
    }
    if (i != 0){
      if (cur_img(j-1, i-1) != 0)
        S++;
    }

    if (i!=DIM){
      if (cur_img(j-1, i+1) != 0)
        S++;
    }
}

if (j!=DIM){
  if (cur_img(j+1, i) != 0)
    S++;
  if (i!=0){
    if (cur_img(j+1, i-1) != 0)
      S++;
  }
  if (i!=DIM){
    if (cur_img(j+1, i+1) != 0)
      S++;
  }
}

if (i!=0){
  if (cur_img(j, i-1) != 0)
    S++;
}
if (i!=DIM){
  if (cur_img(j, i+1) != 0)
    S++;
}*/
return S;
}


unsigned compute_v0 (unsigned nb_iter)
{
  unsigned short stable = 0;
  for (unsigned it = 1; it <= nb_iter; it ++)
  {
    stable = 1;
    for (int i = 0; i < DIM; i++)
    {
      for (int j = 0; j < DIM; j++)
      {
        unsigned S = count_v0(j,i);
        if (S == 3 && cur_img(j, i) == 0)
        {
          next_img(j, i) = YELLOW;
          stable = 0;
        }
        else
        if (cur_img(j, i) != 0 && !(S == 2 || S == 3) )
        {
          next_img(j, i) = 0;
          stable = 0;
        }
        else
          next_img(j,i)=cur_img(j,i);
    }
}
if (stable)
  return it;
    swap_images ();
}
  // retourne le nombre d'étapes nécessaires à la
  // stabilisation du calcul ou bien 0 si le calcul n'est pas
  // stabilisé au bout des nb_iter itérations
  return 0;
}


///////////////////////////// Version OpenMP de base

void first_touch_v1 ()
{
  int i,j ;

#pragma omp parallel for
  for(i=0; i<DIM ; i++) {
    for(j=0; j < DIM ; j += 512)
      next_img (i, j) = cur_img (i, j) = 0 ;
  }
}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v1(unsigned nb_iter)
{
  unsigned short stable = 0;
  for (unsigned it = 1; it <= nb_iter; it ++)
  {
    stable = 1;

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < DIM; i++)
    {
     #pragma omp parallel for schedule(static)
      for (int j = 0; j < DIM; j++)
      {
        unsigned S = count_v0(j,i);
        if (S == 3 && cur_img(j, i) == 0)
        {
          next_img(j, i) = YELLOW;
          stable = 0;
        }
        else
        if (cur_img(j, i) != 0 && !(S == 2 || S == 3) )
        {
          next_img(j, i) = 0;
          stable = 0;
        }
        else
          next_img(j,i)=cur_img(j,i);
    }
  }
  if (stable)
  return it;
    swap_images ();
  }




  return 0;
}



///////////////////////////// Version OpenMP optimisée

void first_touch_v2 ()
{

}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v2(unsigned nb_iter)
{
  return 0; // on ne s'arrête jamais
}


///////////////////////////// Version OpenCL

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v3 (unsigned nb_iter)
{
  return ocl_compute (nb_iter);
}
