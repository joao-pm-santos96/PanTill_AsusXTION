/**
 * @file myf.h
 * @author João Santos, 76912 (santos.martins.joao@ua.pt)
 * @brief 
 * @version 0.1
 * @date 2018-12-26
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#ifndef _PARIBBL_H
#define _PARIBBL_H

#include <gtk/gtk.h>
#include <opencv2/core/core_c.h>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/highgui/highgui_c.h>

#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>

#include <fcntl.h>
#include <termios.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "opencv2/opencv.hpp"
//#include <openni2/OpenNI.h>
#include "OpenNI.h"

#if defined(_MAIN_C_)
GtkBuilder *builderG;
cv::Size size = cv::Size(640, 480);

#else
extern GtkBuilder *builderG;
extern cv::Size size;

#endif

#define com_size 1024
/**
#define path_shrm_com "../src/main.cpp"               //<*send the messages
#define path_shrm_killChild "../pari_bbl/callbacks.c" //<*changes the string so infinite cycles can end
#define path_shrm_RSmsg "../src/UI.glade"             //<*receives the message from the PTU
#define path_shrm_Frame "../pari_bbl/openni2.cpp"     //<*stores the camera's frame
#define path_shrm_typeFrame "../pari_bbl/comms.cpp"   //<* color vs rgb and what ColorMap to use
*/

#define path_shrm_com "/usr"               //<*send the messages
#define path_shrm_killChild "/boot" //<*changes the string so infinite cycles can end
#define path_shrm_RSmsg "/home"             //<*receives the message from the PTU
#define path_shrm_Frame "/bin"     //<*stores the camera's frame
#define path_shrm_typeFrame "/root"   //<* color vs rgb and what ColorMap to use

#define CamSIG SIGUSR1
#define PTUSIG SIGUSR2

/* prototypes should be within the last lines of header */
#include "prototypes.h"

#endif /* _PARIBBL_H */