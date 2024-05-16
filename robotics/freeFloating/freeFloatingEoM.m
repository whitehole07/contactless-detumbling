%% Clean up
clc
close all
clear

%% Base spacecraft paramters
syms x(t) y(t) z(t) phi(t) theta(t) psi(t)

% Variables
b = [x(t) y(t) z(t) phi(t) theta(t) psi(t)].';
f_base = [0; 0; 0; 0; 0; 0];

% Joint 0
m_sc = 1000;                         % [kg] Mass of the chaser spacecraft
r0 = [x(t), y(t), z(t)];             % [m]  Position of chaser spacecraft's CoM in inertial frame (0,0,0)
cube_size = 3;                       % [m]  Cube side length
I0 = ((m_sc/6)*cube_size^2)*eye(3);  % [kg*m^2] Inertia matrix of base spacecraft

% Joint 1
rot_angl_1 = [0, 0, 0];    % [rad] [eps, xi, eta] rotation angles from joint fram 0 to 1
b0 = [0, 0, cube_size/2];  % [m]   Joint 1 position in joint 0 frame

% Base variables
bh = [r0 phi(t) theta(t) psi(t)
      b0 rot_angl_1];

%% Manipulator parameters
syms t1(t) t2(t) t3(t) t4(t) t5(t) t6(t)
% syms scale
scale = 10;

% Joint variables
q = [t1(t); t2(t); t3(t); t4(t); t5(t); t6(t)];
tau = [0; 0; 0; 0; 0; 0];

% Denavit-Hartenberg Parameters
dh = [ % theta, a, d, alpha
    t1(t),   0,  0.5, pi/2
    t2(t), 2.5,    0, 0
    t3(t), 2.5,    0, 0
    t4(t),   0,  0.5, pi/2
    t5(t),   0, 0.25, -pi/2
    t6(t),   0, 0.25, 0
    ];

% Yoshida et al., 1993
m = [5 50 50 10 5 5];

com = [
    0, -dh(1, 3)/2, 0
    dh(2, 2)/2, 0, 0
    dh(3, 2)/2, 0, 0
    0, -dh(4, 3)/2, 0 
    0, dh(5, 3)/2, 0
    0, 0, -dh(6, 3)/2
];

% Moment of inertia
Im = zeros(3, 3, 6);
Im(:, :, 1) = diag([0.0063 0.0292 0.0292]);
Im(:, :, 2) = diag([0.0625 26.1000 26.1000]);
Im(:, :, 3) = diag([0.0625 26.1000 26.1000]);
Im(:, :, 4) = diag([0.0125 0.2150 0.2150]);
Im(:, :, 5) = diag([0.0063 0.0292 0.0292]);
Im(:, :, 6) = diag([0.0292 0.0292 0.0063]);

% Degrees of freedom
n = size(dh, 1);

% Get inertia matrices
% [dxr0i, dqr0i, dxIi, dqIi, dxJTi, dqJTi, dxJRi, dqJRi] = getInertiaMat(b, q, bh, dh, com, Im);
load("dxr0i.mat");
load("dqr0i.mat");
load("dxIi.mat");
load("dqIi.mat");
load("dxJTi.mat");
load("dqJTi.mat");
load("dxJRi.mat");
load("dqJRi.mat");

%% Integrate
tspan = [0, 2];
y0 = [5; 0; 0; 0; 0; 0;  % x, y, z, phi, theta, psi
      0; 0.5; 0.5; 0; 0; 0;  % t1, t2, t3, t4, t5, t6
      0; 0; 0; 0; 0; 0;  % vx, vy, vz, phi_d, theta_d, psi_d
      0; 0; 0; 0; 0.02; 0   % dt1, dt2, dt3, dt4, dt5, dt6
];

% Solve differential equation
options = odeset('RelTol', 1e-2, 'AbsTol', 1e-4); % Set relative tolerance to 1e-6
[time, y] = ode15s( ...
    @(t, y) stateEqFF(t, y, q, b, bh, dh, m, com, Im, m_sc, I0, ...
    dxr0i, dqr0i, dxIi, dqIi, dxJTi, dqJTi, dxJRi, dqJRi, [tau; f_base], tspan), ...
    tspan, y0, options);

%% Plot
% Plot joint angles
figure
plot(time, rad2deg(y(:, 6+1:6+n)), "linewidth", 2)
xlabel("Time [s]")
ylabel("Joint angle [deg]")
legend("$\theta_1$", "$\theta_2$", "$\theta_3$", "$\theta_4$", "$\theta_5$", "$\theta_6$", 'Interpreter', 'latex')
grid on
title("Evolution of Joint Angles")

% Plot joint velocities
figure;
plot(time, rad2deg(y(:, 6+n+6+1:end)), "linewidth", 2)
xlabel("Time [s]")
ylabel("Joint angular velocity [deg/s]")
legend("$\dot{\theta}_1$", "$\dot{\theta}_2$", "$\dot{\theta}_3$", "$\dot{\theta}_4$", "$\dot{\theta}_5$", "$\dot{\theta}_6$", 'Interpreter', 'latex')
grid on
title("Evolution of Joint Angular Velocities")





