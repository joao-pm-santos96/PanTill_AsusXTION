/*File generated automatically in joao-X750JN by root on qua jan 30 20:33:41 WET 2019*/
#ifdef __cplusplus
extern "C" {
#endif
/* callbacks.c */
gboolean AbsoluteMove (GtkButton * button);
void AskButton (GtkButton * button);
void CameraNotFound (int signum);
void EndCycle (int signum);
void FrameTypeChoosen (GtkRange * range, gpointer user_data);
void HaltButton (GtkButton * button);
void PTUNotFound (int signum);
void Pan200Tilt500 (GtkButton * button);
void Pan500Tilt200 (GtkButton * button);
void Pan500Tilt500 (GtkButton * button);
gboolean PanMinus_Clicked (GtkWidget * widget, GdkEvent * event, gpointer user_data);
gboolean PanPlus_Clicked (GtkWidget * widget, GdkEvent * event, gpointer user_data);
void ResetButton (GtkButton * button);
gboolean SpeedChange (GtkComboBox * widget, gpointer user_data);
gboolean TiltMinus_Clicked (GtkWidget * widget, GdkEvent * event, gpointer user_data);
gboolean TiltPlus_Clicked (GtkWidget * widget, GdkEvent * event, gpointer user_data);
void ZeroPosition (GtkButton * button);
void on_window_main_destroy ();
#ifdef __cplusplus
}
#endif
/* gtk_update.cpp */
cv::Mat RetrieveFrame (int ColorFrame, int ColorMap);
gboolean UpdateFrameValues (int SliderVal, gpointer user_data);
gboolean UpdateUIFrame_FromSharedMemory (GtkImage * image_pos);
gboolean UpdateUIMsg (GtkTextView * CaixaDeTexto);
/* openni2.cpp */
gboolean CloseOpenNI ();
gboolean InitOpenNI ();
cv::Mat UpdateNIFrame_Color ();
cv::Mat UpdateNIFrame_Depth (int ColorMap);
/* ptu.cpp */
gboolean AskPTU ();
gboolean HaltPTU ();
gboolean PanTilt_Abs (const char *valP, const char *valT);
gboolean PanTilt_ChangeSpeed (char *PercVal);
gboolean PanTilt_Rel (char *type, int orient);
gboolean ResetPTU ();
/* comms.cpp */
int GetSharedMem (size_t memSize, const char *file_path);
void KillTheChild ();
int OpenPort (const char *port_name);
int ReadPort (int fd);
ssize_t WritePort (int fd, char *text);
