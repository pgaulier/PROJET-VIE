
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
unsigned compute_v4 (unsigned nb_iter);
unsigned compute_v5 (unsigned nb_iter);
unsigned compute_v6 (unsigned nb_iter);
unsigned compute_v7 (unsigned nb_iter);
unsigned compute_v8 (unsigned nb_iter);

void_func_t first_touch [] = {
  first_touch_v1,
  first_touch_v1,
  first_touch_v1,
  first_touch_v1,
  first_touch_v1,
  first_touch_v1,
  first_touch_v1,
  first_touch_v1,
  first_touch_v1
};

int_func_t compute [] = {
  compute_v0,
  compute_v1,
  compute_v5,
  compute_v2,
  compute_v3,
  compute_v6,
  compute_v7,
  compute_v8,
  compute_v4
};

char *version_name [] = {
  "Séquentielle",
  "Séquentielle tuilée",
  "Séquentielle tuilée optimisée",
  "OpenMP",
  "OpenMP tuilée",
  "OpenMP tuilée optimisée",
  "OpenMP task tuilée",
  "OpenMP task tuilée optimisée",
  "OpenCL",
};

unsigned opencl_used [] = {
  0,
  0,
  0,
  0,
  0,
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
  if (i == DIM - 1)
  {
    if (cur_img(j, i-1) != 0)
      S++;
    if (cur_img(j+1, i-1) != 0)
      S++;
    if (cur_img(j+1, i) != 0)
      S++;
  }
  else
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
}
else if (i == 0)
{
  if (j == DIM - 1)
  {
    if (cur_img(j-1, i) != 0)
      S++;
    if (cur_img(j-1, i+1) != 0)
      S++;
    if (cur_img(j, i+1) != 0)
      S++;
  }
  else
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
}
else if (i == DIM - 1)
{
  if (cur_img(j-1, i) != 0)
    S++;
  if (cur_img(j-1, i-1) != 0)
    S++;
  if (cur_img(j, i-1) != 0)
    S++;
  if (cur_img(j+1, i) != 0)
    S++;
  if (cur_img(j+1, i-1) != 0)
    S++;
}
else if (j == DIM - 1)
{
  if (cur_img(j-1, i) != 0)
    S++;
  if (cur_img(j-1, i+1) != 0)
    S++;
  if (cur_img(j, i-1) != 0)
    S++;
  if (cur_img(j, i) != 0)
    S++;
  if (cur_img(j, i+1) != 0)
    S++;
}
else if (i == DIM - 1 && j == DIM - 1)
{
  if (cur_img(j-1, i) != 0)
    S++;
  if (cur_img(j-1, i-1) != 0)
    S++;
  if (cur_img(j, i-1) != 0)
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
        unsigned S = count_v0(j, i);
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
          next_img(j, i) = cur_img(j, i);
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

///////////////////////////// Version séquentielle tuilée

#define TILE_SIZE 32 //dimension des tuiles

unsigned compute_v1(unsigned nb_iter)
{
  unsigned short stable = 0;
  for (unsigned it = 1; it <= nb_iter; it ++)
  {
    stable = 1;
    for (int x = 0; x < DIM; x += TILE_SIZE)
    {
      for (int y = 0; y < DIM; y += TILE_SIZE)
      {
        for (int xloc = x; xloc < x + TILE_SIZE && xloc < DIM; xloc++)
        {
          for (int yloc = y; yloc < y + TILE_SIZE && yloc < DIM; yloc++)
          {
            unsigned S = count_v0(yloc, xloc);
            int ci = cur_img(yloc, xloc);
            if (S == 3 && ci == 0)
            {
              next_img(yloc, xloc) = YELLOW;
              stable = 0;
            }
            else
            if (ci != 0 && !(S == 2 || S == 3) )
            {
              next_img(yloc, xloc) = 0;
              stable = 0;
            }
            else
            next_img(yloc, xloc) = ci;
          }
        }
      }
    }
    if (stable)
      return it;
    swap_images ();
  }
  return 0;
}


///////////////////////////// Version séquentielle tuilée optimisée

#define TRANCHE DIM / TILE_SIZE

void free_stableloc(unsigned short **s)
{
  for (int i = 0; i < TRANCHE; i++)
  {
    free(s[i]);
  }
  free(s);
}

void init_stableloc(unsigned short **s)
{
  for (int i = 0; i < TRANCHE; i++)
  {
    s[i] = malloc(sizeof(unsigned short) * TRANCHE);
    for (int j = 0; j < TRANCHE; j++)
    {
      s[i][j] = 0;
    }
  }
}

unsigned short check_for_stable(int x, int y, unsigned short **s)
{
  int i, j;
  if (x == 0 && y == 0)
  {
    if (s[0][1] == 0 || s[1][0] == 0 || s[1][1])
      return 0;
  }
  else if (x == 0)
  {
    for (i = 0; i <= 1; i++)
    {
      for (j = -1; j <= 1; j++)
      {
        if ((s[x+i][y+j] == 0) && (i != 0 || j != 0))
          return 0;
      }
    }
  }
  else if (y == 0)
  {
    for (i = -1; i <= 1; i++)
    {
      for (j = 0; j <= 1; j++)
      {
        if ((s[x+i][y+j] == 0) && (i != 0 || j != 0))
          return 0;
      }
    }
  }
  else
  for (i = -1; i <= 1; i++)
  {
    for (j = -1; j <= 1; j++)
    {
      if ((s[x+i][y+j] == 0) && (i != 0 || j != 0))
        return 0;
    }
  }
  return 1;
}

void update_stableloc(unsigned short **s, unsigned short **ns)
{
  for (int x = 1; x < TRANCHE-1; x++)
  {
    for (int y = 1; y < TRANCHE-1; y++)
    {
      unsigned short b = 0;
      for (int i = -1; i <= 1; i++)
      {
        for (int j = -1; j <= 1; j++)
        {
          if (s[x+i][y+j] == 0)
          {
            ns[x][y] = 0;
            b = 1;
          }
        }
        if (!b)
          ns[x][y] = 1;
      }
    }
  }
}

unsigned compute_v5(unsigned nb_iter)
{
  unsigned short stable = 0;
  unsigned short **stableloc = malloc(sizeof(unsigned short) * TRANCHE * TRANCHE);
  unsigned short **next_stableloc = malloc(sizeof(unsigned short) * TRANCHE * TRANCHE);
  init_stableloc(stableloc);
  init_stableloc(next_stableloc);
  for (unsigned it = 1; it <= nb_iter; it ++)
  {
    stable = 1;
    for (int x = 0; x < TRANCHE; x++)
    {
      for (int y = 0; y < TRANCHE; y++)
      {
        stableloc[x][y] = 1;
        if (next_stableloc[x][y] == 0)
        {
          for (int xloc = x * TILE_SIZE; xloc < (x+1) * TILE_SIZE && xloc < DIM; xloc++)
          {
            for (int yloc = y * TILE_SIZE; yloc < (y+1) * TILE_SIZE && yloc < DIM; yloc++)
            {
              unsigned S = count_v0(yloc, xloc);
              int ci = cur_img(yloc, xloc);
              if (S == 3 && ci == 0)
              {
                next_img(yloc, xloc) = YELLOW;
                stable = 0;
                stableloc[x][y] = 0;
              }
              else
              if (ci != 0 && !(S == 2 || S == 3) )
              {
                next_img(yloc, xloc) = 0;
                stable = 0;
                stableloc[x][y] = 0;
              }
              else
              next_img(yloc, xloc) = ci;
            }
          }
        }
      }
    }
    update_stableloc(stableloc, next_stableloc);
    if (stable)
    {
      free_stableloc(stableloc);
      free_stableloc(next_stableloc);
      return it;
    }
    swap_images ();
  }
  free_stableloc(stableloc);
  free_stableloc(next_stableloc);
  return 0;
}

///////////////////////////// Version OpenMP de base

void first_touch_v1 ()
{
  int i, j;
  #pragma omp parallel for
    for(i=0; i<DIM ; i++) {
      for(j=0; j < DIM ; j += 512)
        next_img (i, j) = cur_img (i, j) = 0 ;
  }
}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v2(unsigned nb_iter)
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



///////////////////////////// Version OpenMP tuilée

void first_touch_v2 ()
{

}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v3(unsigned nb_iter)
{
  int x, y;
  unsigned short stable = 0;
  for (unsigned it = 1; it <= nb_iter; it ++)
  {
    stable = 1;
    #pragma omp parallel for private(y) collapse(2)
    for (x = 0; x < DIM; x += TILE_SIZE)
    {
      for (y = 0; y < DIM; y += TILE_SIZE)
      {
        for (int xloc = x; xloc < x + TILE_SIZE && xloc < DIM; xloc++)
        {
          for (int yloc = y; yloc < y + TILE_SIZE && yloc < DIM; yloc++)
          {
            unsigned S = count_v0(yloc, xloc);
            int ci = cur_img(yloc, xloc);
            if (S == 3 && ci == 0)
            {
              next_img(yloc, xloc) = YELLOW;
              stable = 0;
            }
            else
            if (ci != 0 && !(S == 2 || S == 3) )
            {
              next_img(yloc, xloc) = 0;
              stable = 0;
            }
            else
            next_img(yloc, xloc) = ci;
          }
        }
      }
    }
    if (stable)
      return it;
    swap_images ();
  }
  return 0; // on ne s'arrête jamais
}

///////////////////////////// Version OpenMP tuilée optimisée

unsigned compute_v6 (unsigned nb_iter)
{
  int x, y;
  unsigned short stable = 0;
  unsigned short **stableloc = malloc(sizeof(unsigned short) * TRANCHE * TRANCHE);
  unsigned short **next_stableloc = malloc(sizeof(unsigned short) * TRANCHE * TRANCHE);
  init_stableloc(stableloc);
  init_stableloc(next_stableloc);
  for (unsigned it = 1; it <= nb_iter; it ++)
  {
    stable = 1;
    #pragma omp parallel for private(y) collapse(2)
    for (x = 0; x < TRANCHE; x++)
    {
      for (y = 0; y < TRANCHE; y++)
      {
        stableloc[x][y] = 1;
        if (next_stableloc[x][y] == 0)
        {
          for (int xloc = x * TILE_SIZE; xloc < (x+1) * TILE_SIZE && xloc < DIM; xloc++)
          {
            for (int yloc = y * TILE_SIZE; yloc < (y+1) * TILE_SIZE && yloc < DIM; yloc++)
            {
              unsigned S = count_v0(yloc, xloc);
              int ci = cur_img(yloc, xloc);
              if (S == 3 && ci == 0)
              {
                next_img(yloc, xloc) = YELLOW;
                stable = 0;
                stableloc[x][y] = 0;
              }
              else
              if (ci != 0 && !(S == 2 || S == 3) )
              {
                next_img(yloc, xloc) = 0;
                stable = 0;
                stableloc[x][y] = 0;
              }
              else
              next_img(yloc, xloc) = ci;
            }
          }
        }
      }
    }
    update_stableloc(stableloc, next_stableloc);
    if (stable)
    {
      free_stableloc(stableloc);
      free_stableloc(next_stableloc);
      return it;
    }
    swap_images ();
  }
  free_stableloc(stableloc);
  free_stableloc(next_stableloc);
  return 0;
}
///////////////////////////// Version OpenMP task tuilée

unsigned compute_v7 (unsigned nb_iter)
{
  int x, y;
  unsigned short stable = 0;
  for (unsigned it = 1; it <= nb_iter; it ++)
  {
    stable = 1;
    #pragma omp parallel
    {
    #pragma omp single
    for (x = 0; x < DIM; x += TILE_SIZE)
    {
      for (y = 0; y < DIM; y += TILE_SIZE)
      {
        #pragma omp task firstprivate(x, y)
        for (int xloc = x; xloc < x + TILE_SIZE && xloc < DIM; xloc++)
        {
          for (int yloc = y; yloc < y + TILE_SIZE && yloc < DIM; yloc++)
          {
            unsigned S = count_v0(yloc, xloc);
            int ci = cur_img(yloc, xloc);
            if (S == 3 && ci == 0)
            {
              next_img(yloc, xloc) = YELLOW;
              stable = 0;
            }
            else
            if (ci != 0 && !(S == 2 || S == 3) )
            {
              next_img(yloc, xloc) = 0;
              stable = 0;
            }
            else
            next_img(yloc, xloc) = ci;
          }
        }
      }
    }
  }
    if (stable)
      return it;
    swap_images ();
  }
  return 0;
}

///////////////////////////// Version OpenMP task tuilée optimisée

unsigned compute_v8 (unsigned nb_iter)
{
  return 0;
}

///////////////////////////// Version OpenCL

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v4 (unsigned nb_iter)
{
  return ocl_compute (nb_iter);
}
