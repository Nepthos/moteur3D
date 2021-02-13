//
// Created by Adrien on 04/02/2021.
//

#include "Matrix.h"
void newline() {
    std::cout << std::endl;
}
Matrix::Matrix(int nb_lines, int nb_cols) {
    lines = nb_lines;
    cols = nb_cols;

    //init matrix with zeros
    for(int i = 0 ; i < nb_lines ; i ++) {
        std::vector<float> line;
        for(int j = 0 ; j < nb_cols ; j ++) {
            line.push_back(0.0f);
        }
        matrix.push_back(line);
        line.clear();
    }
}

Matrix::Matrix(int nb_tot) {
    lines = cols = nb_tot;
    for(int i = 0 ; i < nb_tot ; i ++) {
        std::vector<float> line;
        for(int j = 0 ; j < nb_tot ; j ++) {
            if(i==j) line.push_back(1.0f); // if same col and line, push 1 to create diagonal of identity matrix
            else line.push_back(0.0f); // else 0
        }
        matrix.push_back(line);
        line.clear();
    }
}
// get a column of the matrix
std::vector<float> Matrix::get_col(int idx) {
    check_indexes(0,idx);
    std::vector<float> result;
    for(int i = 0 ; i < matrix.size() ; i ++) {
        result.push_back(matrix.at(i).at(idx));
    }
    return result;
}

// get a line of the matrix
std::vector<float> Matrix::get_line(int idx) {
    check_indexes(idx,0);
    return matrix.at(idx);
}

// get a specific value of the matrix
float Matrix::get_at(int line, int col) {
    check_indexes(line,col);
    return matrix.at(line).at(col);
}

// set a specific value of the matrix
void Matrix::set_at(int line, int col, float value) {
    check_indexes(line,col);
    matrix.at(line).at(col) = value;
}

// check if an operation is not outside of the matrix
void Matrix::check_indexes(int line, int col) {
    if(line >= lines || col >= cols || line < 0 || col < 0) {
        std::cerr << "Given values are outside of the matrix size (size : " << lines << "," << cols << " given " << line << "," << col << ")" <<std::endl;
        exit(-1);
    }
}



void Matrix::print() {
    for(int i = 0 ; i < matrix.size() ; i ++) {
        for(int j = 0 ; j < matrix.at(0).size() ; j ++) {
            std::cout << matrix.at(i).at(j) << "\t";
        }
        std::cout << std::endl;
    }
}

VectFloat Matrix::getVect(int idx) {
    check_indexes(3, idx);
    return {matrix.at(0).at(idx), matrix.at(1).at(idx), matrix.at(2).at(idx)};
}

Vect4Float Matrix::getVect4(int idx) {
    check_indexes(3, idx);
    return {matrix.at(0).at(idx), matrix.at(1).at(idx), matrix.at(2).at(idx), matrix.at(3).at(idx)};
}
Matrix Matrix::transpose() {
    Matrix transposed(cols,lines);
    for(int i = 0 ; i < matrix.size() ; i ++) {
        for(int j = 0 ; j < matrix.at(0).size() ; j ++) {
            transposed.set_at(j, i, this->get_at(i,j));
        }
    }
    return transposed;
}

// extracts a submatrix of -1 dim without col and line
Matrix Matrix::submatrix(Matrix m, int line, int col) {
    Matrix submatrix(m.lines - 1, m.cols-1);
    int cindex = 0, lindex = 0;
    for(int i = 0 ; i < m.lines ; i++) {
        if(i != line) {
            for(int j = 0 ; j < m.cols ; j++) {
                if(j != col) {
                    submatrix.set_at(lindex, cindex, m.get_at(i,j));
                    cindex++;
                }
            }
            lindex++;
            cindex = 0;
        }
    }
    return submatrix;
}


//cofactor matrix for a given matrix is the determinant of the submatrix of each item of the matrix
Matrix Matrix::cofactor(Matrix m) {
    Matrix cofactor(m.lines, m.cols);
    int sign = 1;
    for(int i = 0 ; i < m.lines ; i++) {
        for(int j = 0 ; j < m.cols ; j++) {
            sign = (i+j)%2? -1: 1;
            cofactor.set_at(i,j, (sign * Matrix::determinant(submatrix(m,i,j))));
        }
    }
    return cofactor;
}
//recursive function to calculate the determinant of a n-size matrix
float Matrix::determinant(Matrix m) {
    float curr_det = 0;
    std::vector<float> first_line = m.get_line(0); // get the first line of the matrix
    int sign = 1; // current sign for the operation (will be changed at each iteration)

    if(m.cols == 1) { // if matrix size si 1 by 1, determinant is sign of this element
        return m.get_at(0,0);
    }
    else if(m.cols == 2) {// if matrix a b c d size is 2 by 2, determinant is ad - bc
        return m.get_at(0,0) * m.get_at(1,1) - m.get_at(0,1) * m.get_at(1,0);
    } else { // general case for n-size matrix
        for(int i = 0 ; i < m.lines ; i ++) {
                Matrix submatrix = Matrix::submatrix(m, 0,i); // gets the submatrix for the recursive call
                curr_det += (sign * m.get_at(0,i) * determinant(submatrix)); // adds current determinant
                sign = -sign; // switchs the sign for the next iteration
            }
    }
    return curr_det;
}


//function to gets the inverse of a given matrix
Matrix Matrix::invert(Matrix m) {
    float det; // variable for the matrix determinant
    Matrix inverse(m.lines, m.cols); // creating the inverted matrix
    if(m.lines != m.cols) { // if non squared matrix, error
        std::cerr << "Cannot invert non squared matrix" << std::endl;
        exit(-1);
    } else if((det = determinant(m)) == 0) { // if determinant is 0, cannot resolve, error
        std::cerr << "Cannot resolve matrix if determinant equals 0" << std::endl;
        exit(-1);
    } else {
        Matrix m_adj = cofactor(m.transpose()); // getting the cofactor matrix of the transposed given matrix
         for(int i = 0 ; i < m.lines ; i++) {
            for(int j = 0 ; j < m.lines ; j++) {
                inverse.set_at(i,j, m_adj.get_at(i,j) / det); // each value of the inverted matrix equals the value of the cofactor matrix divided by the determinant
            }
        }
    }
    return inverse;
}



Matrix Matrix::operator*(Matrix e) {
    Matrix new_mat(lines, e.cols);
    if(cols == e.lines) {
     for(int line = 0 ; line < lines ; line++) {
         for(int col = 0 ; col < e.cols ; col++) {
             float value = 0.f;
             for(int curr_col = 0 ; curr_col < cols ; curr_col++) {
                 value += matrix.at(line).at(curr_col) *  e.get_at(curr_col,col);
             }
             new_mat.set_at(line,col, value);
         }
     }
    } else {
        std::cerr << "Cannot multiply matrix with not the same number of lines and cols" << std::endl;
        exit(-1);
    }
    return new_mat;
}
