set title "Impact of modifiers on base probabilities"
set terminal png transparent
set style line 1 lt 1 lw 3
set style line 2 lt rgb 'green' lw 3 
set style line 3 lt 3 lw 3
set grid
set output "probability-modifier.png"
set xlabel "Base probability of success (in %)"
set ylabel "Final probability of success (in %):\nmodifier M applied to base probability"
set key  rmargin vertical  box title "Modifiers M:"
set label "M=50%"  at 28,78 front
set label "M=0%"   at 48,50 front
set label "M=-50%" at 73,30 front
plot "probability-modifier.dat" using 1:22 title '+100%' lt 12 with lines, "probability-modifier.dat" using 1:21 title '+90%'  ls 11 with lines, "probability-modifier.dat" using 1:20 title '+80%'  ls 10 with lines, "probability-modifier.dat" using 1:19 title '+70%'  ls 9  with lines, "probability-modifier.dat" using 1:18 title '+60%'  ls 8  with lines, "probability-modifier.dat" using 1:17 title '+50%'  ls 2  with lines, "probability-modifier.dat" using 1:16 title '+40%'  ls 7  with lines, "probability-modifier.dat" using 1:15 title '+30%'  ls 6  with lines, "probability-modifier.dat" using 1:14 title '+20%'  ls 5  with lines, "probability-modifier.dat" using 1:13 title '+10%'  ls 4  with lines, "probability-modifier.dat" using 1:12 title   '0%'  ls 3  with lines, "probability-modifier.dat" using 1:11 title '-10%'  ls 4  with lines, "probability-modifier.dat" using 1:10 title '-20%'  ls 5  with lines, "probability-modifier.dat" using 1:9  title '-30%'  ls 6  with lines, "probability-modifier.dat" using 1:8  title '-40%'  ls 7  with lines, "probability-modifier.dat" using 1:7  title '-50%'  ls 1  with lines,    "probability-modifier.dat" using 1:6  title '-60%'  ls 8  with lines, "probability-modifier.dat" using 1:5  title '-70%'  ls 9  with lines, "probability-modifier.dat" using 1:4  title '-80%'  ls 10 with lines, "probability-modifier.dat" using 1:3  title '-90%'  ls 11 with lines, "probability-modifier.dat" using 1:2  title '-100%' ls 12 with lines
