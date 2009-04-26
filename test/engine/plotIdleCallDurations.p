set terminal png size 5000,600
set autoscale
unset log                              
unset label                            
set xtic auto						   
set ytic auto		
set grid				   
set title "Comparing the forecasted and actual (measured) durations of idle calls in the course of the scheduling"
set xlabel "Idle call count"
set ylabel "Idle call durations (microsecond)"
set terminal png
set output "testOSDLScheduler-idleCallDurations.png"
plot "idle-calls.dat" using 1:2 title "Forecasted durations" with linespoints, "idle-calls.dat" using 1:3 title "Measured durations" with linespoints
