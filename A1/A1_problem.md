# C PROGRAMMING

23:59 on 29 Jan 2021

Implement a few pure C functions pertaining to simple data structures.

## Objectives

After completing this assignment, you should be able to:

- translate high-level data structures into C structures and
- implement data structures code in pure C.

## Directions

Read the header files [aircraft.h](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/1/aircraft.h), [airport.h](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/1/airport.h) and [flight.h](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/1/flight.h). Design the data structures that are described in these header files and implement them in C. Submit your work as `aircraft.c`, `airport.c` and `flight.c.

## Deliverables

To complete this assignment you must submit source code via [Gradescope](https://www.gradescope.ca/courses/3478).



# Autograder Results

AUTOGRADER SCORE 11.0 / 12.0

Autograder Output

```
Student submitted 4 file(s):
  submission: directory
  submission/aircraft.c: C source, ASCII text, with CRLF line terminators
  submission/airport.c: C source, ASCII text, with CRLF line terminators
  submission/flight.c: C source, ASCII text, with CRLF line terminators

Compiling 'aircraft.c'...
clang -c -Wall -fPIC -I source -I /usr/local/include submission/aircraft.c -o aircraft.o

Compiling 'airport.c'...
clang -c -Wall -fPIC -I source -I /usr/local/include submission/airport.c -o airport.o

Compiling 'flight.c'...
clang -c -Wall -fPIC -I source -I /usr/local/include submission/flight.c -o flight.o

Compiling tests...
clang++ -c -std=c++17 -Wall -fPIC -I source -I /usr/local/include -Wall source/tests.cpp -o tests.o

Linking tests with submission...
clang++ tests.o aircraft.o airport.o flight.o -lgrading -L /usr/local/lib -o tester

Running submission... OK
```

Aircraft allocation/deallocation

```
Test description:
 - create a aircraft with ac_create()
 - free the aircraft with ac_release()


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

Aircraft registration

```
Test description:
 - create an aircraft with registration "C-ENGI"
 - overwrite original registration string
 - check ac_registration still gives correct answer


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

Aircraft kind

```
Test description:
 - create an AC_FIXED aircraft, check ac_kind()
 - create an AC_ROTARY aircraft, check ac_kind()
 - create an AC_TILT aircraft, check ac_kind()
 - free all aircraft with ac_release()


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------
Fixed-wing (AC_FIXED)...
Helicopter (AC_ROTARY)...
Tilt-rotor (AC_TILT)...
Free all...

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

Aircraft refcount

```
Test description:
 - create a aircraft with ac_create()
 - check that refcount is 1
 - hold the aircraft
 - check that refcount is 2
 - hold the aircraft
 - check that refcount is 3
 - release the aircraft
 - check that refcount is 2
 - release the aircraft
 - check that refcount is 1
 - release the aircraft


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

Free NULL flight

```
Test description:
 - call flight_free(NULL)


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------
flight_free over 

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

Flight code

```
Test description:
 - create flight with code "WS" and number 123
 - check that flight_code() returns "WS123"


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------
flight_creat start 
flight_code over 
Flight is WS123
flight_creat over 
flight_code over 

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

Flights referencing aircraft change refcounts

```
Test description:
 - create aircraft C-GZKI, check that refcount==1
 - create flight AC860
 - set AC860's equipment to C-GZKI
 - check that C-GZKI's refcount==2
 - create aircraft C-GZSV, check that refcount==1
 - set AC860's equipment to C-GZSV
 - check that C-GZSV's refcount==2
 - check that C-GZKI's refcount==1
 - set AC860's equipment to NULL
 - check that C-GZSV's refcount==1


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------
Creating C-GZKI...
Creating AC860...
flight_creat start 
flight_code over 
Flight is AC860
flight_creat over 
Setting AC860 equipment to C-GZKI...
flight_change_equipment over 
Creating C-GZSV...
Setting AC860 equipment to C-GZSV...
flight_change_equipment over 
Setting AC860 equipment to NULL...
flight_change_equipment over 

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

Flights equipment freeing

```
Test description:
 - create aircraft C-XYZA, check that refcount==1
 - create flight WS123
 - set WS123's equipment to C-XYZA
 - check that C-XYZA's refcount==2
 - free flight with flight_free()
 - check that C-XYZA's refcount==1


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------
Creating aircraft C-XYZA...
Creating WS123...
flight_creat start 
flight_code over 
Flight is WS123
flight_creat over 
Setting WS123 equipment to C-XYZA...
flight_change_equipment over 
Freeing WS123...
flight_free over 

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

Airport code

```
Test description:
 - write string "CYYT" into string buffer
 - create airport with ap_create(buffer)
 - check that ap_code() returns "CYYT"
 - overwrite buffer with "XXXX"
 - check that ap_code() still returns "CYYT"


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------
Creating airport...
ap_create start 
airport: CYYT;ap_creat over 
Overwriting original buffer...

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

Airport refcount

```
Test description:
 - create a airport with ap_create()
 - check that refcount is 1
 - hold the airport
 - check that refcount is 2
 - hold the airport
 - check that refcount is 3
 - release the airport
 - check that refcount is 2
 - release the airport
 - check that refcount is 1
 - release the airport


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------
ap_create start 
airport: CYOW;ap_creat over 
ap_hold over 
ap_hold over 
ap_release over 
ap_release over 
ap_release over 

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

Add flight to airports

```
Test description:
 - create a flight: CYVR->CYYC->CYYZ->CYUL
 - add this flight to all four airports
 - check that each airport's ac_flights() yields all flights


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: segmentation fault
```

Add flight (missing stop) to airport

```
Test description:
 - create airports CYYT, CYHZ, CYOW and CYYZ 
 - create flight from CYTT to CYHZ to CYYZ
 - attempt to add flight to CYOW (should fail)


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------
ap_create start 
airport: CYYT;ap_creat over 
ap_create start 
airport: CYHZ;ap_creat over 
ap_create start 
airport: CYOW;ap_creat over 
ap_create start 
airport: CYYZ;ap_creat over 
flight_creat start 
flight_code over 
Flight is BA0420x7f
flight_creat over 
ap_add_flight start 
ap_add_flight over 

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```