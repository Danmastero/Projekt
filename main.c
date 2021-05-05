#include <sys/types.h>
#include "funkcje.h"

// @TODO
/*
- zmienic nazwy zmiennych/printf i syslog
- inny skrypt.sh napisac
-
*/
int main(int argc, char * argv[])
{
    openlog("PROJEKT", LOG_PID|LOG_CONS, LOG_USER);
    if(argc < 3)
    {
        printf("Zbyt mala liczba argumentow wejsciowych\n");
        syslog(LOG_ERR, "Zbyt mala liczba argumentow wejsciowych");
        exit(EXIT_FAILURE);
    }
    pid_t pid, sid;
    pid = fork();
    if(pid < 0)
    {
        syslog(LOG_ERR, "Niepoprawny identyfikator procesu child");
        exit(EXIT_FAILURE);
    }
    printf("TEST POCZATKWOY\n");
    if(pid > 0)
    {
        exit(EXIT_SUCCESS);
    }
    printf("pid > 0 EXIT_SUCCESS\n");
    umask(0);
    sid = setsid();
    if(sid < 0)
    {
        syslog(LOG_ERR, "Blad SessionID");
        exit(EXIT_FAILURE);
    }
    if((chdir("/")) < 0)
    {
        syslog(LOG_ERR, "Problem ze zmiana katalogu roboczego");
        exit(EXIT_FAILURE);
    }


    int wybor = 0, rozmiar = 50;
    bool rekurencja = false;
    char *in, *out;
    int czas_spania_demona = 5 * 60; // domyslne 5 min spania demona
    struct stat s;
    char * folder1_sciezka = NULL;
    char * folder2_sciezka = NULL;
    printf("prawie git\n");
    while((wybor = getopt(argc, argv, "s:i:o:r")) != -1)
    {
        switch(wybor)
        {
        case 's': // przekazanie nowego czasu spania demona
            czas_spania_demona = atoi(optarg);
            printf("SPANKO DZIALA\n");
            break;

        case 'i':
            in = optarg;
            if(stat(in, &s) == 0)
            {
                if(s.st_mode & S_IFDIR) //sciezka jest katalogiem
                {
                    folder1_sciezka = optarg;
                    printf("FOLDER DZIALA\n");
                }
                else //sciezka nie jest katalogiem, wywal blad
                {
                    printf("-i: Podany argument nie jest katalogiem\n");
                    syslog(LOG_ERR, "Podany argument nie jest katalogiem");
                    exit(EXIT_FAILURE);
                }
            }
            break;

        case 'o':
            out = optarg;
            if(stat(out, &s) == 0)
            {
                if(s.st_mode & S_IFDIR) //sciezka jest katalogiem
                {
                    folder2_sciezka = optarg;
                    printf("FOLDER 2 DZIALA\n");
                }
                else //sciezka nie jest katalogiem, wywal blad
                {
                    printf("-o: Podany argument nie jest katalogiem\n");
                    syslog(LOG_ERR, "Podany argument nie jest katalogiem");
                    exit(EXIT_FAILURE);
                }
            }
            break;

        case 'r':
            rekurencja = true;
            break;

        }
    }
    sleep(czas_spania_demona); // spanie demona 
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
	syslog(LOG_INFO, "Demon synchronizuje dwa katalogi");
    if(signal(SIGUSR1, Logowanie) == SIG_ERR)
    {
        syslog(LOG_ERR, "Blad sygnalu!");
        exit(EXIT_FAILURE);
    }


    while(1)
    {
        Usuwanie(folder2_sciezka,folder1_sciezka, folder2_sciezka, rekurencja);
        PrzegladanieFolderu(folder1_sciezka,folder1_sciezka, folder2_sciezka,rekurencja, rozmiar);
        syslog(LOG_INFO, "Demon zostal uspany");
        if((sleep(czas_spania_demona)) == 0)
            syslog(LOG_INFO, "Demon zostalo wybudzony");
    }
    closelog();
    exit(EXIT_SUCCESS);
    return 0;
}
