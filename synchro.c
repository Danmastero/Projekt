#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include "synchro.h"

int synchronize(int argc, char** argv)
{
  if(argc != 2)
  {
    fputs("You need to put atleast 2 argument(catalogs).\n", stderr);
    return -1;
  }

}
