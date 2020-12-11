set datafile separator ","
plot "test_out.csv" using 1 with linespoints, "test_out.csv" using 2 with linespoints, "test_out.csv" using 3 with linespoints, "test_out.csv" using 4 with linespoints
pause mouse close
