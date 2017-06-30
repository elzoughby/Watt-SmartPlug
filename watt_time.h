#include <TimeLib.h>        //https://github.com/PaulStoffregen/Time


#define NTP_SERVER "europe.pool.ntp.org"
//                 "us.pool.ntp.org"
//                 "time.nist.gov" 
//                 "time-a.timefreq.bldrdoc.gov"
//                 "time-b.timefreq.bldrdoc.gov"
//                 "time-c.timefreq.bldrdoc.gov"

#define TIME_ZONE 2 // Cairo Time
//                  -5 Eastern Standard Time (USA)
//                  -4 Eastern Daylight Time (USA)
//                  -8 Pacific Standard Time (USA)
//                  -7 Pacific Daylight Time (USA)

#define UDP_PORT 8888 // local port to listen for UDP packets


void time_init();

void time_update();

void digitalClockDisplay();