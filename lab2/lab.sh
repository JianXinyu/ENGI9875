#!/bin/bash
export PATH="~/Documents/ENGI9875/lab2:$PATH"
name=${1? Type error}

rm -f ${name}.dat
# redirect it to /dev/null to not write to the standard output
printf "JOBS\t WORK\t N\t Min\t Max\t Median\t Average\n" | tee -a ${name}.dat > /dev/null
for j in 10 100 1000 10000
do
	for w in 10 100 1000 10000
	do
		make clean
		make JOBS=$j WORK_PER_JOB=$w

		for i in $(seq 100)
		do
			${name} | awk '{printf"%s\n", $7}' | tee -a ${name}_times.dat > /dev/null
		done
		
		
		for i in 100
		do 
			printf "%d\t %d\t" $j $w | tee -a ${name}.dat
			head -$i ${name}_times.dat | ministat -A -s | awk 'NR>2{printf "%s\t%.1f\t%.1f\t%.1f\t%.1f\n", $2, $3, $4, $5, $6}'| tee -a ${name}.dat
		done

	done
done

# used for `Race conditions` 
# rm -f race.dat
# printf "JOBS\t WORK\t Counter\n" | tee -a race.dat
# for j in 10 100 1000 10000
# do
# 	for w in 10 100 1000 10000
# 	do
# 		make clean
# 		make JOBS=$j WORK_PER_JOB=$w
# 		printf "%d\t %d\t" $j $w | tee -a race.dat
# 		${name} | awk '{printf "%s\n", $3}' | tee -a race.dat
# 	done
# done