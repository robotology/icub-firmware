function b = sgact_bytes(x)
%SGACT_BYTES the bytes of a scalar (little endian: the Speedgoat and the PC are x86) as a uint8 column of fixed size
    t = typecast(x, 'uint8');
    b = t(:);
end
