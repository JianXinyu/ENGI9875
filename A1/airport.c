#include "airport.h"
#include <stdlib.h>
struct airport{
    const char *ap_code;
    unsigned int refcount_;
    struct flight * flights[6];
};

struct airport*	ap_create(const char *icao_code)
{
    struct airport *ap = malloc(sizeof(struct airport));
    ap->ap_code = icao_code;
    ap->refcount_ = 0;

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

    int i = 0;
    while((flt->f_stops[i])->ap_code)
    {
        i++;
        if((flt->f_stops[i])->ap_code == ap->ap_code)
        {
            //success
            flag = 0;
            ap->flights[ap->refcount_] = flt;
            ap_hold(ap);
        }
    }
    return flag;
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