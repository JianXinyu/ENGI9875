#include "aircraft.h"
#include "airport.h"
#include "flight.h"

#include <stdlib.h>
int main()
{

    /* Add flights to airports */
//    Test description:
//    - create a flight: CYVR->CYYC->CYYZ->CYUL
//    - add this flight to all four airports
//    - check that each airport's ac_flights() yields all flights

    struct airport *stops[5];
    stops[0] = ap_create("CYVR");
    stops[1] = ap_create("CYYC");
    stops[2] = ap_create("CYYZ");
    stops[3] = ap_create("CYUL");
    stops[4] = NULL;
    struct flight* flt = flight_create("AC", 860, stops);
    struct flight* flt2 = flight_create("AC", 123, stops);
    for(int i = 0; i < 4; i++)
    {
        ap_add_flight(stops[i], flt);
        ap_add_flight(stops[i], flt2);
    }
    struct flight **fpp= malloc(10 * sizeof*fpp);
    size_t i = sizeof(int);
    size_t * n = &i;
    ap_flights(stops[0], fpp, n);
    ap_flights(stops[1], fpp, n);
    ap_flights(stops[2], fpp, n);
    ap_flights(stops[3], fpp, n);
    struct flight *fp_t = fpp[1];

    return 0;
}

