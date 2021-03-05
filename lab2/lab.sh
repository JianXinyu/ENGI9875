#!/bin/bash
export PATH="~/Documents/ENGI9875/lab2:$PATH"

rm -f output.dat
for j in 10 100
do
	for w in 10 100
	do
		make clean
		make JOBS=$j WORK_PER_JOB=$w

		for i in $(seq 10)
		do
			serial | awk '$7' | tee -a serial_times.dat
			# pthreads | awk '{ print $7 }' | tee -a pthreads_times.dat
		done

		printf "JOBS: %d, WORK_PER_JOB: %d\n" $j $w | tee -a output.dat
		for i in {3,5}
		do 
			head -$i serial_times.dat | ministat -A -s | tee -a output.dat
			# head -$i pthreads_times.dat | ministat -A -s
		done

	done
done
