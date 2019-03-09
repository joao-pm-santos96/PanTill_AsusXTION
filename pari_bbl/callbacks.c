/**
 * @file callbacks.c
 * @author João Santos, 76912 (santos.martins.joao@ua.pt)
 * @brief file with all callbacks for the UI
 * @version 0.1
 * @date 2018-12-27
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include "pari_bbl.h"

/**
 * @brief callback called when the user increses the pan value, with the UI button
 * 
 * @param widget 
 * @param event 
 * @param user_data 
 * @return gboolean 
 */
gboolean PanPlus_Clicked(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    int orient = 1;
    char axis[5] = "pan";

    PanTilt_Rel(axis, orient);

    return TRUE;
}

/**
 * @brief callback called when the user decreses the pan value, with the UI button
 * 
 * @param widget 
 * @param event 
 * @param user_data 
 * @return gboolean 
 */
gboolean PanMinus_Clicked(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    int orient = -1;
    char axis[5] = "pan";

    PanTilt_Rel(axis, orient);

    //g_print("%s", msg);

    return TRUE;
}

/**
 * @brief callback called when the user increses the tilt value, with the UI button
 * 
 * @param widget 
 * @param event 
 * @param user_data 
 * @return gboolean 
 */
gboolean TiltPlus_Clicked(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    int orient = 1;
    char axis[5] = "tilt";

    PanTilt_Rel(axis, orient);

    //g_print("%s", msg);

    return TRUE;
}

/**
 * @brief callback called when the user decreses the tilt value, with the UI button
 * 
 * @param widget 
 * @param event 
 * @param user_data 
 * @return gboolean 
 */
gboolean TiltMinus_Clicked(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    int orient = -1;
    char axis[5] = "tilt";

    PanTilt_Rel(axis, orient);

    //g_print("%s", msg);

    return TRUE;
}

/**
 * @brief callback to quit the UI and Communications process
 * 
 */
void on_window_main_destroy()
{
    KillTheChild();
    gtk_main_quit();
}

/**
 * @brief callback to change the absolute value of pan and tilt
 * 
 * @param button 
 * @return gboolean 
 */
gboolean AbsoluteMove(GtkButton *button)
{
    GtkEntry *PanValue = GTK_ENTRY(gtk_builder_get_object(builderG, "PanBox"));
    GtkEntry *TiltValue = GTK_ENTRY(gtk_builder_get_object(builderG, "TiltBox"));

    const char *ValorDePan = gtk_entry_get_text(PanValue);
    const char *ValorDeTilt = gtk_entry_get_text(TiltValue);

    PanTilt_Abs(ValorDePan, ValorDeTilt);

    return TRUE;
}

/**
 * @brief callback to change the absolute value of speed
 * 
 * @param button 
 * @return gboolean 
 */
gboolean SpeedChange(GtkComboBox *widget, gpointer user_data)
{

    GtkEntry *SpeedPercEntry = (GtkEntry *)user_data;
    const char *SpeedPerc = gtk_entry_get_text(SpeedPercEntry);

    PanTilt_ChangeSpeed((char *)SpeedPerc);

    //g_print("%s", msg);

    return TRUE;
}

/**
 * @brief callback to send a message to the zero position
 * 
 * @param button 
 */
void ZeroPosition(GtkButton *button)
{
    const char *PanVal = "0";
    const char *TiltVal = "0";

    PanTilt_Abs(PanVal, TiltVal);
}

/**
 * @brief callback to send a message to the pan 500 tilt 200 position
 * 
 * @param button 
 */
void Pan500Tilt200(GtkButton *button)
{
    const char *PanVal = "500";
    const char *TiltVal = "200";

    PanTilt_Abs(PanVal, TiltVal);
}

/**
 * @brief callback to send a message to the pan 200 tilt 500 position
 * 
 * @param button 
 */
void Pan200Tilt500(GtkButton *button)
{
    const char *PanVal = "200";
    const char *TiltVal = "500";

    PanTilt_Abs(PanVal, TiltVal);
}

/**
 * @brief callback to send a message to the pan 500 tilt 500 position
 * 
 * @param button 
 */
void Pan500Tilt500(GtkButton *button)
{
    const char *PanVal = "500";
    const char *TiltVal = "500";

    PanTilt_Abs(PanVal, TiltVal);
}

/**
 * @brief callback to send a message to reset the unit
 * 
 * @param button 
 */
void ResetButton(GtkButton *button)
{
    ResetPTU();
}

/**
 * @brief callback to send a message to halt the unit
 * 
 * @param button 
 */
void HaltButton(GtkButton *button)
{
    HaltPTU();
}

/**
 * @brief callback to send a message to ask the unit positions
 * 
 * @param button 
 */
void AskButton(GtkButton *button)
{
    AskPTU();
}

/**
 * @brief callback to change if the user sees the color or depth frame and corresponding ColorMap
 * 
 * @param range 
 * @param user_data 
 */
void FrameTypeChoosen(GtkRange *range, gpointer user_data)
{
    //ColorFrame = !ColorFrame;
    gdouble val = gtk_range_get_value(range);
    int slider = (int)val;

    UpdateFrameValues(slider, user_data);
}

/**
 * @brief callback called when the UI receives a signal of camera not found
 * 
 * @param signum 
 */
void CameraNotFound(int signum)
{
    GtkImage *CamImg = GTK_IMAGE(gtk_builder_get_object(builderG, "CameraStatus"));
    const gchar *icon = "gtk-cancel";

    gtk_image_set_from_icon_name(CamImg, icon, GTK_ICON_SIZE_BUTTON);
}

/**
 * @brief callback called when the UI receives a signal of PTU not found
 * 
 * @param signum 
 */
void PTUNotFound(int signum)
{
    GtkImage *CamImg = GTK_IMAGE(gtk_builder_get_object(builderG, "PTUStatus"));
    const gchar *icon = "gtk-cancel";

    gtk_image_set_from_icon_name(CamImg, icon, GTK_ICON_SIZE_BUTTON);
}

/**
 * @brief callback called when the process responsible for the camera communication receives a signal to end itself
 * 
 * @param signum 
 */
void EndCycle(int signum)
{
    CloseOpenNI();
    exit(0);
}
