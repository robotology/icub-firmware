set datafile separator ","
plot "test_out_1.csv" using 1 with linespoints lc rgb "red", "test_out_1.csv" using 2 with linespoints lc rgb "blue", "test_out_1.csv" using 3 with linespoints lc rgb "green", "test_out_1.csv" using 4 with linespoints lc rgb "violet"
pause mouse close
