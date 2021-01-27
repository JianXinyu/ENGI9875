#include "flight.h"

#include <stdlib.h>
#include <string.h>
#include "aircraft.h"

struct flight*	flight_create(const char *airline, uint16_t number, struct airport *stops[])
{
    struct flight *flt = malloc(sizeof(struct flight));
    strcpy(flt->f_airline, airline);
    flt->f_aircraft = NULL;
    flt->f_number = number;
    flt->f_stop_count = 0;
    while(stops[flt->f_stop_count])
        flt->f_stop_count++;

//    memccpy(flt->f_stops, stops, '\0', 1000);
    flt->f_stops = stops;

    return flt;
}

void flight_free(struct flight* flt)
{
    if(flt)
    {
        if(flt->f_aircraft)
            ac_release(flt->f_aircraft);
        free(flt);
    }
}

void flight_change_equipment(struct flight* flt, struct aircraft* ac)
{
    if(flt->f_aircraft)
        ac_release(flt->f_aircraft);
    flt->f_aircraft = ac;
    if(flt->f_aircraft)
        ac_hold(flt->f_aircraft);
}

char* flight_code(const struct flight* flt)
{
    char *ret = malloc(6 * sizeof(char));
    ret[0] = flt->f_airline[0];
    ret[1] = flt->f_airline[1];
    ret[2] = flt->f_number / 100 + '0';
    ret[3] = flt->f_number / 10 % 10 + '0';
    ret[4] = flt->f_number % 10 + '0';
    ret[6] = '\0';
    return ret;
}

