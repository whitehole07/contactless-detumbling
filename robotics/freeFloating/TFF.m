function T = TFF(bh, dh, joint)
% Generic transformation matrix between adjacent frames using DH convention
    if joint == 1
        T =  [
            cos(bh(1, 5))*cos(bh(1, 6)), sin(bh(1, 4))*sin(bh(1, 5))*cos(bh(1, 6)) - cos(bh(1, 4))*sin(bh(1, 6)), cos(bh(1, 4))*sin(bh(1, 5))*cos(bh(1, 6)) + sin(bh(1, 4))*sin(bh(1, 6)), bh(1, 1)
            cos(bh(1, 5))*sin(bh(1, 6)), sin(bh(1, 4))*sin(bh(1, 5))*sin(bh(1, 6)) + cos(bh(1, 4))*cos(bh(1, 6)), cos(bh(1, 4))*sin(bh(1, 5))*sin(bh(1, 6)) - sin(bh(1, 4))*cos(bh(1, 6)), bh(1, 2)
            -sin(bh(1, 5)), sin(bh(1, 4))*cos(bh(1, 5)), cos(bh(1, 4))*cos(bh(1, 5)), bh(1, 3)
            0 0 0 1
            ];
    elseif joint == 2
        T =  [
            cos(bh(2, 5))*cos(bh(2, 6)), sin(bh(2, 4))*sin(bh(2, 5))*cos(bh(2, 6)) - cos(bh(2, 4))*sin(bh(2, 6)), cos(bh(2, 4))*sin(bh(2, 5))*cos(bh(2, 6)) + sin(bh(2, 4))*sin(bh(2, 6)), bh(2, 1)
            cos(bh(2, 5))*sin(bh(2, 6)), sin(bh(2, 4))*sin(bh(2, 5))*sin(bh(2, 6)) + cos(bh(2, 4))*cos(bh(2, 6)), cos(bh(2, 4))*sin(bh(2, 5))*sin(bh(2, 6)) - sin(bh(2, 4))*cos(bh(2, 6)), bh(2, 2)
            -sin(bh(2, 5)), sin(bh(2, 4))*cos(bh(2, 5)), cos(bh(2, 4))*cos(bh(2, 5)), bh(2, 3)
            0 0 0 1
            ];
    else
        j = joint - 2;
        T = [ % bh(1, 5), a, d, alpha
            cos(dh(j, 1)), -sin(dh(j, 1))*cos(dh(j, 4)), sin(dh(j, 1))*sin(dh(j, 4)), dh(j, 2)*cos(dh(j, 1))
            sin(dh(j, 1)), cos(dh(j, 1))*cos(dh(j, 4)), -cos(dh(j, 1))*sin(dh(j, 4)), dh(j, 2)*sin(dh(j, 1))
            0, sin(dh(j, 4)), cos(dh(j, 4)), dh(j, 3)
            0, 0, 0, 1
            ];
    end
end

