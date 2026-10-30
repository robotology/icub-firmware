function x = sgact_rd_u(b, i, n)
%SGACT_RD_U the unsigned integer of n bytes (1, 2, 4, 8) at b(i), little endian, as a double scalar
%   w/ arithmetic, not typecast: in the generated code typecast of a slice can be variable-size, this is always a scalar.
%   a uint64 is exact up to 2^53 (the time in microseconds of amc.gtw: 285 years)
    x = 0;
    for k = n:-1:1
        x = x * 256 + double(b(i + k - 1));
    end
end
