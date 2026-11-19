function h = sgact_tether_header(numOfROPs, tus)
%SGACT_TETHER_HEADER the 16 bytes of the header of a TETHER frame of protocol 6 (tethercodec.py)
%   { uint32 signature 0xFEEDC0DE, uint8 layout = 0 (variable-length ROPs), uint8 numOfROPs, uint16 filler = 0, uint64 timeinmicroseconds }
%   then the ROPs one after the other: { uint8 opc, uint8 id, uint16 size } + size bytes of data + zeros up to a multiple of 4
    c = sgact_const();
    h = zeros(16, 1, 'uint8');
    h(1:4) = sgact_bytes(c.SIGNATURE);
    h(5) = uint8(c.LAYOUT_VARIABLE);
    h(6) = uint8(numOfROPs);
    h(9:16) = sgact_bytes(uint64(tus));
end
