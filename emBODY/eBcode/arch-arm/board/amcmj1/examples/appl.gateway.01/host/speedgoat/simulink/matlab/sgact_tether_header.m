function h = sgact_tether_header(sizeofROP, numOfROPs, tus)
%SGACT_TETHER_HEADER the 16 bytes of the header of a TETHER frame (tethercodec.py)
%   { uint32 signature 0xFEEDC0DE, uint8 sizeofROP, uint8 numOfROPs, uint16 filler = 0, uint64 timeinmicroseconds }
    c = sgact_const();
    h = zeros(16, 1, 'uint8');
    h(1:4) = sgact_bytes(c.SIGNATURE);
    h(5) = uint8(sizeofROP);
    h(6) = uint8(numOfROPs);
    h(9:16) = sgact_bytes(uint64(tus));
end
