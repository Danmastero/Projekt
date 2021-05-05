#include <stdio.h>
#include <sys/types.h>
#include "synchro.h"

int main(int argc, char* argv[])
{

  if(argc < 5)
  {
    printf("Zbyt mala liczba argumentow");
    syslog(LOG_ERR, "Zbyt mala liczba argumentow");
    exit(EXIT_FAILURE)
  }
  pid_t pid, sid;
  pid = fork();

  if(pid < 0)
  {
    syslog(LOG_ERR, "Niepoprawny identyfikator procesu child");
    exit(EXIT_FAILURE);
  }
  if(pid > 0)
  {
    exit(EXIT_FAILURE);
  }

  umask(0);

  sid = setsid();

  if(sid < 0)
  {
    syslog(LOG_ERR, "Blad SessionID");
    exit(EXIT_FAILURE);
  }
  if((chdir("/")) < 0)
  {
    syslog(LOG_ERR, "Blad katalogu roboczego");
    exit(EXIT_FAILURE);
  }


  int wybor = 0, rozmiar = 50;
  bool rekurencja = false;
  char *in *out;
  int demon_sen = 5 * 60 // domyslnie 5 min snu demona
  struct stat s;
  char *folder1_sciezka = NULL;
  char *folder2_sciezka = NULL;
  while((wybor = getopt(argc, argv, "s:i:o:m:r")) != -1)
  {
    switch (wybor)
    {
      case 's':
        demon_sen = atoi(optarg);
        break;

      case 'i':
        in = optarg;
        if(stat(in, &s) == 0)
        {
          if(s.st_mode & S_IFDIR)
          {
            folder1_sciezka = optarg;
          }
          else
          {
            printf("-i: podany argument nie jest katalogiem");
            syslog(LOG_ERR, "Podany argument nie jest katalogiem");
            exit(EXIT_FAILURE);
          }
        }
      break;

      case 'o':
        out = optarg;
        if(stat(out, &s) == 0)
        {
          if(s.st_mode & S_IFDIR)
          {
            folder2_sciezka = optarg;
          }
          else
          {
            printf("-o: Podany argumeny nie jest katalogiem");
            syslog(LOG_ERR, "Podany argument nie jest katalogiem");
            exit(EXIT_FAILURE);
          }
        }
        break;
    }
  }

  return 0;
}
