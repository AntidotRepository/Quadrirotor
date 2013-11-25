#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "rotozoom/SDL_rotozoom.h"
#include <time.h>
#include <SDL/SDL_ttf.h>
#include <pthread.h>
#include <string.h>

#define HAUTEURFENETRE 600
#define LARGEURFENETRE 1000

#define SENSITIVITY 0.5 //Facteur multiplicateur
#define BAUDRATE 115200 //Surement à revoir pour l'adapter à la liaison


//prototypes
int initialization();
void dataReceiving(void *data);
void dataSending(void *data);
void displaying(void *data);

//Globales:
//variables pour la com
int g_comport;
HANDLE g_hComm;
//Format de la trame (peut être à revoir):
/*
╔═══════╦══════╦══════╦════════╦════╗
║ Txxx  ║ Rxxx ║ Lxxx ║ Axxxxx ║ S  ║
╠═══════╬══════╬══════╬════════╬════╣
║tangage║Roulis║Lacet ║Altitude║Stop║
╚═══════╩══════╩══════╩════════╩════╝

Tangage, Roulis, Lacet: angle compris entre 0 et 360
Altitude: Donnée en Cm
Stop; permet de savoir que toute la chaine à été reçue.
*/
char g_bufComRcv[20]= {0};

//variables pour l'envoi (prévoir une initialisation avec des valeurs reçues).
int g_angleTangageReel = 0;
int g_angleTangageSouris = 0;
int g_angleRoulisReel = 0;
int g_angleRoulisSouris = 0;
int g_angleLacetReel = 0;
int g_angleLacetSouris = 0;
int g_altitudeReel = 0;
int g_altitudeClavier = 0;

int g_run = 1;

enum //Step init
{
    ComPort_Open,
    ComPort_Config,
    ComPort_Poll,
    ComPort_Send,
    ComPort_End
};

typedef struct
{
    char key[SDLK_LAST];
    int mouseX, mouseY;
    int mouseXRel, mouseYRel;
    char mouseButtons[8];
    char quit;
} Input;

Input in;
void updateEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            printf("dans le tableau\n");
        case SDL_KEYDOWN:
            in.key[event.key.keysym.sym]=1;
            break;
        case SDL_KEYUP:
            in.key[event.key.keysym.sym]=0;
            break;
        case SDL_MOUSEMOTION:
            in.mouseX=event.motion.x;
            in.mouseY=event.motion.y;
            in.mouseXRel=event.motion.xrel;
            in.mouseYRel=event.motion.yrel;
            break;
        case SDL_MOUSEBUTTONDOWN:
            in.mouseButtons[event.button.button] = 1;
            break;
        case SDL_MOUSEBUTTONUP:
            in.mouseButtons[event.button.button] = 0;
            break;
        case SDL_QUIT:
            in.quit = 1;
            break;
        default:
            break;
        }
    }
}

int main ( int argc, char** argv )
{
    //activation de la console avec la SDL
    freopen("CON","w", stderr);
    freopen("CON","w", stdout);

    pthread_t receivData;
    pthread_t sendData;
    pthread_t display;

    int ret = 0;
//    ret = initialization();

    if(ret == 0)
    {
        //Création de 3 threads qui vont gérer l'envois, la réception et l'affichage:
        ret = pthread_create(&receivData, NULL, (void*)dataReceiving, NULL);
        if(ret)
        {
            printf("Erreur lors de la creation du thread de réception: %d\n", ret);
        }

        ret = pthread_create(&sendData, NULL, (void*)dataSending, NULL);
        if(ret)
        {
            printf("Erreur lors de la creation du thread d'envois: %d\n", ret);
        }

        ret = pthread_create(&display, NULL, (void*)displaying, NULL);
        if(ret)
        {
            printf("Erreur lors de la creation du thread d'affichage: %d\n", ret);
        }

        pthread_join(receivData, NULL);
        pthread_join(sendData, NULL);
        pthread_join(display, NULL);
    }
    return EXIT_SUCCESS;
}

int initialization()
{
    int retour = 0;
    int State = ComPort_Open;
    LPCWSTR port[20];
    char read[50] = {0};
    char tempRead[20] = {0};
    DWORD dwCommEvent;
    char chRead;
    DCB config;
    int i = 0;
    int j = 0;

    memset(&in, 0, sizeof(in)); //Initialisation du tableau pour la gestion du clavier/souris

    printf("Comport: ");
    scanf("%d", &g_comport);
    sprintf((char*)port, "\\\\.\\COM%d", g_comport);

    while(State != ComPort_End)
    {
        switch(State)
        {
        case ComPort_Open:

            g_hComm = CreateFile((LPCSTR)port, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            if(g_hComm == INVALID_HANDLE_VALUE)
            {
                printf("impossible de se co...%d\n", (int)GetLastError());
                CloseHandle(g_hComm);
                return -1;
            }
            else
            {
                printf("Connexion ... Ok\n");
                State = ComPort_Config;
            }
            break;

        case ComPort_Config:
            config.DCBlength = sizeof(DCB);
            if(GetCommState(g_hComm, &config) == 0)
            {
                printf("Erreur lors de la config!");
                return -1;
            }
            else
            {
                config.BaudRate = 57600;             // Current baud
                config.fBinary = TRUE;               // Binary mode; no EOF check
                config.fParity = TRUE;               // Enable parity checking
                config.fOutxCtsFlow = FALSE;         // No CTS output flow control
                config.fOutxDsrFlow = FALSE;         // No DSR output flow control
                config.fDtrControl = DTR_CONTROL_ENABLE;
                // DTR flow control type
                config.fDsrSensitivity = FALSE;      // DSR sensitivity
                config.fTXContinueOnXoff = TRUE;     // XOFF continues Tx
                config.fOutX = FALSE;                // No XON/XOFF out flow control
                config.fInX = FALSE;                 // No XON/XOFF in flow control
                config.fErrorChar = FALSE;           // Disable error replacement
                config.fNull = FALSE;                // Disable null stripping
                config.fRtsControl = RTS_CONTROL_ENABLE;
                // RTS flow control
                config.fAbortOnError = FALSE;        // Do not abort reads/writes on
                // error
                config.ByteSize = 8;                 // Number of bits/byte, 4-8
                config.Parity = NOPARITY;            // 0-4=no,odd,even,mark,space
                config.StopBits = ONESTOPBIT;        // 0,1,2 = 1, 1.5, 2

                if(!SetCommState(g_hComm, &config))
                {
                    printf("erreur de config 2\n");
                    return -1;
                }
                State = ComPort_Poll;
            }
            break;

        case ComPort_Poll:
            GetCommState(g_hComm, &config);
            printf("baudrate = %d\n", config.BaudRate);

            //On attend d'avoir des informations de quadri...
            if(!SetCommMask(g_hComm, EV_RXCHAR))
            {
                printf("F**k!");
            }

            if(WaitCommEvent(g_hComm, &dwCommEvent, NULL))
            {
                if(ReadFile(g_hComm, read, 50, &chRead, NULL))
                {
                    while(read[i]!='T')
                    {
                        i++;
                    }
                    for (j = 0; j<24; j++)
                    {
                        tempRead[j] = read[i+j];
                    }
                    tempRead[19] = '\0';
                    sscanf(tempRead, "T%dR%dL%dA%dS", &g_angleTangageReel, &g_angleRoulisReel, &g_angleLacetReel, &g_altitudeReel);
                }
                else
                {
                    printf("echec!");
                }
            }
            else
            {
                printf("Gros echec!");
            }

            State = ComPort_Send;
            break;

        case ComPort_Send:
            //On lui confirme qu'on a bien reçu les données.
//            while(sendNOK && g_run)
//            {
//                printf("sending ACK...\n");
//                ret = RS232_SendBuf(g_comport, frame_ack, 20);
//                if(ret == 20)
//                {
//                    sendNOK = 0;
//                }
//#warning penser a clearer du cote du quadri...
//            }
            State = ComPort_End;
            break;
        }
    }
    printf("initialization ... Ok\n");
    return retour;
}

void dataReceiving(void *data)
{
    DWORD dwCommEvent;
    char read[50] = {0};
    char chRead;
    char tempRead[20] = {0};
    int i = 0;
    int j = 0;

    while(!in.key[SDLK_ESCAPE] && !in.quit && g_run)
    {
        //On attend d'avoir des informations de quadri...
        if(!SetCommMask(g_hComm, EV_RXCHAR))
        {
            printf("F**k!");
        }
        if(WaitCommEvent(g_hComm, &dwCommEvent, NULL))
        {
            if(ReadFile(g_hComm, read, 50, &chRead, NULL))
            {
                while(read[i]!='T')
                {
                    i++;
                }
                for (j = 0; j<24; j++)
                {
                    tempRead[j] = read[i+j];
                }
                sscanf(tempRead, "T%dR%dL%dA%dS", &g_angleTangageReel, &g_angleRoulisReel, &g_angleLacetReel, &g_altitudeReel);
            }
            else
            {
                printf("echec!");
            }
        }
        else
        {
            printf("Gros echec!");
        }
        i = 0;
    }

//    int ret = 0;
//    while(!in.key[SDLK_ESCAPE]&&!in.quit && g_run)
//    {
////        printf("receiving real values...\n");
////        RS232_PollComport(g_comport, g_bufComRcv, 20);
//        //prise des valeurs reçues comme valeurs de base...
//#warning verifier les calculs
//        sscanf((char*)g_bufComRcv, "T%3dR%3dL%3dA%5dS", (&g_angleTangageReel)-180, (&g_angleRoulisReel)-180, (&g_angleLacetReel)-180, &g_altitudeReel);
//#warning a verifier, faut-il prendre le nombre de caracteres luent(14) ou la chaine complete(20)
//#warning penser a renvoyer regulierement du cote du quadri
//    }
}

void dataSending(void *data)
{
    int sendNOK = 1;
    int ret = 0;

    Uint32 debutBoucle = 0, finBoucle = 0, tempsEcoule = 0;
    Uint16 frameRate = 1000/200;

    OVERLAPPED osWrite = {0};
    DWORD dwWritten;
    DWORD dwRes;
    BOOL fRes;

    //Gros while avec scrutation des touches/souris qui envois
    while(!in.key[SDLK_ESCAPE] && !in.quit && g_run)
    {
        debutBoucle = SDL_GetTicks();

        //envoi des valeurs
        char bufLettre = 0;
        char bufVal[3] = {0};
        char bufAltitudeReel[5] = {0};

        char bufComSnd[25] = {0};

        g_angleTangageSouris -= in.mouseXRel*SENSITIVITY;
        if(g_angleTangageSouris < 0)
        {
            g_angleTangageSouris = 360;
        }
        if(g_angleTangageSouris > 360)
        {
            g_angleTangageSouris = 0;
        }

        g_angleRoulisSouris -= in.mouseYRel*SENSITIVITY;
        if(g_angleRoulisSouris < 0)
        {
            g_angleRoulisSouris = 360;
        }
        if(g_angleRoulisSouris > 360)
        {
            g_angleRoulisSouris = 0;
        }

        in.mouseXRel = 0;
        in.mouseYRel = 0;

        if(in.key[SDLK_w])
        {
            g_altitudeClavier+=1;
        }
        if(in.key[SDLK_s])
        {
            g_altitudeClavier-=1;
        }
        if(in.key[SDLK_a])
        {
            g_angleLacetSouris+=1;
            if(g_angleLacetSouris >= 360)
            {
                g_angleLacetSouris = 0;
            }
        }
        if(in.key[SDLK_d])
        {
            g_angleLacetSouris-=1;
            if(g_angleLacetSouris <= 0)
            {
                g_angleLacetSouris = 360;
            }
        }

        sprintf(bufComSnd, "T%03dR%03dL%03dA%05dS", g_angleTangageSouris%360, g_angleRoulisSouris%360, g_angleLacetSouris%360, g_altitudeClavier);   //Concaténation de la trame à envoyer.
        printf("%s\n", bufComSnd);
//            printf("sending desired values...\n");
//            ret = RS232_SendBuf(g_comport, bufComSnd, 20);
//On attend d'avoir des informations de quadri...
//            if(!SetCommMask(g_hComm, EV_RXCHAR))
//            {
//                printf("F**k!");
//            }

            //Issue write
            if(!WriteFile(g_hComm, bufComSnd, 19, &dwWritten, &osWrite))
            {
                if(GetLastError()!=ERROR_IO_PENDING)
                {
                    //WriteFile failed, but isn't delayed. Report error and abort.
                    fRes = FALSE;
                }
                else
                {
                    //Write is pending
                    dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
                    switch(dwRes)
                    {
                        //OVERLAPPED structure's event has been signaled
                    case WAIT_OBJECT_0:
                        if(!GetOverlappedResult(g_hComm, &osWrite, &dwWritten, FALSE))
                            fRes = FALSE;
                        else
                        {
                            fRes = TRUE;
                        }
                        break;
                    default:
                        fRes = FALSE;
                        break;
                    }
                }
            }
            else
            {
//                printf("envoi ok\n");
                fRes = TRUE;
            }
//            if(WriteFile(g_hComm, bufComSnd, 1, NULL, NULL))
//            {
//                printf("send successful\n");
//            }
//            else
//            {
//                printf("echec!");
//            }
//            if(ret == 20)
//            {
//                sendNOK = 0;
//            }
#warning penser a clearer du cote du quadri...


        finBoucle = SDL_GetTicks();
        tempsEcoule = finBoucle - debutBoucle;
        if(tempsEcoule<frameRate)
        {
            SDL_Delay(frameRate-tempsEcoule);
        }
    }
}

void displaying(void *data)
{
    SDL_Surface *ecran = NULL;
    Uint32 debutBoucle = 0, finBoucle = 0, tempsEcoule = 0;
    Uint16 frameRate = 1000/20;


    SDL_Surface *surfaceBGTangage = IMG_Load("res/TangageBG.png");
    SDL_Surface *surfaceAvTangage = IMG_Load("res/AvTangage.png");
    SDL_Surface *surfaceAvTangageSouris = IMG_Load("res/AvTangageSouris.png");
    SDL_Surface *surfaceBGLacet = IMG_Load("res/BoussoleBG.png");
    SDL_Surface *surfaceAvLacet = IMG_Load("res/AvBoussole.png");
    SDL_Surface *surfaceAvLacetSouris = IMG_Load("res/AvBoussoleSouris.png");
    SDL_Surface *surfaceBGRoulis = IMG_Load("res/RouliBG.png");
    SDL_Surface *surfaceAvRoulis = IMG_Load("res/AvRoulis.png");
    SDL_Surface *surfaceAvRoulisSouris = IMG_Load("res/AvRoulisSouris.png");

    SDL_Rect positionBGTangage;
    SDL_Rect positionAvTangage;
    SDL_Rect positionAvTangageSouris;
    SDL_Rect positionBGRoulis;
    SDL_Rect positionAvRoulis;
    SDL_Rect positionAvRoulisSouris;
    SDL_Rect positionBGLacet;
    SDL_Rect positionAvLacet;
    SDL_Rect positionAvLacetSouris;
    SDL_Rect positionAltitude;
    SDL_Rect positionAltitudeClavier;
    SDL_Rect positionSignal;
    SDL_Rect positionBatterie;

    positionBGTangage.x = 20;
    positionBGTangage.y = 20;

    positionAvTangage.x = 30;
    positionAvTangage.y = 30;

    positionAvTangageSouris.x = 30;
    positionAvTangageSouris.y = 30;

    positionBGRoulis.x = 200;
    positionBGRoulis.y = 20;

    positionAvRoulis.x = 210;
    positionAvRoulis.y = 30;

    positionAvRoulisSouris.x = 210;
    positionAvRoulisSouris.y = 30;

    positionBGLacet.x = 360;
    positionBGLacet.y = 20;

    positionAvLacet.x = 370;
    positionAvLacet.y = 30;

    positionAvLacetSouris.x = 370;
    positionAvLacetSouris.y = 30;

    positionAltitude.x = 550;
    positionAltitude.y = 80;

    positionAltitudeClavier.x = 550;
    positionAltitudeClavier.y = 110;

    positionBatterie.x = 30;
    positionBatterie.y = 90;

    positionSignal.x = 130;
    positionSignal.y = 90;

    // Initialisation de la SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Erreur lors de l'initialisation de la SDL : %s", SDL_GetError());
        SDL_Quit();
        g_run = 0;
    }

    //Initialisation de SDL_ttf
    if(TTF_Init() == -1)
    {
        printf("Erreur lors de l'initialisation de SDL_ttf: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
        g_run = 0;
    }
    // Création de la fenêtre
    ecran = SDL_SetVideoMode(LARGEURFENETRE, HAUTEURFENETRE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

    TTF_Font *Font_g_altitude = NULL;
    Font_g_altitude = TTF_OpenFont("res/DS-DIGI.ttf", 40);
    SDL_Color couleurNoire = {0,0,0};
    SDL_Surface *surfaceAltitude;
    SDL_Surface *surfaceAltitudeClavier;

    SDL_WM_GrabInput(SDL_GRAB_ON);
    SDL_ShowCursor(SDL_DISABLE);

    g_angleTangageSouris = g_angleTangageReel;
    g_angleRoulisSouris = g_angleRoulisReel;
    g_angleLacetSouris = g_angleLacetReel;
    g_altitudeClavier = g_altitudeReel;

    while (!in.key[SDLK_ESCAPE] && !in.quit && g_run)
    {
        debutBoucle = SDL_GetTicks();

        updateEvents();

        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 116, 124, 121));

        SDL_BlitSurface(surfaceBGTangage, NULL, ecran, &positionBGTangage);
        SDL_BlitSurface(surfaceBGRoulis, NULL, ecran, &positionBGRoulis);
        SDL_BlitSurface(surfaceBGLacet, NULL, ecran, &positionBGLacet);

//        --- AvLacet -------------------------------------------------------------------
//        g_angleAvLacet++;
//        if(g_angleAvLacet>=360)
//        {
//            g_angleAvLacet-=360;
//        }
//        SDL_Surface* rotatedAvLacetReel = rotozoomSurface(surfaceAvLacet, g_angleAvLacet, 1.0, 0);
        SDL_Surface* rotatedAvLacetVoulue = rotozoomSurface(surfaceAvLacetSouris, g_angleLacetSouris%360, 1.0, 0);
        positionAvLacetSouris.x = 440-(rotatedAvLacetVoulue->w)/2;
        positionAvLacetSouris.y = 100-(rotatedAvLacetVoulue->w)/2;
        SDL_BlitSurface(rotatedAvLacetVoulue, NULL, ecran, &positionAvLacetSouris);
        SDL_FreeSurface(rotatedAvLacetVoulue);

        SDL_Surface* rotatedAvLacetReel = rotozoomSurface(surfaceAvLacet, g_angleLacetReel%360, 1.0, 0);
        positionAvLacet.x = 440-(rotatedAvLacetReel->w)/2;
        positionAvLacet.y = 100-(rotatedAvLacetReel->w)/2;
        SDL_BlitSurface(rotatedAvLacetReel, NULL, ecran, &positionAvLacet);
        SDL_FreeSurface(rotatedAvLacetReel);


        //----- AvRoulis -----------------------------------------------------------------
//        g_angleAvRoulis++;
//        if(g_angleAvRoulis>=360)
//        {
//            g_angleAvRoulis-=360;
//        }
        SDL_Surface* rotatedAvRoulisVoulue = rotozoomSurface(surfaceAvRoulisSouris, g_angleRoulisSouris%360, 1.0, 0);
        positionAvRoulisSouris.x = 280-(rotatedAvRoulisVoulue->w)/2;
        positionAvRoulisSouris.y = 100-(rotatedAvRoulisVoulue->w)/2;

        SDL_BlitSurface(rotatedAvRoulisVoulue, NULL, ecran, &positionAvRoulisSouris);
        SDL_FreeSurface(rotatedAvRoulisVoulue);

        SDL_Surface* rotatedAvRoulisReel = rotozoomSurface(surfaceAvRoulis, g_angleRoulisReel%360, 1.0, 0);
        positionAvRoulis.x = 280-(rotatedAvRoulisReel->w)/2;
        positionAvRoulis.y = 100-(rotatedAvRoulisReel->w)/2;

        SDL_BlitSurface(rotatedAvRoulisReel, NULL, ecran, &positionAvRoulis);
        SDL_FreeSurface(rotatedAvRoulisReel);
        //----- AvTangage -----------------------------------------------------------------
//        g_angleAvTangage++;
//        if(g_angleAvTangage>=360)
//        {
//            g_angleAvTangage-=360;
//        }

        SDL_Surface *rotatedAvTangageVoulue = rotozoomSurface(surfaceAvTangageSouris, g_angleTangageSouris%360, 1.0, 0);
        positionAvTangageSouris.x = 100-(rotatedAvTangageVoulue->w)/2;
        positionAvTangageSouris.y = 100-(rotatedAvTangageVoulue->w)/2;

        SDL_BlitSurface(rotatedAvTangageVoulue, NULL, ecran, &positionAvTangageSouris);
        SDL_FreeSurface(rotatedAvTangageVoulue);

        SDL_Surface *rotatedAvTangage = rotozoomSurface(surfaceAvTangage, g_angleTangageReel%360, 1.0, 0);

        positionAvTangage.x = 100-(rotatedAvTangage->w)/2;
        positionAvTangage.y = 100-(rotatedAvTangage->w)/2;

        SDL_BlitSurface(rotatedAvTangage, NULL, ecran, &positionAvTangage);
        SDL_FreeSurface(rotatedAvTangage);
        //----- Altitude -------------------------------------------------------------------
        char chaineAltitudeReel[22] = {0};
        char chaineAltitudeClavier[22] = {0};
        char StringAltitudeClavier[18] = {'a', 'l', 't', 'i', 't', 'u', 'd', 'e', ' ', 'v', 'o', 'u', 'l', 'u', 'e', ':', ' '};
        char StringAltitudeReel[18] = {'A', 'l', 't', 'i', 't', 'u', 'd', 'e', ' ', 'r', 'e', 'e', 'l', 'l', 'e', ':', ' '};
//        g_altitudeReel++;
        char bufAltitudeReel[4];
        char bufAltitudeClavier[4];

        strcat(chaineAltitudeReel, StringAltitudeReel);
        sprintf(bufAltitudeReel, "%d", g_altitudeReel);
        strcat(chaineAltitudeReel, bufAltitudeReel);
        surfaceAltitude = TTF_RenderText_Blended(Font_g_altitude, chaineAltitudeReel, couleurNoire);
        SDL_BlitSurface(surfaceAltitude, NULL, ecran, &positionAltitude);

        strcat(chaineAltitudeClavier, StringAltitudeClavier);
        sprintf(bufAltitudeClavier, "%d", g_altitudeClavier);
        strcat(chaineAltitudeClavier, bufAltitudeClavier);
        surfaceAltitudeClavier = TTF_RenderText_Blended(Font_g_altitude, chaineAltitudeClavier, couleurNoire);
        SDL_BlitSurface(surfaceAltitudeClavier, NULL, ecran, &positionAltitudeClavier);

        //----- Charge batterie ------------------------------------------------------------

        //----- Qualité signal -------------------------------------------------------------

        //----------------------------------------------------------------------------------

        SDL_Flip(ecran);
        finBoucle = SDL_GetTicks();
        tempsEcoule = finBoucle - debutBoucle;

        if(tempsEcoule<frameRate)
        {
            SDL_Delay(frameRate-tempsEcoule);
        }
    }

    TTF_CloseFont(Font_g_altitude);
    TTF_Quit();
    SDL_Quit();
}
