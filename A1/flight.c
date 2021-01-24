#include "flight.h"

#include <stdlib.h>
#include <string.h>

enum aircraft_kind {
    /* A fixed-wing aircraft */
    AC_FIXED,

    /* Gyrocopter */
    AC_GYRO,

    /* A rotary-wing aircraft, i.e., a helicopter */
    AC_ROTARY,

    /* A tilt-rotor aircraft, e.g., an Osprey */
    AC_TILT,
};

struct aircraft{
    enum aircraft_kind kind_;
    const char *regcode_;
    unsigned int refcount_;
};



struct flight*	flight_create(const char *airline, uint16_t number,
                                const struct airport *stops[])
{
    struct flight *flt = malloc(sizeof(struct flight));
    strcpy(flt->f_airline, airline);
// flt->airline[2] = '\0';
    flt->f_number = number;
    flt->f_stops = stops;

    return flt;
}

void flight_free(struct flight* flt)
{
    if(flt) free(flt);
}

void flight_change_equipment(struct flight* flt, struct aircraft* ac)
{
    if(ac)
    {
        ac->refcount_--;
    }
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

