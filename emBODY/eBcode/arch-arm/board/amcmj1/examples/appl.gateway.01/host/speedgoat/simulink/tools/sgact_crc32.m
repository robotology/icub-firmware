function crc = sgact_crc32(path)
%SGACT_CRC32 the CRC32 of a file with CRLF counted as LF (as sgact_spec.crc_of in Python): the build compares it with
%   sgact_spec().CRC to know that matlab/sgact_spec.m was generated from the current sgact_panel.toml
    fid = fopen(path, 'r');
    if fid < 0
        error('sgact:crc', 'cannot open %s', path);
    end
    data = fread(fid, inf, '*uint8');
    fclose(fid);
    keep = true(size(data));
    keep(find(data(1:end - 1) == 13 & data(2:end) == 10)) = false;    % CR of a CRLF
    data = data(keep);

    persistent table
    if isempty(table)
        table = zeros(256, 1, 'uint32');
        for i = 0:255
            c = uint32(i);
            for k = 1:8
                if bitand(c, uint32(1))
                    c = bitxor(bitshift(c, -1), uint32(3988292384));     % 0xEDB88320
                else
                    c = bitshift(c, -1);
                end
            end
            table(i + 1) = c;
        end
    end
    crc = uint32(4294967295);
    for i = 1:numel(data)
        idx = bitand(bitxor(crc, uint32(data(i))), uint32(255));
        crc = bitxor(bitshift(crc, -8), table(idx + 1));
    end
    crc = bitxor(crc, uint32(4294967295));
end
