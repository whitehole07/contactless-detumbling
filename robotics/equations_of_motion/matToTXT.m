function matToTXT(matrix, filename)
    % Open the file for writing
    fid = fopen(filename, 'w');
    if fid == -1
        error(['Unable to open file ', filename, ' for writing']);
    end

    % Get the dimensions of the matrix
    [rows, cols] = size(matrix);

    % Write each element of the matrix to the file
    for i = 1:rows
        for j = 1:cols
            % Convert the symbolic expression to a string and write to file
            str = char(matrix(i, j));

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
            fprintf(fid, '%s', str);
            
            % Add comma if not the last element in the row
            if j < cols
                fprintf(fid, ',');
            end
        end
        
        % Add semicolon to separate different rows
        if i < rows
            fprintf(fid, ';\n');
        end
    end

    % Close the file
    fclose(fid);
end
