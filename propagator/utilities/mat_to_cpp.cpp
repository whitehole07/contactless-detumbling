#include <iostream>
#include <fstream>

/* SymEngine */
#include <symengine/symengine_config.h>
#include <symengine/parser.h>
#include <symengine/expression.h>
#include <symengine/symbol.h>
#include <symengine/symengine_casts.h>
#include <symengine/matrix.h>

/* RapidJSON */
#include "rapidjson/document.h"

using namespace std;
using namespace SymEngine;
using namespace rapidjson;

DenseMatrix load_matrix_from_JSON(const string& filename);

void cpp_out(DenseMatrix mat, string name);

int main() {
    /* Load matrices */
    DenseMatrix D = load_matrix_from_JSON("/home/whitehole/PycharmProjects/master_thesis/robotics/equations_of_motion/D.json");
    DenseMatrix C = load_matrix_from_JSON("/home/whitehole/PycharmProjects/master_thesis/robotics/equations_of_motion/C.json");

    /* Out Matrices*/
    cpp_out(D, "D");
    cpp_out(C, "C");

    return 0;
}

void cpp_out(DenseMatrix mat, string name) {
    string str;
    for (size_t i = 0; i < 6; i++)
    {
        for (size_t j = 0; j < 6; j++)
        {
            string cppcode = ccode(*mat.get(i, j));
            str = str + "IJth(" + name + ", " + to_string(i) + ", " + to_string(j) + ") = " + cppcode + ";\n";
        }
        
    }

    // Open a file for writing
    ofstream outputFile(name + "_cpp.txt");

    // Check if the file is opened successfully
    if (outputFile.is_open()) {
        // Write the string to the file
        outputFile << str;
        
        // Close the file
        outputFile.close();

        cout << "String saved to " + name + "_cpp.txt"<< endl;
    } else {
        cerr << "Error: Unable to open file for writing." << endl;
        return; // Return error code
    }
    return;
}

DenseMatrix load_matrix_from_JSON(const string& filename) {
    // Open the JSON file
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    // Parse the JSON document
    Document doc;
    string jsonContent((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
    doc.Parse(jsonContent.c_str());

    // Check if parsing succeeded
    if (doc.HasParseError()) {
        cerr << "Error: Parse error in JSON file " << filename << " at position " << doc.GetErrorOffset() << endl;
        exit(1);
    }

    // Check if the JSON document is an array
    if (!doc.IsArray()) {
        cerr << "Error: JSON file " << filename << " does not contain an array" << endl;
        exit(1);
    }

    // Get matrix dimensions
    size_t rows = doc.Size();
    size_t cols = doc[0].Size();

    // Initialize DenseMatrix
    DenseMatrix matrix(rows, cols);

    for (size_t i = 0; i < rows; ++i) {
    const Value& row = doc[i];
        for (size_t j = 0; j < row.Size(); ++j) {
            const Value& entry = row[j];
            if (entry.HasMember("symbolic_matrix") && entry["symbolic_matrix"].IsString()) {
                auto parsed_expr = parse(entry["symbolic_matrix"].GetString());
                matrix.set(i, j, parsed_expr);
            } else {
                cout << "Error: Entry at row " << i << ", column " << j << " does not contain a valid symbolic_matrix string." << endl;
                // Handle this case appropriately
            }
        }
    }
    return matrix;
}