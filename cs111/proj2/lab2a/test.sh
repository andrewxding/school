#!/bin/bash
rm -rf lab2_add.csv
rm -rf lab2_list.csv

threads=(2 4 8 12)
yield_threads=(2 8)
threads1=(1 2 4 8 12)
add_1_iter=(10 20 40 80 100 1000 10000 100000)
add_2_iter=(100 1000 10000 100000)
list1_iter=(10 100 1000 10000 20000)
list2_iter=(1 10 100 1000) 
list3_iter=(1 2 4 8 16 32)
list_threads=(1 2 4 8 12 16 24)
list_yield=(i d il dl)

for t in "${threads[@]}"; do
	for i in "${add_1_iter[@]}"; do
		echo "./lab2a_add --iterations=$i --threads=$t"
		./lab2_add --iterations=$i --threads=$t 1>>lab2_add.csv
	done
done


for t in "${yield_threads[@]}"; do
	for i in "${add_2_iter[@]}"; do
		echo "./lab2a_add --iterations=$i --threads=$t --yield"
		./lab2_add --iterations=$i --threads=$t --yield 1>>lab2_add.csv
	done
done

for i in "${add_1_iter[@]}"; do
	echo "./lab2_add --threads=1  --iterations=$i"
	./lab2_add --threads=1  --iterations=$i >> lab2_add.csv
done

for t in "${threads[@]}"; do
	echo "./lab2a_add --iterations=10000 --threads=$t --sync=c --yield"
	./lab2_add --iterations=$i --threads=$t --sync=c  --yield 1>>lab2_add.csv

	echo "./lab2a_add --iterations=10000 --threads=$t --sync=m --yield"
	./lab2_add --iterations=$i --threads=$t --sync=m  --yield 1>>lab2_add.csv

	echo "./lab2a_add --iterations=1000 --threads$t --sync=s --yield"
	./lab2_add --iterations=$i --threads=$t --sync=s  --yield 1>>lab2_add.csv

	echo "./lab2a_add --iterations=1000 --threads$t --yield"
	./lab2_add --iterations=$i --threads=$t  --yield 1>>lab2_add.csv
done

for t in "${threads1[@]}"; do
	echo "./lab2a_add --iterations=10000 --threads=$t --sync=c"
	./lab2_add --iterations=$i --threads=$t --sync=c 1>>lab2_add.csv

	echo "./lab2a_add --iterations=10000 --threads=$t --sync=m"
	./lab2_add --iterations=$i --threads=$t --sync=m 1>>lab2_add.csv

	echo "./lab2a_add --iterations=1000 --threads$t --sync=s"
	./lab2_add --iterations=$i --threads=$t --sync=s 1>>lab2_add.csv

	echo "./lab2a_add --iterations=1000 --threads$t"
	./lab2_add --iterations=$i --threads=$t 1>>lab2_add.csv
done


# list
for i in "${list1_iter[@]}"; do
	echo "./lab2_list --iterations=$i --threads=1"
	./lab2_list --iterations=$i --threads=1 1>>lab2_list.csv
done

for t in "${threads[@]}"; do
	for i in "${list2_iter[@]}"; do
		echo "./lab2_list --iterations=$i --threads=1"
		./lab2_list --iterations=$i --threads=1 1>>lab2_list.csv
	done
done

for t in "${threads[@]}"; do
	for i in "${list3_iter[@]}"; do
		for y in "${list_yield[@]}"; do
			echo "./lab2_list --iterations=$i --threads=$t --yield=$y"
			./lab2_list --iterations=$i --threads=$t --yield=$y 1>>lab2_list.csv
		done
	done
done

for t in "${threads[@]}"; do
	for i in "${list3_iter[@]}"; do
		for y in "${list_yield[@]}"; do
			echo "./lab2_list --iterations=$i --threads=$t --yield=$y --sync=m"
			./lab2_list --iterations=$i --threads=$t --yield=$y --sync=m 1>>lab2_list.csv
			echo "./lab2_list --iterations=$i --threads=$t --yield=$y --sync=s"
			./lab2_list --iterations=$i --threads=$t --yield=$y --sync=s 1>>lab2_list.csv
		done
	done
done

for t in "${list_threads[@]}";do
	echo "./lab2_list --iterations=1000 --threads=$t --sync=m"
	./lab2_list --iterations=1000 --threads=$t --sync=m 1>>lab2_list.csv
	echo "./lab2_list --iterations=1000 --threads=$t --sync=s"
	./lab2_list --iterations=1000 --threads=$t --sync=s 1>>lab2_list.csv
done