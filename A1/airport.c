#include "airport.h"
#include <stdlib.h>
struct airport{
    const char *ap_code;
    unsigned int refcount_;
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

int	ap_add_flight(struct flight* flt)
{

}
