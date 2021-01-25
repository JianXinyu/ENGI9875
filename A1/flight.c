#include "flight.h"

#include <stdlib.h>
#include <string.h>


struct flight*	flight_create(const char *airline, uint16_t number, struct airport *stops[])
{
    struct flight *flt = malloc(sizeof(struct flight));
    strcpy(flt->f_airline, airline);
// flt->airline[2] = '\0';
    flt->f_number = number;
    flt->f_stop_count = 0;

    memccpy(flt->f_stops, stops, '\0', 1000);
    return flt;
}

void flight_free(struct flight* flt)
{
    if(flt) free(flt);
}

void flight_change_equipment(struct flight* flt, struct aircraft* ac)
{
    flt->f_aircraft = ac;
}

char* flight_code(const struct flight* flt)
{
    char *ret = NULL;
    ret[0] = flt->f_airline[0];
    ret[1] = flt->f_airline[1];
    ret[2] = flt->f_number;
    return ret;
}

