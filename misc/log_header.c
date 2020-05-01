#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define TS_BUF_LENGTH 32

#define LOG_COUNT       0x01 // 0b00000001
#define LOG_DATE        0x02 // 0b00000010
#define LOG_TIME        0x04 // 0b00000100
#define LOG_USER        0x08 // 0b00001000
#define LOG_DATETIME    0x06 // 0b00000110
#define LOG_ALL         0xFF // 0b11111111

// NOTE: pass variadic arguments. Or not.
// NOTE:? Should I create log message for every protocol in order to make the code more readable.
void log_header(FILE *fp, char *message, uint8_t options) {
    static uint64_t log_count = 0;

    time_t time_val;
    char timestamp[TS_BUF_LENGTH];
    char datestamp[TS_BUF_LENGTH];
    struct tm *tm_info;

    time_val = time(NULL);
    tm_info = localtime(&time_val);

    strftime(datestamp, TS_BUF_LENGTH, "%F (%a)", tm_info);
    strftime(timestamp, TS_BUF_LENGTH, "%H:%M:%S", tm_info);

    if (LOG_COUNT & options) {
        fprintf(fp, "%lli: ", ++log_count);
    }
    if (LOG_DATE & options) {
        fprintf(fp, "[%s, ", datestamp);
    }
    if (LOG_TIME & options) {
        fprintf(fp, "%s] ", timestamp);        
    }
    if (LOG_USER & options) {
        fprintf(fp, "%s - ", getuserlogin());        
    }
}