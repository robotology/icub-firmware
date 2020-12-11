set datafile separator ","
plot "generated/rhomb_8192_raw.csv" using 1 with linespoints lc rgb "blue", "generated/rhomb_8192_interpolated.csv" using 1 with linespoints lc rgb "red", "generated/rhomb_8192_interpolated_and_scaled.csv" using 1 with linespoints lc rgb "green"
pause mouse close
