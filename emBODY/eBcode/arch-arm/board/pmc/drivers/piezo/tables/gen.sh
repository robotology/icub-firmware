#!/bin/bash
mkdir -p generated
cd generated

../faulhaber_csv_to_c.py ../srcdata/delta.csv 3 8192 65407 128 delta_8192
../faulhaber_csv_to_c.py ../srcdata/delta.csv 3 1024 65407 128 delta_1024
../faulhaber_csv_to_c.py ../srcdata/rhomb.csv 7 8192 65407 128 rhomb_8192
