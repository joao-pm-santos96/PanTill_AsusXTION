/**
 * @file main.c
 * @author João Santos, 76912 (santos.martins.joao@ua.pt)
 * @brief The main function of the project: controls the PTU and show the Image, with three processes running
 * @version 0.1
 * @date 2018-12-26
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#define _MAIN_C_

#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "pari_bbl.h"

int main(int argc, char const *argv[])
{
    using namespace std;

    int width = size.width;                  /** image width */
    int height = size.height;                /** image height */
    int channels = 3;                        /** numvber of channels in the image (RGB or BGR) */
    int mem_dim = width * height * channels; /** size of shared memory for each frame of the camera */

    int shrm_killChild = GetSharedMem(128, path_shrm_killChild); /** shared memory to tell the sons to end */
    int shrm_rsmsg = GetSharedMem(com_size, path_shrm_RSmsg);    /** shared memory to store the RS232 message to read */
    int shrm_com = GetSharedMem(com_size, path_shrm_com);        /** shared memory to store the RS232 message to send */
    int shrm_frm = GetSharedMem(mem_dim, path_shrm_Frame);       /** shared memory to store the frame */
    int shrm_frameType = GetSharedMem(128, path_shrm_typeFrame); /** shared memory that defines what to show */

    int pid_pai = getpid();
    int pid_com = fork();

    /**
     * @brief father process, responsible for the interface
     * 
     */
    if (pid_com)
    {

        char *toKill = (char *)shmat(shrm_killChild, (void *)0, 0);
        strcpy(toKill, "nokill");
        shmdt(toKill);

        gtk_init(&argc, (char ***)&argv);

        /** load the interface after a configuration file*/
        builderG = gtk_builder_new();
        gtk_builder_add_from_file(builderG, "UI.glade", NULL);

        /** connect the signals in the interface */
        gtk_builder_connect_signals(builderG, NULL);
        signal(CamSIG, CameraNotFound);
        signal(PTUSIG, PTUNotFound);

        /** get main window Widget ID and connect special signals */
        //GtkWidget *t = GTK_WIDGET(gtk_builder_get_object(builderG, "Window1"));

        /*get pointer to where to show the frames in the user interface */
        GtkImage *ImgGTK = GTK_IMAGE(gtk_builder_get_object(builderG, "Image"));

        /** get pointer to where to write RS232 received message */
        GtkTextView *CaixaDeTexto = GTK_TEXT_VIEW(gtk_builder_get_object(builderG, "ReceivedText"));

        /** puts the functions to update the frame in idle */
        g_idle_add((GSourceFunc)UpdateUIMsg, CaixaDeTexto);
        g_timeout_add(34, (GSourceFunc)UpdateUIFrame_FromSharedMemory, ImgGTK);

        gtk_main();

        /** verifies if the camera process is still going, and if so, sends a signal so it can end its stuff and end */
        int is_cam_alive = kill(pid_com, 0);

        if (is_cam_alive == 0)
        {
            kill(pid_com, SIGUSR1);
        }

        wait(NULL);

        /** only the first process (father of all) can mark the shared memories to be removed */
        shmctl(shrm_com, IPC_RMID, NULL);
        shmctl(shrm_frm, IPC_RMID, NULL);
        shmctl(shrm_frameType, IPC_RMID, NULL);

        shmctl(shrm_killChild, IPC_RMID, NULL);
        shmctl(shrm_rsmsg, IPC_RMID, NULL);
    }

    /**
     * @brief child: responsible for both communications (RS232 and camera)
     * 
     */
    else
    {

        usleep(1e5); /** wait for the fork so, in case a signal is sent, the father process as already attatched the signals to itself */
        int pid_rs = fork();

        /**
         * @brief child->father: reponsible for the camera
         * 
         */
        if (pid_rs)
        {
            using namespace cv;

            /* variables defenitions */
            int ColorFrame, ColorMap;
            unsigned char *buf;
            Mat frame, frameRGB, frameClone;
            gboolean NIstate;

            signal(SIGUSR1, EndCycle);

            /*start OpenNI to retrieve the frames from the Xtion */
            NIstate = InitOpenNI();

            if (!NIstate)
            {
                kill(pid_pai, CamSIG); /** sends signal to change the UI state of the camera */
                //exit(0);
            }

            /** attatch to the shared memories */
            char *theTypes = (char *)shmat(shrm_frameType, (void *)0, 0);
            char *toKill = (char *)shmat(shrm_killChild, (void *)0, 0);

            strcpy(theTypes, "1_0"); /** initialization of the ColorFrame and the ColorMap */

            do
            {

                sscanf(theTypes, "%d_%d", &ColorFrame, &ColorMap);

                frame = RetrieveFrame(ColorFrame, ColorMap);

                flip(frame, frame, 0);
                cvtColor(frame, frameRGB, COLOR_BGR2RGB);

                frameClone = frameRGB.clone(); /** is as to be continues in the memory */

                buf = (unsigned char *)shmat(shrm_frm, (void *)0, 0);

                memcpy(buf, frameClone.ptr(), mem_dim);

            } while ((strcmp(toKill, "nokill") == 0) && NIstate);

            shmdt(buf);

            /*ends the OpenNI session */
            CloseOpenNI();

            exit(0);
        }

        /**
         * @brief child->child: responsible for the RS232
         * 
         */
        else
        {
            int port;
            char *to_do;

            to_do = (char *)shmat(shrm_com, (const void *)0, 0);
            strcpy(to_do, "");

            const char *port_name0 = "/dev/ttyUSB0";
            port = OpenPort(port_name0);

            if (port == -1)
            {
                kill(pid_pai, PTUSIG); /** sends signal to change the UI state of the PTU */
                //exit(0);
            }

            char *toKill = (char *)shmat(shrm_killChild, (void *)0, 0);
            const char *emptyRS = "noMessage\n\0";
            const char *PosRequest = "PP\nTP\n";

            while ((strcmp(toKill, "nokill") == 0) && (port >= 0))
            {

                if (strcmp(to_do, emptyRS) && to_do != (char *)-1)
                {

                    WritePort(port, to_do);

                    strcpy(to_do, emptyRS);

                    WritePort(port, (char *)PosRequest);
                }

                ReadPort(port);

            }

            close(port);

            shmdt(to_do);
            shmdt(toKill);

            exit(0);
        }

        return 1;
    }
}