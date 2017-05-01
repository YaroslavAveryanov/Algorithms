#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <iomanip>
#include <vector>
#include <string>
using namespace std;

const int INFINITY = 999999;

struct st_arguments {
	int a;
	int b;
	int c;
	string d;
};

struct st_elements_fine {
	int i;
	int j;
	int fine;

	st_elements_fine(st_arguments arg) {
		i = arg.a;
		j = arg.b;
		fine = arg.c;
	}
};

struct st_edge {
    int i;
    int j;
    string type;

	st_edge(st_arguments arg) {
		i = arg.a;
		j = arg.b;
		type = arg.d;
	}
};

void PrintPath(vector<st_edge> edges) {
	for (size_t i = 0; i < edges.size(); i++)
		printf("(%d - %d) ", edges[i].i, edges[i].j);
	printf("\n");
}

void PrintPath_result(vector<st_edge> edges, int city, FILE * fd1) {
	for (size_t i = 0; i < edges.size(); i++) {
		if ((edges[i].i == city) && (edges[i].j != 0)) {
			fprintf(fd1,"%d->",edges[i].i + 1);
			PrintPath_result(edges, edges[i].j, fd1);
		}
		if ((edges[i].i == city) && (edges[i].j == 0)) { 
			fprintf(fd1, "%d->%d\n", edges[i].i + 1, edges[i].j + 1);
		}
	}    
	fclose (fd1);
}

void PrintMatrix(vector<vector<int> > matrix) {
    for(size_t i = 0; i < matrix.size(); i++) {
        for(size_t j = 0; j < matrix[i].size(); j++) {
            if (matrix[i][j] == INFINITY)
                cout << setw(7) << -1 << ' ';
            else
                cout << setw(7) << matrix[i][j] << ' ';
        }
        cout << "\n";
    }
	cout << "\n";
}

int min_in_string(vector<int> *a, int N) {
    int min;
    min = INFINITY;
    for (size_t i = 1; i < N-1; i++)
        if ((*a)[i] < min)
            min = (*a)[i];
    return min;
}

int min_in_string(vector<int> *a, int k, int N) {
	int min;
    min = INFINITY;
    for (size_t i = 1; i < N-1; i++)
        if (((*a)[i] < min) && (i != k))
            min = (*a)[i];
    return min;
}

int min_in_column(vector<vector<int> > *a, int j, int N) {
    int min;
    min = INFINITY;
    for (size_t i = 1; i < N-1; i++)
        if ((*a)[i][j] < min)
            min = (*a)[i][j];
    return min;
}

int min_in_column(vector<vector<int> > *a, int j, int k, int N) {
    int min;
    min = INFINITY;
    for (size_t i = 1; i < N-1; i++)
        if (((*a)[i][j] < min) && (i != k))
            min = (*a)[i][j];
    return min;
}

void Cycle (vector<vector<int> > *matrix, int i_close, int j_close, vector<vector<int> > *matrixFC, size_t N) {
	for (size_t i = 1; i < N - 1; i++) {
		for (size_t j = 1; j < N - 1; j++) {
			if (((*matrixFC)[i][j] != INFINITY) && (i != i_close) && (j != j_close) && ((*matrixFC)[i][j_close] == INFINITY) && ((*matrixFC)[i_close][j] == INFINITY) ) {
				(*matrix)[i][j] = INFINITY;
				(*matrixFC)[i][j] = INFINITY;
			}
		}
	}
}

int CreateMatrix(vector<vector <int> > *matrix, char *fileName) {
	FILE * fd;
    char c;
    int num;
	vector<int> temp;
	fd = fopen (fileName, "rt");
    if (fd == NULL) return 2;
    else
    {   
		char c1;
        while ((c = getc(fd)) != EOF)
        {
            switch(c) {
                case '\n':
                    temp.push_back(0);
					matrix->push_back(temp);
                    temp.clear();
					c1 = c;
                    break;
                case '-': 
                    temp.push_back(INFINITY); 
					c1 = c;
                    break;
                case ' ': 
					break;
				case '\t':
                    break;
                default: 
                    ungetc (c,fd); 
                    fscanf(fd, "%d", &num);
                    temp.push_back(num);
                    break;
            }
        } 
        fclose (fd);
		if( c1 != '\n' ) {
			temp.push_back(0);
			matrix->push_back(temp);
            temp.clear();
		}

	}
	for (size_t i = 0; i <= matrix->size(); i++)
		temp.push_back(0);
	matrix->push_back(temp);
	temp.clear();

	// добавл€ем в таблицу номер строки и номер столбца
	vector<int>::iterator it_st;
	vector<vector<int> >::iterator it_col;
	size_t N = matrix->size();
	for (size_t i = 0; i < N; i++) {
		it_st = (*matrix)[i].begin();
		if (i != N - 1)
			(*matrix)[i].insert(it_st, i);
		else
			(*matrix)[i].insert(it_st, -1);
	}
	temp.push_back(-2);
	for (size_t i = 0; i < N - 1; i++)
		temp.push_back(i);
	temp.push_back(-1);
	it_col = matrix->begin();
	matrix->insert(it_col, temp);
	temp.clear();

	return 0;
}

void FindPath (vector<vector<int> > matrix, int costPath, vector<st_edge> edges, int *cost_result_p, vector<st_edge> *result_path, vector<vector<int> > matrix_for_cycle) {
	int min;
	int cost_path_local = 0;
	size_t N = matrix.size();
	vector<st_elements_fine> fines;
	st_arguments arg;
	vector<vector<int> > matrix_FC_copy = matrix_for_cycle;

	//редуцирую матрицу
	for (size_t i = 1; i < N-1; i++) {
        min = min_in_string(&matrix[i], N);
        if (min != INFINITY)
            for (size_t j = 1; j < N-1; j++) 
                if (matrix[i][j] != INFINITY)
					matrix[i][j] -= min;
		matrix[i][N-1] = min;
    }
	for (size_t i = 1; i < N-1; i++) {
		min = min_in_column(&matrix, i, N);
        if (min != INFINITY)
			for (size_t j = 1; j < N-1; j++)
			    if (matrix[j][i] != INFINITY)
					matrix[j][i] -= min;
		matrix[N-1][i] = min; 
	}
    int total = 0;
	int min_st, min_col;
	for (size_t i = 1; i < N-1; i++)
		if (matrix[N-1][i] != INFINITY)
			total += matrix[N-1][i];
	for (size_t i = 1; i < N-1; i++)
		if (matrix[i][N-1] != INFINITY)
			total += matrix[i][N-1];
	matrix[N-1][N-1] = total;
	// оновл€ем стоимость
	cost_path_local = total + costPath;
	
	// создаю массив штрафов
	int flag_erase = false;
	for (size_t i = 1; i < N - 1; i++) {
		for (size_t j = 1; j < N - 1; j++) {
			if (matrix[i][j] == 0) {
				min_st = min_in_string(&matrix[i], j, N);
				min_col = min_in_column(&matrix, j, i, N);
				flag_erase = true;
				arg.a = matrix[i][0];
				arg.b = matrix[0][j];
				if ((min_st == INFINITY) || (min_col == INFINITY))
					arg.c = INFINITY;
				else
					arg.c = min_st + min_col;
				fines.push_back(arg);
			}
		}
	}
	// ищем максимальный штраф
	if (flag_erase == true) {
		int max_fine = -1;
		for (size_t i = 0; i < fines.size(); i++)
			if ((fines[i].fine > max_fine) && (fines[i].fine != INFINITY))
				max_fine = fines[i].fine;
		if (max_fine == -1)
			max_fine = INFINITY;
		// нашли
		//добавл€ю в массив ребер ребро с мак штрафом
		for (size_t i = 0; i < fines.size(); i++)
			if (fines[i].fine == max_fine) {
				arg.a = fines[i].i;
				arg.b = fines[i].j;
				arg.d = "yes";
				edges.push_back(arg);
				break;
			}

		vector<vector<int> > matrix_local;
		vector<st_edge> edges_local;
		vector<int> path_int_local;
		matrix_local = matrix;
		edges_local = edges;

		int j_for_erase;
		int i_for_erase;
		for (size_t j = 1; j < N; j++)
			if (matrix_local[0][j] == edges[edges.size()-1].j) {
					j_for_erase = j;
					break;
				} 
		for (size_t i = 1; i < N - 1; i++)
			if (matrix_local[i][0] == edges[edges.size()-1].i) {
				i_for_erase = i;
				break;
			}

		// ищем и удал€ем подциклы
		if (N > 4)
			Cycle(&matrix_local, i_for_erase, j_for_erase, &matrix_for_cycle, N);

		// теперь "удал€ем строку и столбец"
		matrix_local[N-1].erase(matrix_local[N-1].begin() + N - 1);  // удал€ю элемент [N-1][N-1]
		matrix_local[0].erase(matrix_local[0].begin() + j_for_erase);
		matrix_for_cycle[N-1].erase(matrix_for_cycle[N-1].begin() + N - 1);
		matrix_for_cycle[0].erase(matrix_for_cycle[0].begin() + j_for_erase);
		for (size_t i = 1; i < N - 1; i++) {
			matrix_local[i].erase(matrix_local[i].begin() + j_for_erase);
			matrix_for_cycle[i].erase(matrix_for_cycle[i].begin() + j_for_erase);
		}
		matrix_local.erase(matrix_local.begin() + i_for_erase);
		matrix_for_cycle.erase(matrix_for_cycle.begin() + i_for_erase);

		if (N > 3) {
			if ((*cost_result_p != INFINITY) && (cost_path_local > *cost_result_p)) 
				;
			else
				FindPath(matrix_local, cost_path_local, edges, cost_result_p, result_path, matrix_for_cycle);
		}
		else {
			if (cost_path_local < *cost_result_p) {
				*cost_result_p = cost_path_local;
				*result_path = edges;
			}
			return;
		}

		int cost_left_node = cost_path_local + max_fine;
		if (cost_left_node > INFINITY)
			cost_left_node = INFINITY;

		if (*cost_result_p > cost_left_node) {
			matrix_local = matrix;
			matrix_for_cycle = matrix_FC_copy;
			matrix_local[i_for_erase][j_for_erase] = INFINITY;
			edges.pop_back();
			if (edges.size() != N - 1)
				FindPath(matrix_local, cost_path_local, edges, cost_result_p, result_path, matrix_for_cycle);
			else
				return;
		}
		return;
	}
	else {
		cost_path_local = INFINITY;
		return;
	}
}

int main(int argc, char *argv[]) {
    vector<vector <int> > matrix;
	vector<st_edge> result_path;
	int cost_result_path = INFINITY;

	if( argv[1] == NULL || argv[2] == NULL )
		return 2;

    if(CreateMatrix(&matrix, argv[1]))
		return 2;
	//создадим матрицу дл€ циклов
	vector<vector <int> > matrix_f_c = matrix;
	for (size_t i = 1; i < matrix.size() - 1; i++)
		for (size_t j = 1; j < matrix.size() - 1; j++)
			if ((i != j) && (matrix_f_c[i][j] == INFINITY))
				matrix_f_c[i][j] = 1;

    int costPath = 0;
    vector<st_edge> edges;
    FindPath(matrix, costPath, edges, &cost_result_path, &result_path, matrix_f_c);

	FILE * file_out;
	file_out = fopen (argv[2], "wt");
	
    if (file_out == NULL) return 2;
	else {
		fprintf(file_out,"%d\n",cost_result_path);
		PrintPath_result(result_path, 0, file_out);	
	}

	return 0;
}  