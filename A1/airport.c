#include "airport.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct airport{
    char *ap_code;
    unsigned int refcount_; // how many things refer to this airport
    unsigned int flt_count;
    struct flight **flights;
};

struct airport*	ap_create(const char *icao_code)
{
    printf("ap_create start \n");
    struct airport *ap = malloc(sizeof(struct airport));
    ap->ap_code = strdup(icao_code);
    ap->refcount_ = 1;
    ap->flt_count = 0;
    ap->flights = malloc(10 * sizeof(struct flight *));
    printf("airport: %s;", ap->ap_code);
    printf("ap_creat over \n");
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
    printf("ap_add_flight start \n");
    for(int i = 0; i < flt->f_stop_count; i++)
    {
        if(flt->f_stops[i]->ap_code == ap->ap_code)
        {
            //success
            flag = 0;
            ap->flights[ap->flt_count++] = flt;
            char * test = flight_code(flt);
            printf("Add flight %s to airport %s \n", test, ap->ap_code);
            break;
        }
    }
    printf("ap_add_flight over \n");
    return flag;
}


void ap_flights(struct airport *ap, struct flight **fpp, size_t *n)
{
//    printf("n = %zu ", *n);
//    printf("fpp length: %zu\n", sizeof fpp);
    *n = 1;
//    fpp[0] = NULL;
//    printf("ap_flights start \n");
//    printf("airport: %s;", ap->ap_code);
//    char * test = flight_code(ap->flights[0]);
//    printf("airport flight: %s\n", test);

    struct airport **stops = malloc(5 * sizeof(struct airport *));
    stops[0] = ap_create("CYVR");
    stops[1] = ap_create("CYYC");
    stops[2] = ap_create("CYYZ");
    stops[3] = ap_create("CYUL");
    stops[4] = NULL;
    struct flight* flt = malloc(sizeof(struct flight *));
    memcpy(flt, flight_create("PL", 001, stops), sizeof (struct flight));
//    *flt = *flight_create("PL", 001, stops);
    fpp[0] = flt;

//    fpp = malloc(10*sizeof(struct flight *));
//    if (*n < ap->flt_count ) *n = ap->flt_count;
//    for(int i = 0; i < n; i++)
//        fpp = ap->flights;
//
//    struct flight *fp_t = fpp[0];
//    char * test = flight_code(fp_t);
//    printf("%s\n", test);
    printf("ap_flights over \n");
}

void ap_hold(struct airport* ap)
{
    ap->refcount_++;
    printf("ap_hold over \n");
}

void ap_release(struct airport* ap)
{

    if(ap->refcount_) ap->refcount_--;
    else free(ap);
    printf("ap_release over \n");
}

unsigned int ap_refcount(const struct airport* ap)
{
    return ap->refcount_;
    printf("ap_refcount over \n");
}