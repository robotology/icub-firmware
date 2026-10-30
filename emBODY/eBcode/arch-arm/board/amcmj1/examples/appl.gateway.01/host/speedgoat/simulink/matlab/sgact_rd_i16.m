function x = sgact_rd_i16(b, i)
%SGACT_RD_I16 the int16 at b(i), little endian, as a double scalar (see sgact_rd_u)
    x = sgact_rd_u(b, i, 2);
    if x >= 32768
        x = x - 65536;
    end
end
