/**
 * @file gtk_update.cpp
 * @author João Santos, 76912, MIEM-UA (santos.martins.joao@ua.pt)
 * @brief this file has the functions that are requested to update info in the GTK user interface
 * @version 0.1
 * @date 2019-01-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */

float maxPan = 3000; /** starting maximum value of Pan */
float minPan = -3000; /** starting minimun value of Pan */
float maxTilt = 500; /** starting maximum value of Tilt */
float minTilt = -500; /** starting minimun value of Tilt */

#include "pari_bbl.h"

/**
 * @brief Updates everything related to the RS232's received data (message, values, etc.).
 * 
 * If a message relative to the Current or Maximun positions is received, the function not only updated the text, but also the
 * variables values.
 * 
 * This functions also updates what is shown in the UI, more specifically the bars and the maximum values.
 * 
 * The code that makes the text box automatically scroll to the bottom was based on https://mail.gnome.org/archives/gtk-app-devel-list/2003-June/msg00030.html
 * 
 * @param CaixaDeTexto 
 * @return gboolean 
 */
gboolean UpdateUIMsg(GtkTextView *CaixaDeTexto)
{

    //GObject *CaixaDeTexto = gtk_builder_get_object(builderG, "ReceivedText");
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(CaixaDeTexto);
    GtkProgressBar *PanBar = GTK_PROGRESS_BAR(gtk_builder_get_object(builderG, "ProgBar_Pan"));
    GtkProgressBar *TiltBar = GTK_PROGRESS_BAR(gtk_builder_get_object(builderG, "ProgBar_Tilt"));
    GtkLabel *MaxPan_Lbl = GTK_LABEL(gtk_builder_get_object(builderG, "MaxPan_Lbl"));
    GtkLabel *MaxTilt_Lbl = GTK_LABEL(gtk_builder_get_object(builderG, "MaxTilt_Lbl"));
    GtkLabel *MinPan_Lbl = GTK_LABEL(gtk_builder_get_object(builderG, "MinPan_Lbl"));
    GtkLabel *MinTilt_Lbl = GTK_LABEL(gtk_builder_get_object(builderG, "MinTilt_Lbl"));

    int shrm_rsmsg = GetSharedMem(com_size, path_shrm_RSmsg);

    char *msg = (char *)shmat(shrm_rsmsg, (void *)0, 0);

    const char *terminus = "deadEnd\n\0";

    /**
    * @brief definitions of strings
    * 
    */
    const char PanCall[20] = "Current Pan";
    const char TiltCall[20] = "Current Tilt";
    const char PanMax[20] = "Maximum Pan";
    const char TiltMax[20] = "Maximum Tilt";
    const char PanMin[20] = "Minimum Pan";
    const char TiltMin[20] = "Minimum Tilt";

    const char *PanStrStart;
    const char *TiltStrStart;
    const char *MaxPanStr;
    const char *MaxTiltStr;
    const char *MinPanStr;
    const char *MinTiltStr;

    float PP, PT;
    float percPP, percPT;

    if (strcmp(msg, terminus) && msg != (char *)-1)
    {

        /**
         * @brief gets the pointer for the requested phrase
         * 
         */
        PanStrStart = strstr(msg, PanCall);
        TiltStrStart = strstr(msg, TiltCall);
        MaxPanStr = strstr(msg, PanMax);
        MaxTiltStr = strstr(msg, TiltMax);
        MinPanStr = strstr(msg, PanMin);
        MinTiltStr = strstr(msg, TiltMin);

        if (MaxPanStr)
        {
            sscanf(MaxPanStr, "Maximum Pan position is %f", &maxPan);
            gtk_label_set_text(MaxPan_Lbl, MaxPanStr);
        }

        if (MinPanStr)
        {
            sscanf(MinPanStr, "Minimum Pan position is %f", &minPan);
            gtk_label_set_text(MinPan_Lbl, MinPanStr);
        }

        if (MaxTiltStr)
        {
            sscanf(MaxTiltStr, "Maximum Tilt position is %f", &maxTilt);
            gtk_label_set_text(MaxTilt_Lbl, MaxTiltStr);
        }

        if (MinTiltStr)
        {
            sscanf(MinTiltStr, "Minimum Tilt position is %f", &minTilt);
            gtk_label_set_text(MinTilt_Lbl, MinTiltStr);
        }

        if (PanStrStart)
        {

            sscanf(PanStrStart, "Current Pan position is %f", &PP);

            if (PP >= 0 && maxPan != 0)
            {
                percPP = abs(PP) / abs(maxPan);
            }
            else if (PP < 0 && minPan != 0)
            {
                percPP = abs(PP) / abs(minPan);
            }
            else
            {
                percPP = 0;
            }

            gtk_progress_bar_set_fraction(PanBar, percPP);
        }

        if (TiltStrStart)
        {

            sscanf(TiltStrStart, "Current Tilt position is %f", &PT);

            if (PT >= 0 && maxTilt != 0)
            {
                percPT = abs(PT) / abs(maxTilt);
            }
            else if (PT < 0 && minTilt != 0)
            {
                percPT = abs(PT) / abs(minTilt);
            }
            else
            {
                percPT = 0;
            }

            gtk_progress_bar_set_fraction(TiltBar, percPT);
        }

        gtk_text_buffer_insert_at_cursor(buffer, msg, -1); //<* update the text box
        fflush(stdout);

        strcpy(msg, terminus);

        /**
         * @brief automatically scrolls the text box to the bottom
         * 
         */
        GtkTextMark *mark = NULL;
        GtkTextIter iter_start, iter_end;

        gtk_text_buffer_get_bounds(buffer, &iter_start, &iter_end);

        mark = gtk_text_buffer_create_mark(buffer, NULL, &iter_end, FALSE);
        gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(CaixaDeTexto), mark);
        gtk_text_buffer_delete_mark(buffer, mark);
    }

    shmdt(msg);

    return TRUE;
}

/**
 * @brief This function read a frame (AKA cv::Mat) from a shared memory, converts it to a pixbuf and updates the UI with it. 
 * 
 * It cretas a copy of the original pixbuf to workaround a memory management problem. In the end it frees both pixbufs memories so there are 
 * no memory leaks
 * (based on: https://nofurtherquestions.wordpress.com/2017/03/03/python-3-5-gtk-3-glade-and-opencv/ ,
 * https://stackoverflow.com/questions/42620357/how-to-write-opencv-cvmat-image-directly-in-boost-shared-memory ,
 * and https://github.com/iamsurya/minimal-working-examples/tree/master/OpenCV-SharedMemory)
 * 
 * @param image_pos 
 * @return gboolean 
 */
gboolean UpdateUIFrame_FromSharedMemory(GtkImage *image_pos)
{

    using namespace cv;

    int width = size.width;
    int height = size.height;
    int channels = 3;

    int mem_dim = width * height * channels;
    int shrm_frm = GetSharedMem(mem_dim, path_shrm_Frame);

    unsigned char *buf = (unsigned char *)shmat(shrm_frm, (void *)0, 0);

    cv::Mat received = cv::Mat(size, CV_8UC3, buf, channels * width);

    GdkPixbuf *pix, *pixCopy;

    pix = gdk_pixbuf_new_from_data((guchar *)received.data,
                                   GDK_COLORSPACE_RGB,
                                   FALSE,
                                   8,
                                   received.cols,
                                   received.rows,
                                   received.step,
                                   NULL, //callback to free allocated memory: void (*GdkPixbufDestroyNotify) (guchar *pixels, gpointer fn_data);
                                   NULL  //argument to previous callback: gpointer fn_data
    );

    pixCopy = gdk_pixbuf_copy(pix);

    gtk_image_set_from_pixbuf(image_pos, pixCopy);

    g_object_unref(pix);
    g_object_unref(pixCopy);
    shmdt(buf);

    return TRUE;
}

/**
 * @brief This function returns the correct frame that the user requested.
 * 
 * @param ColorFrame 
 * @param ColorMap 
 * @return cv::Mat 
 */
cv::Mat RetrieveFrame(int ColorFrame, int ColorMap)
{
    using namespace cv;

    Mat frame;

    if (ColorFrame == 1)
    {
        frame = UpdateNIFrame_Color();
    }

    else
    {
        frame = UpdateNIFrame_Depth(ColorMap);
    }

    if (frame.size() == cv::Size(10, 10))
    {
        cv::Mat Nula(size, CV_8UC3, cv::Scalar(144, 144, 144));

        return Nula;
    }

    return frame;
}

/**
 * @brief Updates the frame that must be displayed and the correspondig label (ColorMap descriptons: https://docs.opencv.org/3.1.0/d3/d50/group__imgproc__colormap.html).
 * 
 * @param SliderVal 
 * @param user_data 
 * @return gboolean 
 */
gboolean UpdateFrameValues(int SliderVal, gpointer user_data)
{
    int ColorFrame, ColorMap;
    const char *TextToColorLabel[14] = {"RGB Image\n", "Depth Image\nColorMap: Autumn", "Depth Image\nColorMap: Bone", "Depth Image\nColorMap: Jet", "Depth Image\nColorMap: Winter", "Depth Image\nColorMap: Rainbow", "Depth Image\nColorMap: Ocean", "Depth Image\nColorMap: Summer", "Depth Image\nColorMap: Spring", "Depth Image\nColorMap: Cool", "Depth Image\nColorMap: HSV", "Depth Image\nColorMap: Pink", "Depth Image\nColorMap: Hot", "Depth Image\nColorMap: Parula"};
    //char *toSend;

    if (SliderVal == 0)
    {
        ColorFrame = 1;
        ColorMap = 0;
    }

    else
    {
        ColorFrame = 0;
        ColorMap = SliderVal - 1;
    }

    int shrm_frameType = GetSharedMem(128, path_shrm_typeFrame);
    char *theTypes = (char *)shmat(shrm_frameType, (void *)0, 0);

    //strcpy(theTypes,toSend);
    sprintf(theTypes, "%d_%d", ColorFrame, ColorMap);

    shmdt(theTypes);

    gtk_label_set_text((GtkLabel *)user_data, TextToColorLabel[SliderVal]);

    return TRUE;
}