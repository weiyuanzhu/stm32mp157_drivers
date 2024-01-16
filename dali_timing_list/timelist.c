
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SIZE 60
#define OVERFLOW_VAL 65535

int main(int argc, char **argv)
{
    // Specify the device file
    const char *deviceFile = "/dev/ttyRPMSG0";
    const char *timingListFile = "./timingList.txt";
    char lineBuffer[256];
    long timeArray[MAX_SIZE] = {0xFFFF};
    int counter = 0;
    unsigned char txBuffer[120] = {0x00};

    // Open the device file for writing
    int ttyRPMSG0 = open(deviceFile, O_WRONLY);
    FILE *timeingListFile = fopen(timingListFile, "r");

    // Check for errors in opening the file
    if (ttyRPMSG0 == -1)
    {
        perror("Error opening ttyRPMSG0");
        return 1;
    }

    if (timeingListFile == NULL)
    {
        perror("Error opening timing file");
        return 1;
    }

    // read time list file
    if (fgets(lineBuffer, sizeof(lineBuffer), timeingListFile) != NULL)
    {
        printf("Timing list fist line: %s\r\n", lineBuffer);
    }

    const char delimiter[] = ";";
    char *token;
    token = strtok(lineBuffer, delimiter);
    while (token != NULL)
    {
        printf("token: %s, ", token);
        char *endptr;
        long result = strtol(token, &endptr, 10);

        // Check for errors
        if (*endptr != '\0')
        {
            printf("Conversion error: Not a valid integer: %s\n", token);
            return 1;
        }
        else
        {
            if (counter < MAX_SIZE)
            {
                printf("Converted integer: %ld\n", result);
                timeArray[counter] = OVERFLOW_VAL - result * 3; // 16 bit TE timer running at 3MHz,
                txBuffer[counter * 2 + 2] = (unsigned char)(timeArray[counter]);
                // printf("counter + 2: %d\n", (unsigned char)(timeArray[counter]));
                txBuffer[counter * 2 + 1] = (unsigned char)(timeArray[counter] >> 8);
                // printf("counter + 1: %d\n", (unsigned char)(timeArray[counter] >> 8));
                counter++;
            }
            else
            {
                printf("number more than MAX_SIZE(%d)\r\n", MAX_SIZE);
            }

            // Print the result
        }

        token = strtok(NULL, delimiter);
    }

    // Char buffer to be sent

    printf("[");
    for (int i = 0; i < counter; i++)
    {
        printf("%d ", timeArray[i]);
    }

    printf("]\r\n");

    txBuffer[0] = 123;
    for (int i = 0; i < 120; i++)
    {
        printf("%d ", txBuffer[i]);
    }

    printf("counter: %d, txBuffer size: %d\r\n", counter, (1 + counter * 2));
    // Write the buffer to the device file
    ssize_t bytesWritten = write(ttyRPMSG0, txBuffer, (1 + counter * 2));

    // Check for errors in writing to the file
    if (bytesWritten == -1)
    {
        perror("Error writing to device file");
        close(ttyRPMSG0);
        return 1;
    }

    // Close the device file
    close(ttyRPMSG0);

    printf("Buffer sent successfully!\n");

    return 0;
}