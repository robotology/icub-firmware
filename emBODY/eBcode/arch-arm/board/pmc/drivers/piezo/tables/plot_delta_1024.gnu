set datafile separator ","
plot "generated/delta_1024_raw.csv" using 1 with linespoints lc rgb "blue", "generated/delta_1024_interpolated.csv" using 1 with linespoints lc rgb "red", "generated/delta_1024_interpolated_and_scaled.csv" using 1 with linespoints lc rgb "green"
pause mouse close
