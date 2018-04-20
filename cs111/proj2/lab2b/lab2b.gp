#! /usr/bin/gnuplot

set terminal png
set datafile separator ","

# GRAPH 1
set title "Lab2B-1: Throughput vs Number of Threads for Mutexs and Spin-Locks"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_1.png'

plot \
     "< grep list-none-m lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex list operations' with points lc rgb 'blue', \
     "< grep list-none-s lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spin-lock list operations' with points lc rgb 'orange'

# GRAPH 2
set title "Lab2B-2: Wait-for-Lock Time and Average Time per Operation vs Number Of Threads for Mutex"
set xlabel "Number of threads"
set logscale x 2
set ylabel "Time"
set logscale y 10
set output 'lab2b_2.png'

plot \
     "< grep list-none-m lab2b_list.csv" using ($2):($7) \
	title 'Average Time Per Operation: Mutex' with points lc rgb 'red', \
     "< grep list-none-m lab2b_list.csv" using ($2):($8) \
	title 'Average Wait Time: Mutex' with points lc rgb 'blue'

# GRAPH 3
set title "Lab2B-3: Unprotected and Protected Iterations that Run Without Failure"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'

plot \
     "< grep list-id-none lab2b_list.csv" using ($2):($3) \
	title 'unprotected' with points lc rgb 'green', \
     "< grep list-id-s lab2b_list.csv" using ($2):($3) \
	title 'spin lock' with points lc rgb 'blue', \
	 "< grep list-id-s lab2b_list.csv" using ($2):($3) \
	title 'mutex' with points lc rgb 'red'

# GRAPH 4
set title "Lab2B-4: Throughput with Mutex"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Operations per Second"
set logscale y 10
set output 'lab2b_4.png'

# grep out the iterations containing "list-none-m"
plot \
     "< grep 'list-none-m,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '1 list' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,.*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '4 lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,.*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '8 lists' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,.*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '16 lists' with linespoints lc rgb 'orange'


# GRAPH 5
set title "Lab2B-5: Throughput with Spin Lock"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Operations per Second"
set logscale y 10
set output 'lab2b_5.png'

plot \
     "< grep 'list-none-s,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '1 list' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,.*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '4 lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-s,.*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '8 lists' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s,.*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '16 lists' with linespoints lc rgb 'orange'