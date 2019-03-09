/**
 * @file ptu.cpp
 * @author João Santos, 76912, MIEM-UA (santos.martins.joao@ua.pt)
 * @brief this files contains the functions used to generate the messages to communicate with the PTU
 * @version 0.1
 * @date 2019-01-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "pari_bbl.h"

#define DeltaPan 100
#define DeltaTilt 100

/**
 * @brief This functions converts the absolute values of Pan and Tilt to the right message to be sent to the unit.
 * 
 * PPx -> Goes to the x position in pan;
 * 
 * TPx -> Goes to the x position in titl;
 * 
 * (message specifications from: https://www.cs.cmu.edu/Groups/xavier/doc/lnk2hrdwr.docs/PTU-manual.pdf).
 * 
 * @param valP 
 * @param valT 
 * @return gboolean 
 */
gboolean PanTilt_Abs(const char *valP, const char *valT)
{

    if (strcmp(valP, "") && strcmp(valT, "")) //*< none are empty
    {
        int shrm_com = GetSharedMem(com_size, path_shrm_com);

        char *send = (char *)shmat(shrm_com, (const void *)0, 0);
        char cmd[20];

        strcpy(cmd, "PP");
        strcat(cmd, valP);
        strcat(cmd, "\n");

        strcat(cmd, "TP");
        strcat(cmd, valT);
        strcat(cmd, "\n");

        strcpy(send, cmd);

        shmdt(send);

        return TRUE;
    }

    else if (strcmp(valP, "") && strcmp(valT, "") == 0) //*< tilt value os empty, so its changing pan
    {

        int shrm_com = GetSharedMem(com_size, path_shrm_com);

        char *send = (char *)shmat(shrm_com, (const void *)0, 0);
        char cmd[20];

        strcpy(cmd, "PP");
        strcat(cmd, valP);
        strcat(cmd, "\n");

        strcpy(send, cmd);

        shmdt(send);

        return TRUE;
    }

    else if (strcmp(valP, "") == 0 && strcmp(valT, "")) //*< pan value is empty, so its changing tilt
    {
        int shrm_com = GetSharedMem(com_size, path_shrm_com);

        char *send = (char *)shmat(shrm_com, (const void *)0, 0);
        char cmd[20];

        strcpy(cmd, "TP");
        strcat(cmd, valT);
        strcat(cmd, "\n");

        strcpy(send, cmd);

        shmdt(send);

        return TRUE;
    }

    else
    {
        return FALSE;
    }
}

/**
 * @brief This function creates a message to increment either the Pan or Tilt value.
 * 
 * POx -> increases pan value by x;
 * 
 * TOx -> increses tilt value by x;
 * 
 * (message specifications from: https://www.cs.cmu.edu/Groups/xavier/doc/lnk2hrdwr.docs/PTU-manual.pdf);
 * 
 * @param type 
 * @param orient 
 * @return gboolean 
 */
gboolean PanTilt_Rel(char *type, int orient)
{
    int shrm_com = GetSharedMem(com_size, path_shrm_com);

    char *send = (char *)shmat(shrm_com, (const void *)0, 0);
    char cmd[20];

    if (strcmp(type, "pan") == 0)
    {
        char PanValue[10];
        int val = DeltaPan * orient;
        sprintf(PanValue, "%d", val);

        strcpy(cmd, "PO");
        strcat(cmd, PanValue);
        strcat(cmd, "\n");
    }

    else if (strcmp(type, "tilt") == 0)
    {
        char TiltValue[10];
        int val = DeltaTilt * orient;
        sprintf(TiltValue, "%d", val);

        strcpy(cmd, "TO");
        strcat(cmd, TiltValue);
        strcat(cmd, "\n");
    }

    strcpy(send, cmd);

    shmdt(send);

    return TRUE;
}

/**
 * @brief This functions creates the message to change the speed, as required.
 * 
 * PSx -> changes pan speed to x;
 * 
 * TSx -> changes tilt speed to x;
 * 
 * (message specifications from: https://www.cs.cmu.edu/Groups/xavier/doc/lnk2hrdwr.docs/PTU-manual.pdf).
 * 
 * @param PercVal 
 * @return gboolean 
 */
gboolean PanTilt_ChangeSpeed(char *PercVal)
{
    int shrm_com = GetSharedMem(com_size, path_shrm_com);

    char *send = (char *)shmat(shrm_com, (const void *)0, 0);

    char cmd[20];
    char ApplySpeed[10];
    int maxSpeed = 1000;

    int perc = atoi(PercVal);

    int actualSpeed = maxSpeed * perc / 100;
    sprintf(ApplySpeed, "%d", actualSpeed);

    strcpy(cmd, "PS");
    strcat(cmd, ApplySpeed);
    strcat(cmd, "\nTS");
    strcat(cmd, ApplySpeed);
    strcat(cmd, "\n");

    strcpy(send, cmd);

    shmdt(send);

    return TRUE;
}

/**
 * @brief This functios creates the message that makes the unit do a Reset.
 * 
 * R -> resest the PTU (so it can know its limit values);
 * 
 * A -> waits for the conclusion of the task;
 * 
 * PX -> asks the PTU for its maximum pan position;
 * 
 * TX -> asks the PTU for its maximum tilt position;
 * 
 * (message specifications from: https://www.cs.cmu.edu/Groups/xavier/doc/lnk2hrdwr.docs/PTU-manual.pdf)
 * 
 * @return gboolean 
 */
gboolean ResetPTU()
{
    int shrm_com = GetSharedMem(com_size, path_shrm_com);

    char *send = (char *)shmat(shrm_com, (const void *)0, 0);
    //const char *cmd = "R\n";
    const char *cmd = "R\nA\nPX\nTX\nPN\nTN\n";

    strcpy(send, cmd);

    shmdt(send);

    return TRUE;
}

/**
 * @brief This function is used to create a message that halts the operation that is ocurring in the unit.
 * 
 * H -> halt the ptu;
 * 
 * (message specifications from: https://www.cs.cmu.edu/Groups/xavier/doc/lnk2hrdwr.docs/PTU-manual.pdf);
 * 
 * @return gboolean 
 */
gboolean HaltPTU()
{
    int shrm_com = GetSharedMem(com_size, path_shrm_com);

    char *send = (char *)shmat(shrm_com, (const void *)0, 0);
    const char *cmd = "H\n";

    strcpy(send, cmd);

    shmdt(send);

    return TRUE;
}

/**
 * @brief Function that creates a message to tell the user the current Pan and Tilt values;
 * 
 * PP -> asks the PTU for its current pan position;
 * 
 * TP -> asks the PTU for its current tilt position;
 * 
 * (message specifications from: https://www.cs.cmu.edu/Groups/xavier/doc/lnk2hrdwr.docs/PTU-manual.pdf);
 * 
 * @return gboolean 
 */
gboolean AskPTU()
{
    int shrm_com = GetSharedMem(com_size, path_shrm_com);

    char *send = (char *)shmat(shrm_com, (const void *)0, 0);
    const char *cmd = "A\nPX\nTX\nPN\nTN\nPP\nTP\n";

    strcpy(send, cmd);

    shmdt(send);

    return TRUE;
}