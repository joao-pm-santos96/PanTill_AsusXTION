/**
 * @file comms.cpp
 * @author João Santos, 76912, MIEM-UA (santos.martins.joao@ua.pt)
 * @brief this file has all the functions related to communications (RS232, Shared Memories)
 * @version 0.1
 * @date 2019-01-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "pari_bbl.h"

/**
 * @brief Get the Shared Mem object. Based on a file path creates a key and then gets the id of the memory associated to that key.
 * 
 * @param memSize 
 * @param file_path 
 * @return int 
 */
int GetSharedMem(size_t memSize, const char *file_path)
{
    key_t key; /*it's a long int */
    int shm_id;
    key = ftok(file_path, 'X'); /* generate a random key based on a file name and char */
    if (key == -1)
    {
        perror("ftok");
        return -1;
    }

    /* now connect to (or possibly create) the shared memory segment with permission 644 */
    //shm_id = shmget(key, memSize, 0644 | IPC_CREAT);
    shm_id = shmget(key, memSize, 0666 | IPC_CREAT);
    if (shm_id == -1)
    {
        perror("shmget");
        return -1;
    }

    return shm_id;
}

/**
 * @brief Opens the communication port (RS232) with the correct parameters:
 * 
 * BaudRate 9600 bit/s;
 * 
 * Parity: No;
 * 
 * Stop bit: 1;
 * 
 * @param port_name 
 * @return int 
 */
int OpenPort(const char *port_name)
{
    int fd;
    fd = open(port_name, O_RDWR | O_NONBLOCK);

    if (fd == -1)
    {
        printf("Error. Could not open port\n");
        return -1;
    }

    //Settings
    struct termios settings;
    tcgetattr(fd, &settings);

    cfsetospeed(&settings, B9600); /* baud rate */
    settings.c_cflag &= ~PARENB;   /* no parity */
    settings.c_cflag &= ~CSTOPB;   /* 1 stop bit */
    settings.c_cflag &= ~CSIZE;
    settings.c_cflag |= CS8 | CLOCAL; /* 8 bits */
    settings.c_lflag = ICANON;        /* canonical mode */
    settings.c_oflag &= ~OPOST;       /* raw output */

    tcsetattr(fd, TCSANOW, &settings); /* apply the settings */
    tcflush(fd, TCOFLUSH);

    return fd;
}

/**
 * @brief Writes message to port.
 * 
 * @param fd 
 * @param text 
 * @return ssize_t 
 */
ssize_t WritePort(int fd, char *text)
{
    ssize_t size = write(fd, text, strlen(text));
    return size;
}

/**
 * @brief Reads the message from port, a charater at a time until the end character is read.
 * 
 * After reading it appends the NULL char to the end of the string and writes it to the shared memory responsible for the communication from the PTU to the UI.
 * 
 * @param fd 
 * @return int 
 */
int ReadPort(int fd)
{
    char ch, n;
    char string[100];
    int x = 0;

    do
    {
        n = read(fd, &ch, 1);
        if (n == -1)
        {
            return -1;
            break;
        }

        if (n > 0)
        {
            //printf("%c", ch);
            string[x] = ch;
            fflush(stdout);
            x++;
        }

    } while (ch != '\n'); //<* end character

    string[x] = '\0';

    //printf("%s", string);

    int shrm_rsmsg = GetSharedMem(1024, path_shrm_RSmsg);

    char *msg = (char *)shmat(shrm_rsmsg, (void *)0, 0);
    strcpy(msg, string);
    shmdt(msg);

    return 1;
}


/**
 * @brief Changes a variable value (inside a shared memory) so that the infinite loop within all the child processes and the processes themselves can stop.
 * 
 */
void KillTheChild()
{
    int shrm_killChild = GetSharedMem(128, path_shrm_killChild);

    char *toKill = (char *)shmat(shrm_killChild, (void *)0, 0);
    strcpy(toKill, "killitNOW");
    shmdt(toKill);
}