set title "Impact of aging on the abilities of a character"
set terminal png
set grid
set autoscale
set style fill solid 1.0
set output "aging-impact.png"
set xlabel "Percentage of lifespan reached"
set ylabel "Percentage of abilities actually available"
set key box right
set style fill solid border -1 
plot "aging-impact.dat" using 1:2 title 'Ability Percentage' with linespoints lt rgb "blue" , "aging-impact.dat" using 1:3 title 'Corresponding modifier' with linespoints lt rgb "orange" 

