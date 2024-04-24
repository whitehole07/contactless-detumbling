function plotMovement(parameters, angles_evolution)
    % parameters: DH parameters of the robotic arm (nx4 matrix)
    % angles_evolution: joint angles evolution (Nx2 matrix)
    % time: time vector for animation (Nx1 matrix)

    % Create a VideoWriter object with MPEG-4 codec
    writerObj = VideoWriter('animation.avi', 'Motion JPEG AVI'); 

    % Set the frame rate (adjust as needed)
    writerObj.FrameRate = 15;
    
    % Open the VideoWriter object
    open(writerObj);
    
    % Define constants
    num_joints = size(parameters, 1);
    num_frames = size(angles_evolution, 1);
    
    % Initialize figure and axis
    figure;
    ax = gca;
    hold(ax, 'on');
    axis(ax, 'equal');
    
    % Set axis limits
    xlim(ax, [-15 15]);
    ylim(ax, [-15 15]);
    zlim(ax, [-15 15]);
    grid on;
    
    % Initialize transformation matrices
    T = cell(1, num_joints);
    for i = 1:num_joints
        T{i} = eye(4);
    end
    
    % Animation loop
    for t = 1:num_frames
        % Update transformation matrices based on joint angles
        for i = 1:num_joints
            a = parameters(i, 2);
            d = parameters(i, 3);
            alpha = parameters(i, 4);
            theta = angles_evolution(t, i);
            
            % Denavit-Hartenberg transformation matrix
            T{i} = [cos(theta) -sin(theta)*cos(alpha) sin(theta)*sin(alpha) a*cos(theta);
                    sin(theta) cos(theta)*cos(alpha) -cos(theta)*sin(alpha) a*sin(theta);
                    0 sin(alpha) cos(alpha) d;
                    0 0 0 1];

            if i > 1; T{i} = T{i-1} * T{i}; end
        end
        
        % Plot robot arm links
        for i = 1:num_joints
            if i == 1
                link_start = [0 0 0];
            else
                link_start = T{i-1}(1:3, 4)';
            end
            link_end = T{i}(1:3, 4)';
            plot3(ax, [link_start(1) link_end(1)], [link_start(2) link_end(2)], [link_start(3) link_end(3)], 'b', 'LineWidth', 3);
        end
        
        % Plot robot arm joints
        plot3(0, 0, 0, 'go', 'MarkerSize', 5, 'MarkerFaceColor', 'g')
        for i = 1:num_joints
            joint_pos = T{i}(1:3, 4)';
            plot3(ax, joint_pos(1), joint_pos(2), joint_pos(3), 'ro', 'MarkerSize', 5, 'MarkerFaceColor', 'r');
        end
        
        % Update plot
            % Capture the frame
        view(50, 30); % Example: azimuthal angle of 30 degrees, elevation angle of 30 degrees
        frame = getframe(gcf);
        
        % Write the frame to the video
        writeVideo(writerObj, frame);
%             drawnow;
%             pause(0.05); % Adjust pause time as needed
        cla;
    end

    % Close the video writer
    close(writerObj);
end