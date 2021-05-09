# Main files


- proxy.c: main routine
- cache.h, cache.c: cache implementation
- Makefile
- csapp.h, csapp.c: helper functions 

# Docs

this directory contains:

- lab report: 解答流程和思路
- recitations: most are about PXYDRIVE
- lab handout

# Test

- driver.sh
      The autograder for Basic, Concurrency, and Cache.        
      usage: ./driver.sh
- tiny
      Tiny Web server from the CS:APP text
- pxy: PXYDRIVE, a REPL for testing your proxy implementation
  - .cmd files are test scripts for PXYDRIVE

# Helper files


- port-for-user.pl, 
    Generates a random port for a particular user
    usage: ./port-for-user.pl <userID>
- free-port.sh
	Handy script that identifies an unused TCP port that you can use
    for your proxy or tiny. 
    usage: ./free-port.sh

- nop-server.py
       helper for the autograder.         



