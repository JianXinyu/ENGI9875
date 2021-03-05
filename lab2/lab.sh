#!/bin/bash
JOB=${1?Error: no JOBS given}
WORK=${2?Error: no WORK_PER_JOB given}
make JOBS=$JOB WORK_PER_JOB=$WORK clean serial
export PATH="~/Documents/ENGI9875/lab2:$PATH"
echo $PATH
for i in $(seq 100)
do
	serial | awk '{ print $7 }' | tee -a initial_serial_times.dat
done

for i in {5,10,100}
do 
	head -$i initial_serial_times.dat | ministat -A -s
done

