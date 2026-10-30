function x = sgact_rd_f32(b, i)
%SGACT_RD_F32 the single at b(i:i+3), little endian, as a double scalar. v(1): a scalar also in the generated code
    v = typecast(uint8([b(i); b(i + 1); b(i + 2); b(i + 3)]), 'single');
    x = double(v(1));
end
