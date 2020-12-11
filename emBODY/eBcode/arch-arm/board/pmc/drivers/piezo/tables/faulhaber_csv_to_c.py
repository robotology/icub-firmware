#!/usr/bin/python

import csv
import numpy as np
import argparse
from os import path

def parse_csv(fname, col):
    values = []
    header = True
    with open(fname) as c:
        reader = csv.reader(c, delimiter=',')
        for row in reader:
            try:
                data = row[col]
                if not header and data == '':
                    #skip footer
                    return values
                v = int(data)
            except ValueError as e:
                if header:
                    continue
                else:
                    raise e
            header = False
            values.append(v)
    return values

"""
def parse_csv(fname):
    return read_csv(fname, usecols = [3], skiprows = 9, squeeze=True)
"""
def write_csv(fname, values):
    with open(fname, 'w') as c:
        for v in values:
            c.write(str(int(v)) +',\n')


def write_c_array(fname, values, arrayname):
    with open(fname, 'w') as f:
        f.write("static const uint16_t " + arrayname + "[{:d}] = {{\n\t".format(len(values)))
        for i, v in enumerate(values):
            if i > 0 and i % 8 == 0:
                f.write('\n\t')
            f.write(str(v) + ',\t')
        f.write("\n};")


def scale_data(values, target_max, target_min):
    data_max = max(values)
    data_min = min(values)
    print "data in [ {:f}:{:f} ]".format(data_min, data_max)
    # kill offset
    values = [v - data_min for v in values]
    data_max -= data_min

    # scale factor; we will apply offset later, so take this into account
    k = (target_max - target_min) / float(data_max)
    values = [v * k for v in values]
    values = [int(v + target_min) for v in values]

    print "new data in [ {:d}:{:d} ]".format(min(values), max(values))
    return values

def interpolate_decimate_data(values, n_points):
    orig = np.linspace(0, len(values), len(values))
    target = np.linspace(0, len(values), n_points)
    result = np.interp(target, orig, values)
    return result


def parse_cli():
    parser = argparse.ArgumentParser(description = "command line opts")
    parser.add_argument('fsrc', metavar = 'fsrc', help = 'src faulhaber file')
    parser.add_argument('col', metavar = 'col', type=int, help = 'column to parse (i.e. 3 for delta)')
    parser.add_argument('n_points', metavar = 'n_points', type=int,
                        help="The target number of points for interpolation/decimation")
    parser.add_argument('max', metavar = 'max', type=int,
                        help="max value of the wave")
    parser.add_argument('min', metavar = 'min', type=int,
                        help="min value of the wave")
    parser.add_argument('fpfx', metavar = 'fpfx', help = 'file name prefix / array_name', nargs='?')
    return parser.parse_args()

args = parse_cli()

values = parse_csv(args.fsrc, args.col)
if args.fpfx is None:
    fdst = path.splitext(path.basename(args.fsrc))[0]
else:
    fdst = args.fpfx
write_csv(fdst + '_raw.csv', values)
values = interpolate_decimate_data(values, args.n_points)
write_csv(fdst + '_interpolated.csv', values)
values = scale_data(values, args.max, args.min)
write_csv(fdst +'_interpolated_and_scaled.csv', values)
write_c_array(fdst +'_table.c', values, fdst)
