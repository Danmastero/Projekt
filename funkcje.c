#include "funkcje.h"

time_t pobierz_czas(char* wej)
{
    struct stat czas;
    if(stat(wej, &czas) == -1)
    {
        syslog(LOG_ERR, "Blad z pobraniem daty modyfikacji dla pliku %s", wej);
        exit(EXIT_FAILURE);
    }
    return czas.st_mtime;
}
mode_t pobierz_chmod(char *wej)
{
    struct stat mod;
    if(stat(wej, &mod) == -1)
    {
        syslog(LOG_ERR, "Blad pobrania chmod dla pliku %s", wej);
        exit(EXIT_FAILURE);
    }
    return mod.st_mode;
}

void zmien_parametry(char* wej, char *wyj)
{
    struct utimbuf czas;
    czas.actime = 0;
    czas.modtime = pobierz_czas(wej);
    if(utime(wyj, &czas) != 0)
    {
        syslog(LOG_ERR, "Blad zwiazany z data modyfikacji!");
        exit(EXIT_FAILURE);
    }
    mode_t stary = pobierz_chmod(wej);
    if(chmod(wyj, stary)!= 0)
    {
        syslog(LOG_ERR, "Blad ustawienia uprawnien do pliku!");
        exit(EXIT_FAILURE);
    }
}
char *podmien_folder2(char * sciezka1, char* sciezka_folderu1, char* sciezka_folderu2)
{
    char * sciezka = sciezka1+strlen(sciezka_folderu2);
    char * nowa_sciezka = malloc(strlen(sciezka_folderu1)+strlen(sciezka)+1);
    strcpy(nowa_sciezka,sciezka_folderu1);
    strcat(nowa_sciezka,sciezka);
    return nowa_sciezka;
}
char *podmien_folder1(char * sciezka1, char* sciezka_folderu1, char* sciezka_folderu2)
{
    char*sciezka = sciezka1+strlen(sciezka_folderu1);
    char * nowa_sciezka = malloc(strlen(sciezka_folderu2)+strlen(sciezka)+1);
    strcpy(nowa_sciezka,sciezka_folderu2);
    strcat(nowa_sciezka,sciezka);
    return nowa_sciezka;
}
char *dodaj_do_sciezki(char* sciezka,char *dodatek)
{
    char * nowa_sciezka = malloc(strlen(sciezka)+2+strlen(dodatek));
    strcpy(nowa_sciezka,sciezka);
    strcat(nowa_sciezka,"/");
    strcat(nowa_sciezka, dodatek);
    nowa_sciezka[strlen(sciezka) + 1 + strlen(dodatek)] ='\0';
    return nowa_sciezka;
}
bool sprawdzanie(char * nazwa_sciezki, char* sciezka_folderu1, char* sciezka_folderu2)
{
    bool wynik = 0;
    char *nazwa_sciezki_zm = nazwa_sciezki+strlen(sciezka_folderu1);
    char *szukamy = malloc(strlen(nazwa_sciezki_zm));
    char * nowa_sciezka = podmien_folder1(nazwa_sciezki,sciezka_folderu1,sciezka_folderu2);

    int i=strlen(nowa_sciezka);
    for (i; nowa_sciezka[i]!='/' ; i--);
    strcpy(szukamy,nowa_sciezka+i+1);
    nowa_sciezka[i] ='\0';
    struct dirent * plik;
    DIR * sciezka;
    sciezka = opendir( nowa_sciezka );

    while(( plik = readdir( sciezka ) ) )
    {
        if(strcmp(plik->d_name,szukamy) == 0)
        {
            free(szukamy);
            if((plik->d_type) == DT_DIR)  //    GDY JEST FOLDEREM
            {
                return 0;
            }
            else
            {
                int czas1 = (int)pobierz_czas(nazwa_sciezki),czas2 = (int)pobierz_czas(dodaj_do_sciezki(nowa_sciezka,plik->d_name));
                if(czas1 == czas2)
                {
                    return 0;
                }
                else
                {
                    return 1;
                }
            }
        }
        else
        {
            wynik = 1;
        }
    }
    closedir(sciezka);
    return wynik;
}


void Usuwanie(char * nazwa_sciezki_folder2,char* sciezka_folderu1, char* sciezka_folderu2, bool CzyR)
{
    struct dirent * plik;
    DIR * sciezka, * pom;
    sciezka = opendir( nazwa_sciezki_folder2 );
    while(( plik = readdir( sciezka ) ) )
    {
        if((plik->d_type) == DT_DIR)  //    GDY JEST FOLDEREM
        {
            if(CzyR)
            {
                if( !( strcmp( plik->d_name, "." ) == 0 || strcmp( plik->d_name, "..") == 0 ) )
                {
                    char *nowa_sciezka = dodaj_do_sciezki(nazwa_sciezki_folder2,plik->d_name);
                    Usuwanie(nowa_sciezka,sciezka_folderu1,sciezka_folderu2,CzyR);
                    if(!(pom = opendir(podmien_folder2(nowa_sciezka,sciezka_folderu1, sciezka_folderu2))))
                    {
                        syslog(LOG_INFO, "Usunieto katalog %s", nowa_sciezka);
                        remove(nowa_sciezka);
                    }
                    else
                    {
                        closedir(pom);
                    }
                }
            }
        }
        else
        {
            char *nowa_sciezka = dodaj_do_sciezki(nazwa_sciezki_folder2,plik->d_name);
            if(access(podmien_folder2(nowa_sciezka,sciezka_folderu1,sciezka_folderu2),F_OK) ==-1)
            {
                syslog(LOG_INFO, "Usunieto plik %s", nowa_sciezka);
                remove(nowa_sciezka);
            }
        }
    }
    closedir(sciezka);
}

void kopiuj(char *wej, char *wyj)
{
    char bufor[16];
    int plikwej, plikwyj;
    int czytajwej, czytajwyj;
    plikwej = open(wej, O_RDONLY);
    plikwyj = open(wyj, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if(plikwej == -1 || plikwyj == -1)
    {
        syslog(LOG_ERR, "Blad w otwarciu pliku!");
        exit(EXIT_FAILURE);
    }

    while((czytajwej = read(plikwej, bufor, sizeof(bufor)))>0)
    {
        czytajwyj = write(plikwyj, bufor, (ssize_t) czytajwej);
        if(czytajwyj != czytajwej)
        {
            perror("BLAD");
            exit(EXIT_FAILURE);
        }
    }
    close(plikwej);
    close(plikwyj);
    zmien_parametry(wej, wyj);
    syslog(LOG_INFO, "Skopiowano plik %s", wej);
}

void PrzegladanieFolderu(char * nazwa_sciezki1, char* sciezka_folderu1, char* sciezka_folderu2, bool CzyR,int Wielkosc_pliku)
{
    printf("JESTESMY W : %s\n",nazwa_sciezki1);
    struct dirent * plik;
    DIR * sciezka,*pom;
    sciezka = opendir( nazwa_sciezki1 );
    char* nowa_sciezka;
    while(( plik = readdir( sciezka ) ) )
    {
        printf("%s  \n",plik->d_name);
        if((plik->d_type) == DT_DIR)  //    GDY JEST FOLDEREM
        {
            if(CzyR)
            {
                if(!(strcmp( plik->d_name, "." ) == 0 || strcmp( plik->d_name, "..") == 0))
                {
                    char * sciekza_do_folderu = podmien_folder1(dodaj_do_sciezki(nazwa_sciezki1,plik->d_name),sciezka_folderu1,sciezka_folderu2);
                    if(!(pom = opendir(sciekza_do_folderu)))
                    {
                        syslog(LOG_INFO, "Stworzono folder %s", sciekza_do_folderu);
                        mkdir(sciekza_do_folderu,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                    }
                    else
                    {
                        closedir(pom);
                    }
                    nowa_sciezka = dodaj_do_sciezki(nazwa_sciezki1,plik->d_name);
                    PrzegladanieFolderu(nowa_sciezka,sciezka_folderu1,sciezka_folderu2,CzyR,Wielkosc_pliku);

                }
            }
        }
        else if((plik->d_type) == DT_REG)// GDY nie jest folderem
        {
            nowa_sciezka = dodaj_do_sciezki(nazwa_sciezki1,plik->d_name);
            int i;
            if((i = sprawdzanie(nowa_sciezka,sciezka_folderu1,sciezka_folderu2)) == 1)
            {
                    kopiuj(nowa_sciezka,podmien_folder1(nowa_sciezka,sciezka_folderu1,sciezka_folderu2));
            }
        }
    }
    closedir(sciezka);

}
void Logowanie(int sig)
{
    syslog(LOG_INFO, "Wybudzenie demona przez sygnal SIGUSR1");
}
