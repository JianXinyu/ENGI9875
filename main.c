#include "aircraft.h"
#include "airport.h"
#include "flight.h"
#include <stdio.h>
#include <stdlib.h>
int main()
{
    struct aircraft *ac = ac_create(AC_FIXED, "C-GZKI");
//    const char *ch = ac_registration(ac);
//    int ref = ac_refcount(ac);
//    ac_hold(ac);
//    ref = ac_refcount(ac);
//    ac_release(ac);
//    ref = ac_refcount(ac);
//    ac_release(ac);
//    ref = ac_refcount(ac);

    struct airport *stops[5];
    stops[0] = ap_create("CYVR");
    stops[1] = ap_create("CYYC");
    stops[2] = ap_create("CYYZ");
    stops[3] = ap_create("CYUL");
    stops[4] = NULL;
    struct flight* flt = flight_create("AC", 860, stops);
//    struct flight* flt2 = flight_create("AC", 226, stops);
    for(int i = 0; i < 4; i++)
    {
        ap_add_flight(stops[i], flt);
//        ap_add_flight(stops[i], flt2);
    }
    struct flight **fpp= malloc(10 * sizeof*fpp);
    size_t i = sizeof(int);
    size_t * n = &i;
    ap_flights(stops[0], fpp, n);
    ap_flights(stops[1], fpp, n);
    ap_flights(stops[2], fpp, n);
    ap_flights(stops[3], fpp, n);
    flight_change_equipment(flt, ac);
//    int ref = ac_refcount(ac);
//    flight_free(flt);
//    ref = ac_refcount(ac);
////    struct aircraft *ac2 = ac_create(AC_FIXED, "C-GZSV");
////    flight_change_equipment(flt, ac2);
////    ref = ac_refcount(ac2);
////    ref = ac_refcount(ac);
////    flight_change_equipment(flt, NULL);
////    ref = ac_refcount(ac2);
//    char buffer[5] = "CYYT";
//    struct airport * ap2 = ap_create("CYYT");
//    char * ch2 = ap_code(ap2);
//    printf(ch2);
//    printf("\n");
//    for(int i = 0; i < 4; i++)
//        buffer[i] = 'X';
//    ch2 = ap_code(ap2);
//    printf(ch2);
//    printf("\n");
//
//    struct airport *ap3 = ap_create("CYYT");
//    ap_hold(ap3);
//    ap_hold(ap3);
//    ref = ap_refcount(ap3);
//    printf("%d\n", ref);
//    ap_release(ap3);
//    ap_release(ap3);
//    ref = ap_refcount(ap3);
//    printf("%d\n", ref);
//    ap_release(ap3);
    return 0;
}

