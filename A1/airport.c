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
    printf("ap_flights start \n");
     fflush(stdout); // hold the buffer
//    printf("n = %zu ", *n);
//    printf("airport: %s;", ap->ap_code);
//    char * test = flight_code(ap->flights[0]);
//    printf("airport flight: %s\n", test);

    if (*n < ap->flt_count )
    {
        *n = ap->flt_count;
        return;
    }

    for(int i = 0; i < ap->flt_count; i++)
        fpp[i] = ap->flights[i];

    printf("ap_flights over \n");
}

/* Add flights to airports */
//    Test description:
//    - create a flight: CYVR->CYYC->CYYZ->CYUL
//    - add this flight to all four airports
//    - check that each airport's ac_flights() yields all flights

//struct airport *stops[5];
//stops[0] = ap_create("CYVR");
//stops[1] = ap_create("CYYC");
//stops[2] = ap_create("CYYZ");
//stops[3] = ap_create("CYUL");
//stops[4] = NULL;
//struct flight* flt = flight_create("AC", 860, stops);
//struct flight* flt2 = flight_create("AC", 123, stops);
//for(int i = 0; i < 4; i++)
//{
//ap_add_flight(stops[i], flt);
//ap_add_flight(stops[i], flt2);
//}
//struct flight **fpp= malloc(1 * sizeof*fpp);
//size_t i = sizeof(int);
//size_t * n = &i;
//ap_flights(stops[0], fpp, n);
//ap_flights(stops[1], fpp, n);
//ap_flights(stops[2], fpp, n);
//ap_flights(stops[3], fpp, n);

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