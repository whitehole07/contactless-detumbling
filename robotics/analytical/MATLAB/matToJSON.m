function matToJSON(filename, symMatrix)
%MATTOJSON Summary of this function goes here
%   Detailed explanation goes here
% Assume you have a symbolic matrix 'symMatrix' defined in MATLAB
% Here's a sample symbolic matrix for demonstration:

% Convert symbolic matrix to a cell array of strings
[row, col] = size(symMatrix);
symStrings = cell(row, col);
for i = 1:row
    for j = 1:col
        str = char(symMatrix(i, j));

        % Series of replacement
        % Define a cell array of word pairs to replace
        word_pairs = {
            't1(t)', 't1'; 
            't2(t)', 't2'; 
            't3(t)', 't3'; 
            't4(t)', 't4'; 
            't5(t)', 't5'; 
            't6(t)', 't6';
            'diff(t1, t)', 'dt1';
            'diff(t2, t)', 'dt2';
            'diff(t3, t)', 'dt3';
            'diff(t4, t)', 'dt4';
            'diff(t5, t)', 'dt5';
            'diff(t6, t)', 'dt6';
            };
        
        % Loop through each word pair and replace the words in the string
        for k = 1:size(word_pairs, 1)
            word_to_replace = word_pairs{k, 1};
            replacement_word = word_pairs{k, 2};
            str = strrep(str, word_to_replace, replacement_word);
        end

        % Save
        symStrings{i, j} = str;
    end
end

% Create a struct representing the symbolic matrix
jsonStruct = struct('symbolic_matrix', symStrings);

% Convert the struct to JSON string
jsonStr = jsonencode(jsonStruct);

% Write JSON string to a file
fid = fopen(filename, 'w');
if fid == -1
    error('Could not open file for writing');
end
fprintf(fid, '%s', jsonStr);
fclose(fid);

end

