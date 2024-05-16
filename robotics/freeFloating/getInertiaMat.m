function [dxr0i, dqr0i, dxIi, dqIi, dxJTi, dqJTi, dxJRi, dqJRi] = getInertiaMat(b, q, bh, dh, com, Im)
%GETINERTIAMAT Summary of this function goes here
%   Detailed explanation goes here

% Degrees of freedom manipulator
n = size(q, 1);

%% Dynamics (basic quantities)
% Compute Ti, CoM0i, JTi JRi
Ti = sym(zeros(4, 4, n+2));
com0i = sym(zeros(n, 3));
JTi = sym(zeros(3, n, n));
JRi = sym(zeros(3, n, n));

% Loop
Ti(:, :, 1) = TFF(bh, dh, 1);
Ti(:, :, 2) = simplify(Ti(:, :, 1)*TFF(bh, dh, 2));
for i = 1:n
    % Get next transformation matrix
    Ti(:, :, i+2) = simplify(Ti(:, :, i+1) * TFF(bh, dh, i+2));

    % Extract required mat
    T0i = Ti(:, :, i+2);
    
    % Get i-th CoM wrt sc CoM
    com0i(i, :) = simplify(T0i(1:3, 4) + T0i(1:3, 1:3)*com(i, :).'); % !!!! This is wrong, you're starting from the base
    
    % Compute JTi and JRi
    JTit = sym(zeros(3, n));
    JRit = sym(zeros(3, n));
    for j = 1:i     
        % Extract required mat
        T0jp = Ti(:, :, j+1);

        % Update Jacobians
        JTit(:, j) = cross(T0jp(1:3, 3), com0i(i, :).' - T0jp(1:3, 4));
        JRit(:, j) = T0jp(1:3, 3);
        
        % Simplify
        JTit = simplify(JTit);
        JRit = simplify(JRit);
    end
    
    % Append
    JTi(:, :, i) = JTit;
    JRi(:, :, i) = JRit;

    % Debug
    fprintf("Recovered base quantities: %d\n", i)
end

%% Compute derivatives (dx and dq)
% init dx
dxr0i = sym(zeros(n, 3, 6));     % joints, dimension, base
dxIi = sym(zeros(3, 3, n, 6));   % joints, dimension, dimension, base
dxJTi = sym(zeros(3, n, n, 6));  % joints, dimension, joints, base
dxJRi = sym(zeros(3, n, n, 6));  % joints, dimension, joints, base

% init dq
dqr0i = sym(zeros(n, 3, n));     % joints, dimension, joints
dqIi = sym(zeros(3, 3, n, n));   % joints, dimension, dimension, joints
dqJTi = sym(zeros(3, n, n, n));  % joints, dimension, joints, joints
dqJRi = sym(zeros(3, n, n, n));  % joints, dimension, joints, joints

% Loop dq
for i = 1:n
    % Position of CoMs
    dqr0i(:, :, i) = diff(com0i(:, :), q(i));
    for j = 1:n
        % Transf mat
        Tj = Ti(:, :, j+2);

        % Inertia matrix
        dqIi(:, :, j, i) = diff(Tj(1:3, 1:3)*Im(:, :, j)*Tj(1:3, 1:3).', q(i));

        % Jacobians
        dqJTi(:, :, j, i) = diff(JTi(:, :, j), q(i));
        dqJRi(:, :, j, i) = diff(JRi(:, :, j), q(i));
    end
    fprintf("Simplified dq: %d\n", i)
end

% Loop dx
for i = 1:6
    % Position of CoMs
    dxr0i(:, :, i) = diff(com0i(:, :), b(i));
    for j = 1:n
        % Transf mat
        Tj = Ti(:, :, j+2);

        % Inertia matrix
        dxIi(:, :, j, i) = diff(Tj(1:3, 1:3)*Im(:, :, j)*Tj(1:3, 1:3).', b(i));

        % Jacobians
        dxJTi(:, :, j, i) = diff(JTi(:, :, j), b(i));
        dxJRi(:, :, j, i) = diff(JRi(:, :, j), b(i));
    end
    fprintf("Simplified dx: %d\n", i)
end

% Save matrices
save("dxr0i.mat", "dxr0i");
save("dqr0i.mat", "dqr0i");
save("dxIi.mat", "dxIi");
save("dqIi.mat", "dqIi");
save("dxJTi.mat", "dxJTi");
save("dqJTi.mat", "dqJTi");
save("dxJRi.mat", "dxJRi");
save("dqJRi.mat", "dqJRi");

end

