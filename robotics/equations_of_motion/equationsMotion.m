% Find UR10 standard robotic arm equations of motion

%% Clean up
clc
close all
clear

%% Define parameters
syms t1(t) t2(t) t3(t) t4(t) t5(t) t6(t)

% Joint variables
q = [t1(t); t2(t); t3(t); t4(t); t5(t); t6(t)];
tau = [0; 0; 0; 0; 0; 0];

% Denavit-Hartenberg Parameters
dh = [
    t1(t), 0, 0.1807, pi/2
    t2(t), -0.6127, 0, 0
    t3(t), -0.57155, 0, 0
    t4(t), 0, 0.17415, pi/2
    t5(t), 0, 0.11985, -pi/2
    t6(t), 0, 0.11655, 0
    ];

% Masses
m = [7.369, 13.051, 3.989, 2.1, 1.98, 0.615];
com = [
    0.021, 0.000, 0.027
    0.38, 0.000, 0.158
    0.24, 0.000, 0.068
    0.000, 0.007, 0.018
    0.000, 0.007, 0.018
    0, 0, -0.026
];

% Moment of inertia
Im = zeros(3, 3, 6);
Im(:, :, 1) = [
    0.0341, 0.0000, -0.0043
    0.0000, 0.0353, 0.0001
    -0.0043, 0.0001, 0.0216
    ];

Im(:, :, 2) = [
    0.0281, 0.0001, -0.0156
    0.0001, 0.7707, 0.0000
    -0.0156, 0.0000, 0.7694
    ];

Im(:, :, 3) = [
    0.0101, 0.0001, 0.0092
    0.0001, 0.3093, 0.0000
    0.0092, 0.0000, 0.3065
    ];

Im(:, :, 4) = [
    0.0030, -0.0000, -0.0000
    -0.0000, 0.0022, -0.0002
    -0.0000, -0.0002, 0.0026
    ];

Im(:, :, 5) = [
    0.0030, -0.0000, -0.0000
    -0.0000, 0.0022, -0.0002
    -0.0000, -0.0002, 0.0026
    ];

Im(:, :, 6) = [
    0.0000, 0.0000, -0.0000
    0.0000, 0.0004, 0.0000
    -0.0000, 0.0000, 0.0003
    ];

% Degrees of freedom
n = size(dh, 1);

% Load matrices
load("C.mat")
load("D.mat")

%% Transformation matrix
% Generic transformation matrix between adjacent frames using DH convention
T = @(j) [ % theta, a, d, alpha
    cos(dh(j, 1)), -sin(dh(j, 1))*cos(dh(j, 4)), sin(dh(j, 1))*sin(dh(j, 4)), dh(j, 2)*cos(dh(j, 1))
    sin(dh(j, 1)), cos(dh(j, 1))*cos(dh(j, 4)), -cos(dh(j, 1))*sin(dh(j, 4)), dh(j, 2)*sin(dh(j, 1))
    0, sin(dh(j, 4)), cos(dh(j, 4)), dh(j, 3)
    0, 0, 0, 1
    ];

%% Inertia matrix
% % Init inertia matrix
% D = zeros(n);
% 
% % Loop
% for i = 1:n
%     % Jacobians link i
%     JPi = sym(zeros(3, n));
%     JOi = sym(zeros(3, n));
% 
%     % Position of CoM
%     T0i = eye(4); T0ip = eye(4); 
%     for j = 1:i; T0ip = T0i; T0i = T0i*T(j); end % Transformation matrices
%     pli = T0ip(1:3, 4) + T0i(1:3, 1:3) * com(i, :).';
%     
%     % Init transformation matrix
%     T0jp = eye(4);
%     for j = 1:i     
%         % Update Jacobians
%         JPi(:, j) = cross(T0jp(1:3, 3), pli - T0jp(1:3, 4));
%         JOi(:, j) = T0jp(1:3, 3);
% 
%         % Update transformation matrix
%         T0jp = T0jp*T(j);
%     end
% 
%     % Sum contribution
%     D = D + m(i)*(JPi.')*JPi + (JOi.')*T0i(1:3, 1:3)*Im(:, :, i)*(T0i(1:3, 1:3).')*JOi;
% end
% 
% % Save
% D = simplify(D);
% fprintf("Simplified D")
% save("D.mat", "D");
% matToTXT("D.json", D);

%% Christoffel symbols
% Init C tensor
% C = sym(zeros(n));
% for j = 1:n
%     for k = 1:n
%         % Init value
%         ckj = 0;
% 
%         % Summation of christoffels
%         for i = 1:n
%             ckj = ckj + 0.5 * (diff(D(k, j), q(i)) + diff(D(k, i), q(j)) - diff(D(i, j), q(k))) * diff(q(i), t);
%             ckj = simplify(ckj);
%             fprintf("Simplified C (%d, %d, %d)\n", i, j, k)
%         end
% 
%         % Assign value
%         C(k, j) = ckj;
%     end
% end
% 
% % Save
% save("C.mat", "C");
% matToTXT("C.json", C);

%% State Space
% % Initial parameters
tspan = [0, 200];
y0 = [0; -0.7; 0; 0; 0; 0; 0.1; 0; 0; 0; 0; 0]; %; 0; 0; 0; 0; 0.1; 0.1];

% Solve differential equation
options = odeset('RelTol', 1e-8, 'AbsTol', 1e-8); % Set relative tolerance to 1e-6
[time, y] = ode15s(@(t, y) stateEquation(t, y, D, C, q, tau, tspan), tspan, y0, options);

%% Animate
% % Plot evolution
plotMovement(dh, y(:, 1:n));

%% Plot
% Plot joint angles
figure;
time = linspace(tspan(1), tspan(end), size(y, 1));
plot(time, rad2deg(y(:, 1:n)), "linewidth", 2)
xlabel("Time [s]")
ylabel("Joint angle [deg]")
legend("$\theta_1$", "$\theta_2$", "$\theta_3$", "$\theta_4$", "$\theta_5$", "$\theta_6$", 'Interpreter', 'latex')
grid on
title("Evolution of Joint Angles")

% Plot joint velocities
figure;
time = linspace(tspan(1), tspan(end), size(y, 1));
plot(time, rad2deg(y(:, n+1:end)), "linewidth", 2)
xlabel("Time [s]")
ylabel("Joint angular velocity [deg/s]")
legend("$\dot{\theta}_1$", "$\dot{\theta}_2$", "$\dot{\theta}_3$", "$\dot{\theta}_4$", "$\dot{\theta}_5$", "$\dot{\theta}_6$", 'Interpreter', 'latex')
grid on
title("Evolution of Joint Angular Velocities")

%% Test
val = [0,-0.7,0,0,0,0,0.1,0,0,0,0,0];
double(subs(D, [t1(t) t2(t) t3(t) t4(t) t5(t) t6(t) diff(t1(t), t) diff(t2(t), t) diff(t3(t), t) diff(t4(t), t) diff(t5(t), t) diff(t6(t), t)], val))
double(subs(C, [t1(t) t2(t) t3(t) t4(t) t5(t) t6(t) diff(t1(t), t) diff(t2(t), t) diff(t3(t), t) diff(t4(t), t) diff(t5(t), t) diff(t6(t), t)], val))

%% Load custom
p = csvread("prop_result.csv", 1, 0);
t = p(:, 1);
p = p(:, 2:n+n+1);

% plotMovement(dh, p(:, 1:n));

% Plot joint angles
figure;
time = linspace(t(1), t(end), size(p, 1));
plot(time, rad2deg(p(:, 1:n)), "linewidth", 2)
xlabel("Time [s]")
ylabel("Joint angle [deg]")
legend("$\theta_1$", "$\theta_2$", "$\theta_3$", "$\theta_4$", "$\theta_5$", "$\theta_6$", 'Interpreter', 'latex')
grid on
title("Evolution of Joint Angles")

% Plot joint velocities
figure;
plot(time, rad2deg(p(:, n+1:end)), "linewidth", 2)
xlabel("Time [s]")
ylabel("Joint angular velocity [deg/s]")
legend("$\dot{\theta}_1$", "$\dot{\theta}_2$", "$\dot{\theta}_3$", "$\dot{\theta}_4$", "$\dot{\theta}_5$", "$\dot{\theta}_6$", 'Interpreter', 'latex')
grid on
title("Evolution of Joint Angular Velocities")
