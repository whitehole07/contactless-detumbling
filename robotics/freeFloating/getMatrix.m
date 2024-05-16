function [H, H_dot, c] = getMatrix( ...
    q, qd, qs, b, bd, bs, bh, dh, ...
    m, com, Im, m_sc, I0, ...
    dxr0i, dqr0i, dxIi, dqIi, dxJTi, dqJTi, dxJRi, dqJRi)

%GETMATRIX Summary of this function goes here
%   Detailed explanation goes here
% Subs values
bh = double(subs(bh, [q b], [qs bs]));
dh = double(subs(dh, [q b], [qs bs]));

dxr0i = double(subs(dxr0i, [q b], [qs bs]));
dqr0i = double(subs(dqr0i, [q b], [qs bs]));
dxIi = double(subs(dxIi, [q b], [qs bs]));
dqIi = double(subs(dqIi, [q b], [qs bs]));
dxJTi = double(subs(dxJTi, [q b], [qs bs]));
dqJTi = double(subs(dqJTi, [q b], [qs bs]));
dxJRi = double(subs(dxJRi, [q b], [qs bs]));
dqJRi = double(subs(dqJRi, [q b], [qs bs]));

% Total mass
mtot = sum(m) + m_sc;

% Degrees of freedom
n = size(dh, 1);

% Extract value
r0 = bh(1, 1:3);

%% Dynamics (basic quantities)
% Compute Ti, CoM0i, JTi JRi, wi
Ti = zeros(4, 4, n+2);
com0i = zeros(n, 3);
Ii = zeros(3, 3, n);
Id = zeros(3, 3, n);
JTi = zeros(3, n, n);
JRi = zeros(3, n, n);
dJTi = zeros(3, n, n);
dJRi = zeros(3, n, n);
wi = zeros(3, n+2);
vji = zeros(3, n);
vli = zeros(3, n);

% Initiate base spacecraft links
dR0 = [
  -cos(bs(5))*sin(bs(6))*bd(6) - cos(bs(6))*sin(bs(5))*bd(5), sin(bs(4))*sin(bs(6))*bd(4) - cos(bs(4))*cos(bs(6))*bd(6) + cos(bs(4))*cos(bs(6))*sin(bs(5))*bd(4) + cos(bs(6))*cos(bs(5))*sin(bs(4))*bd(5) - sin(bs(4))*sin(bs(6))*sin(bs(5))*bd(6), cos(bs(4))*sin(bs(6))*bd(4) + cos(bs(6))*sin(bs(4))*bd(6) - cos(bs(6))*sin(bs(4))*sin(bs(5))*bd(4) - cos(bs(4))*sin(bs(6))*sin(bs(5))*bd(6) + cos(bs(4))*cos(bs(6))*cos(bs(5))*bd(5)
  cos(bs(6))*cos(bs(5))*bd(6) - sin(bs(6))*sin(bs(5))*bd(5), cos(bs(4))*sin(bs(6))*sin(bs(5))*bd(4) - cos(bs(4))*sin(bs(6))*bd(6) - cos(bs(6))*sin(bs(4))*bd(4) + cos(bs(6))*sin(bs(4))*sin(bs(5))*bd(6) + cos(bs(5))*sin(bs(4))*sin(bs(6))*bd(5), sin(bs(4))*sin(bs(6))*bd(6) - cos(bs(4))*cos(bs(6))*bd(4) + cos(bs(4))*cos(bs(6))*sin(bs(5))*bd(6) + cos(bs(4))*cos(bs(5))*sin(bs(6))*bd(5) - sin(bs(4))*sin(bs(6))*sin(bs(5))*bd(4)
  -cos(bs(5))*bd(5), cos(bs(4))*cos(bs(5))*bd(4) - sin(bs(4))*sin(bs(5))*bd(5), - cos(bs(5))*sin(bs(4))*bd(4) - cos(bs(4))*sin(bs(5))*bd(5)                                                                                 
  ];

wi(:, 1) = [dR0(3, 2), dR0(1, 3), dR0(2, 1)]';
wi(:, 2) = wi(:, 1);

Ti(:, :, 1) = TFF(bh, dh, 1);
Ti(:, :, 2) = Ti(:, :, 1)*TFF(bh, dh, 2);

% Loop
for i = 1:n
    % Get next transformation matrix
    Ti(:, :, i+2) = Ti(:, :, i+1) * TFF(bh, dh, i+2);

    % Extract required mat
    T0i = Ti(:, :, i+2);
    
    % Get i-th CoM wrt sc CoM
    com0i(i, :) = T0i(1:3, 4) + T0i(1:3, 1:3)*com(i, :).'; % TODO: Check better

    % Init velocities
    vji(:, i) = bd(1:3) + skewMatrix(wi(:, 1))*(T0i(1:3, 4) - r0.');
    vli(:, i) = bd(1:3) + skewMatrix(wi(:, 1))*com0i(i, :).';

    % Compute angular velocity
    wi(:, i+2) = wi(:, i+1) + T0i(1:3 , 3)*qd(i);

    % Recover rotation matrix
    Rdi = skewMatrix(wi(:, i+2))*T0i(1:3, 1:3);

    % Compute Im in global frame
    Ii(:, :, i) = T0i(1:3, 1:3)*Im(:, :, i)*(T0i(1:3, 1:3).');
    
    % Recover inertia matrix derivative
    Id(:, :, i) = Rdi*Im(:, :, i)*T0i(1:3, 1:3).' + T0i(1:3, 1:3)*Im(:, :, i)*Rdi.';

    % Compute JTi and JRi
    JTit = zeros(3, n);
    JRit = zeros(3, n);
    dJTit = zeros(3, n);
    dJRit = zeros(3, n);
    for j = 1:i     
        % Extract required mat
        T0jp = Ti(:, :, j+1);
        T0j = Ti(:, :, j+2);  % Check which one is the right one

        % Update Jacobians
        JTit(:, j) = cross(T0jp(1:3, 3), com0i(i, :).' - T0jp(1:3, 4));
        JRit(:, j) = T0jp(1:3, 3);

        % Recover rotation matrix
        Rdj = skewMatrix(wi(:, j+2))*T0j(1:3, 1:3);

        % Update Jacobian derivatives
        dJTit(:, j) = skewMatrix(Rdj)*(com0i(i, :).' - T0jp(1:3, 4)) + skewMatrix(T0jp(1:3, 3))*(vli(:, i) - vji(:, j));
        dJRit(:, j) = Rdj(1:3, 3);

        % Update velocities
        vji(:, i) = vji(:, i) + skewMatrix(T0j(1:3, 3))*(T0i(1:3, 4) - T0j(1:3, 4))*qd(j);
        vli(:, i) = vli(:, i) + skewMatrix(T0j(1:3, 3))*((com0i(i, :).' + r0.') - T0j(1:3, 4))*qd(j);
    end
    
    % Append
    JTi(:, :, i) = JTit;
    JRi(:, :, i) = JRit;
    dJTi(:, :, i) = dJTit;
    dJRi(:, :, i) = dJRit;
end

%% H0, base spacecraft inertia matrix (6x6)
% Get position of system center of mass and Hs
link_contr = 0;
Hs = zeros(3, 3);
for i = 1:n
    % Get CoM0i
    link_contr = link_contr + m(i)*com0i(i, :);

    % Hs
    Hs = Hs + Ii(:, :, i) - m(i)*skewMatrix(com0i(i, :))*skewMatrix(com0i(i, :));
end

% CoM
rC = r0 + (1/mtot)*link_contr;
r0C = rC - r0;

% Hs
Hs = Hs + I0;  % TODO: Check correctness of frame I0

% Inertia matrix
H0 = [
    mtot*eye(3)            , -mtot*skewMatrix(r0C)
    mtot*skewMatrix(r0C)   ,  Hs
    ];

%% Hm, manipulator inertia matrix
% Init inertia matrix
Hm = zeros(n);

% Loop
for i = 1:n
    % Get matrices
    JTit = JTi(:, :, i);
    JRit = JRi(:, :, i);

    % Sum contribution
    Hm = Hm + m(i)*(JTit.')*JTit + (JRit.')*Ii(:, :, i)*JRit;
end

%% H0m, dynamic coupling inertia matrix
% Init sub-matrices
JTS = zeros(3, n);
Hsq = zeros(3, n);

% Loop
for i = 1:n
    % Get matrices
    JTit = JTi(:, :, i);
    JRit = JRi(:, :, i);

    % Sum contribution
    JTS = JTS + m(i)*JTit;
    Hsq = Hsq + Ii(:, :, i)*JRit + m(i)*skewMatrix(com0i(i, :))*JTit;
end

% Assemble matrix
H0m = [
    JTS
    Hsq
];

%% dot(Hm)
% Init matrix
dot_Hm = zeros(n);

% Update Hm
for i = 1:n
    dot_Hm = dot_Hm + (dJRi(:, :, i).')*Ii(:, :, i)*JRi(:, :, i) + ...
            (JRi(:, :, i).')*Id(:, :, i)*JRi(:, :, i) + ...
            (JRi(:, :, i).')*Ii(:, :, i)*dJRi(:, :, i) + ...
            m(i)*((dJTi(:, :, i).')*JTi(:, :, i) + (JTi(:, :, i).')*dJTi(:, :, i));
end

%% dot(H0)
dot_Hs = zeros(3, 3);

% Loop
ext_cont = 0;
for i = 1:n
    % Loop through j
    part_cont = 0;
    for j = 1:i
        % Get transformation matrix
        T0j = Ti(:, :, j+2);  % Check which one is the right one
        
        % Update
        part_cont = part_cont + skewMatrix(T0j(1:3, 3))*((com0i(i, :).' + r0.') - T0j(1:3, 4))*qd(j);
    end

    % Update
    ext_cont = ext_cont + m(i)*skewMatrix(skewMatrix(wi(:, 1))*com0i(i, :).' + part_cont);

    % dot_Hs
    v0i_sk = skewMatrix(vli(:, i) - bd(1:3));
    dot_Hs = dot_Hs + Id(:, :, i) - m(i)*(v0i_sk*skewMatrix(com0i(i, :)) + ...
        skewMatrix(com0i(i, :))*v0i_sk);
end

% Derivative of Spacecraft inertia
T0 = Ti(:, :, 1);
Rd0 = skewMatrix(wi(:, 1))*T0(1:3, 1:3);
dot_Hs = dot_Hs + (Rd0*I0*(T0(1:3, 1:3).') + T0(1:3, 1:3)*I0*(Rd0.'));

dot_H0 = [
    zeros(3, 3), -ext_cont
    ext_cont, dot_Hs
];

%% dot(H0m)
dJTS = zeros(3, n);
dHsq = zeros(3, n);

% Loop
for i = 1:n
    % link CoM vel wrt 0
    v0i_sk = skewMatrix(vli(:, i) - bd(1:3));
    
    % Update
    dJTS = dJTS + m(i)*dJTi(:, :, i);
    dHsq = dHsq + Id(:, :, i)*JRi(:, :, i) + Ii(:, :, i)*dJRi(:, :, i) + ...
        m(i)*(v0i_sk*JTi(:, :, i) + skewMatrix(com0i(i, :))*dJTi(:, :, i));
end

% Assemble
dot_H0m = [
    dJTS
    dHsq
];

%% c0
% Init
dxr0c = zeros(1, 3, 6);
dxHs = zeros(3, 3, 6);
dxJTS = zeros(3, n, 6);
dxHsq = zeros(3, n, 6);

dxH0 = zeros(6, 6, 6);
dxH0m = zeros(6, n, 6);
dxHm = zeros(n, n, 6);

c0 = zeros(6, 1);

% Get dH0
for i = 1:6 % derivative
    for j = 1:n % joint
        % Position of CoM
        dxr0c(:, :, i) = dxr0c(:, :, i) + m(j)*dxr0i(j, :, i);

        % dHs
        dxHs(:, :, i) = dxHs(:, :, i) + dxIi(:, :, j, i) - ...
            m(j)*(skewMatrix(dxr0i(j, :, i))*skewMatrix(com0i(j, :)) + ...
            skewMatrix(com0i(j, :))*skewMatrix(dxr0i(j, :, i)));

        % dJTS
        dxJTS(:, :, i) = dxJTS(:, :, i) + m(j)*dxJTi(:, :, j, i);

        % dHsq
        dxHsq(:, :, i) = dxHsq(:, :, i) + dxIi(:, :, j, i)*JRi(:, :, j) + ...
            Im(:, :, j)*dxJRi(:, :, j, i) + m(j)*(skewMatrix(dxr0i(j, :, i))*JTi(:, :, j) + ...
            com0i(j, :)*dxJTi(:, :, j, i));

        % dHm
        dxHm(:, :, i) = dxHm(:, :, i) + (dxJRi(:, :, j, i).')*Im(:, :, j)*JRi(:, :, j) + ...
            (JRi(:, :, j).')*dxIi(:, :, j, i)*JRi(:, :, j) + (JRi(:, :, j).')*Im(:, :, j)*dxJRi(:, :, j, i) + ...
            m(j)*(dxJTi(:, :, j, i).')*JTi(:, :, j) + m(j)*(JTi(:, :, j).')*dxJTi(:, :, j, i);

    end
    % Get dr0c
    dxr0c(:, :, i) = dxr0c(:, :, i)/mtot;

    % dH0
    dxH0(:, :, i) = [
        zeros(3, 3), -mtot*skewMatrix(dxr0c(:, :, i))
        mtot*skewMatrix(dxr0c(:, :, i)), dxHs(:, :, i) %% CHECK r0C!!! not complete
    ];

    % dHom
    dxH0m(:, :, i) = [
        dxJTS(:, :, i)
        dxHsq(:, :, i)
    ];

    % Get c0
    c0(i) = -0.5 * ((bd.')*dxH0(:, :, i)*bd + (qd.')*dxHm(:, :, i)*qd + ...
        (bd.')*dxH0m(:, :, i)*qd + (qd.')*(dxH0m(:, :, i).')*bd);
end

%% cm
% Init
dqr0c = zeros(1, 3, 6);
dqHs = zeros(3, 3, 6);
dqJTS = zeros(3, n, 6);
dqHsq = zeros(3, n, 6);

dqH0 = zeros(6, 6, 6);
dqH0m = zeros(6, n, 6);
dqHm = zeros(n, n, 6);

cm = zeros(n, 1);

% Get dH0
for i = 1:6 % derivative
    for j = 1:n % joint
        % Position of CoM
        dqr0c(:, :, i) = dqr0c(:, :, i) + m(j)*dqr0i(j, :, i);

        % dHs
        dqHs(:, :, i) = dqHs(:, :, i) + dqIi(:, :, j, i) - ...
            m(j)*(skewMatrix(dqr0i(j, :, i))*skewMatrix(com0i(j, :)) + ...
            skewMatrix(com0i(j, :))*skewMatrix(dqr0i(j, :, i)));

        % dJTS
        dqJTS(:, :, i) = dqJTS(:, :, i) + m(j)*dqJTi(:, :, j, i);

        % dHsq
        dqHsq(:, :, i) = dqHsq(:, :, i) + dqIi(:, :, j, i)*JRi(:, :, j) + ...
            Im(:, :, j)*dqJRi(:, :, j, i) + m(j)*(skewMatrix(dqr0i(j, :, i))*JTi(:, :, j) + ...
            com0i(j, :)*dqJTi(:, :, j, i));

        % dHm
        dqHm(:, :, i) = dqHm(:, :, i) + (dqJRi(:, :, j, i).')*Im(:, :, j)*JRi(:, :, j) + ...
            (JRi(:, :, j).')*dqIi(:, :, j, i)*JRi(:, :, j) + (JRi(:, :, j).')*Im(:, :, j)*dqJRi(:, :, j, i) + ...
            m(j)*(dqJTi(:, :, j, i).')*JTi(:, :, j) + m(j)*(JTi(:, :, j).')*dqJTi(:, :, j, i);

    end
    % Get dr0c
    dqr0c(:, :, i) = dqr0c(:, :, i)/mtot;

    % dH0
    dqH0(:, :, i) = [
        zeros(3, 3), -mtot*skewMatrix(dqr0c(:, :, i))
        mtot*skewMatrix(dqr0c(:, :, i)), dqHs(:, :, i) %% CHECK r0C!!! not complete
    ];

    % dHom
    dqH0m(:, :, i) = [
        dqJTS(:, :, i)
        dqHsq(:, :, i)
    ];

    % cm
    cm(i) = -0.5 * ((bd.')*dqH0(:, :, i)*bd + (qd.')*dqHm(:, :, i)*qd + ...
        (bd.')*dqH0m(:, :, i)*qd + (qd.')*(dqH0m(:, :, i).')*bd);
end


%% H, H_dot, and c
% H
H = [
    H0, H0m
    H0m.', Hm
];

% H_dot
H_dot = [
    dot_H0, dot_H0m
    dot_H0m.', dot_Hm
];

% c
c = [
    c0
    cm
];

end











