#include "airport.h"
#include <stdlib.h>
#include <string.h>
struct airport{
    char *ap_code;
    unsigned int refcount_; // how many things refer to this airport
    unsigned int flt_count;
    struct flight *flights[];
};

struct airport*	ap_create(const char *icao_code)
{
    struct airport *ap = malloc(sizeof(struct airport));
//    strcpy(ap->ap_code, icao_code);
//ap->ap_code = NULL;
//ap->ap_code = icao_code;
    ap->ap_code = strdup(icao_code);
    ap->refcount_ = 1;
    ap->flt_count = 0;
    return ap;
}

const char*	ap_code(const struct airport* ap)
{
    return ap->ap_code;
}

int	ap_add_flight(struct airport *ap, struct flight* flt)
{
    int flag = -1;
    //flight is invalid: NULL pointer
    if(!flt) return flag;

    for(int i = 0; i < flt->f_stop_count; i++)
    {
        if(flt->f_stops[i]->ap_code == ap->ap_code)
        {
            //success
            flag = 0;
            ap->flights[ap->flt_count++] = flt;
            break;
        }
    }
    return flag;
}

/**
 * Copy pointers to flights into a caller-provided array.
 *
 * @param   ap      The airport to query
 * @param   fpp     Array of flight pointers to copy flight data into
 * @param   n       Number of elements in the passed-in array. If too small
 *                  to hold all of the flights, this will be set to the minimum
 *                  size required.
 */

void ap_flights(struct airport *ap, struct flight **fpp, size_t *n)
{
    if (*n < ap->flt_count ) *n = ap->flt_count;
    for(int i = 0; i < ap->flt_count; i++)
        fpp[i] = ap->flights[i];
}

void ap_hold(struct airport* ap)
{
    ap->refcount_++;
}

void ap_release(struct airport* ap)
{
    if(ap->refcount_) ap->refcount_--;
    else free(ap);
}

unsigned int ap_refcount(const struct airport* ap)
{
    return ap->refcount_;
}