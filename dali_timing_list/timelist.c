
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TIMING_LIST_SIZE 120
#define OVERFLOW_VAL 65536

enum
{
    TYPE_NOTHING,                 // Nothing to do
    TYPE_PING,                    // Ask for a reply in order to test the communication
    TYPE_DALI_COMMAND,            // Send command on the DALI bus
    TYPE_STATE_QUERY,             // [DEBUG] Ask the DALI state machine what state it's in
    TYPE_DALI_COMMAND_SEQUENCE,   // Send group of commands on the DALI bus
    TYPE_PING_DELAY,              // Ask the device for a delayed reply (for accurate PC application timing)
    TYPE_SEND_DATA_TIMING_LIST,   // Send arbitrary waveform on the DALI bus
    TYPE_RESET_TIMING_LIST,       // Prepare to load arbitrary waveform data
    TYPE_LOAD_DATA_TIMING_LIST,   // Load arbitrary waveform data
    TYPE_SET_RECEIVE_TIMING_LIST, // Enable/Disable waveform capture vs. decoded protocol
    TYPE_READ_FLAGS,              // [DEBUG] Read DALI state machine flags
    TYPE_ENABLE_DEBUG_PACKETS,    // Enable/Disable the reception of debug data (state transitions)
    TYPE_GET_FW_VERSION           // Request firmware version
};

int main(int argc, char **argv)
{
    // Specify the device file
    const char *deviceFile = "/dev/ttyRPMSG0";
    char *timingListFile = "./timingList.txt";
    char lineBuffer[256];
    long timeArray[TIMING_LIST_SIZE] = {0xFFFF};
    int counter = 0;
    unsigned char txBuffer[256] = {[0 ... 255] = 0xFF};

    printf("argc: %d, %s, %s\r\n", argc, argv[0], argv[1]);

    if (argc < 2)
    {
        printf("usage: send <cmd id> [filename]\r\n");
        printf("0 - TYPE_NOTHING \r\n");
        printf("1 - TYPE_PING \r\n");
        printf("2 - TYPE_DALI_COMMAND \r\n");
        printf("3 - TYPE_STATE_QUERY \r\n");
        printf("4 - TYPE_DALI_COMMAND_SEQUENCE \r\n");
        printf("5 - TYPE_PING_DELAY \r\n");
        printf("6 - TYPE_SEND_DATA_TIMING_LIST \r\n");
        printf("7 - TYPE_RESET_TIMING_LIST \r\n");
        printf("8 - TYPE_LOAD_DATA_TIMING_LIST [filename]\r\n");
        printf("9 - TYPE_SET_RECEIVE_TIMING_LIST \r\n");
        printf("10 - TYPE_READ_FLAGS \r\n");
        printf("11 - TYPE_ENABLE_DEBUG_PACKETS \r\n");
        printf("12 - TYPE_GET_FW_VERSION \r\n");
        return 1;
    }

    int cmd = atoi(argv[1]);
    txBuffer[0] = (char)cmd;

    switch (cmd)
    {
    case 8:
        if (argc == 2)
        {
            printf("please select a file to load timing list, example: ./send.out 8 ./test.txt\r\n");
            return 1;
        }
        else
        {
            timingListFile = argv[2];
            FILE *file = fopen(timingListFile, "r");
            if (file == NULL)
            {
                printf("Error opening timing file: %s\r\n", timingListFile);
                return 1;
            }

            // read time list file
            if (fgets(lineBuffer, sizeof(lineBuffer), file) != NULL)
            {
                printf("Timing list fist line: %s\r\n", lineBuffer);
            }

            const char delimiter[] = ";";
            char *token;
            token = strtok(lineBuffer, delimiter);
            while (token != NULL)
            {
                // printf("token: %s, ", token);
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
                    if (counter < TIMING_LIST_SIZE)
                    {
                        // printf("Converted integer: %ld\n", result);
                        timeArray[counter] = OVERFLOW_VAL - result * 3; // 16 bit TE timer running at 3MHz,
                        txBuffer[counter * 2 + 1] = (unsigned char)(timeArray[counter] >> 8);
                        txBuffer[counter * 2 + 2] = (unsigned char)(timeArray[counter]);
                        counter++;
                    }
                    else
                    {
                        printf("number more than MAX_SIZE(%d)\r\n", TIMING_LIST_SIZE);
                    }

                    // Print the result
                }

                token = strtok(NULL, delimiter);
            }

            // Char buffer to be sent

            printf("[ ");
            for (int i = 0; i < counter; i++)
            {
                printf("%d ", timeArray[i]);
            }

            printf("]\r\n");
        }
        break;
    default:
        break;
    }

    // Open the device file for writing
    int ttyRPMSG0 = open(deviceFile, O_WRONLY);

    // Check for errors in opening the file
    if (ttyRPMSG0 == -1)
    {
        perror("Error opening ttyRPMSG0");
        return 1;
    }

    printf("\r\ncounter: %d, txBuffer size: %d\r\n", counter, (1 + counter * 2));
    for (int i = 0; i < 3 + counter * 2; i++)
    {
        printf("%d ", txBuffer[i]);
    }
    printf("\r\n");
    // Write the buffer to the device file
    ssize_t bytesWritten = write(ttyRPMSG0, txBuffer, (3 + counter * 2)); // first byte, + 2 * 0xFF stop bytes

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