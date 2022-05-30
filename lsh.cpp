#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <fstream>
#include <cmath>

#include<cstring>
using namespace std;

#define SIGNATURE_DIM 30
#define DOC_NUM 1000


vector<vector<float>> read_matrix()
{
	ifstream csv_file("jaccard_sim.csv");
	vector<vector<float>> sim_matrix;
	string line;
	if (!csv_file.is_open())
		{ cout << "Could not open csv file!" << endl; exit(1);}
	while(getline(csv_file, line))
	{
		istringstream sin(line);
		vector<float> data_line;
		string field;
		while(getline(sin, field, ','))
		{
			data_line.push_back((float)atof(field.c_str()));
		}
		sim_matrix.push_back(data_line);
	}
	return sim_matrix;
}


void LSH(int b, int r) {
    unsigned int signature[SIGNATURE_DIM][DOC_NUM];

    double **result = new double *[DOC_NUM];
    for (int i = 0; i < DOC_NUM; i++) {
        result[i] = new double[DOC_NUM];
    }

    bool **crash = new bool *[DOC_NUM];
    for (int i = 0; i < DOC_NUM; i++) {
        crash[i] = new bool[DOC_NUM];
    }


    for (int i = 0; i < DOC_NUM; i++) {
        for (int j = 0; j < DOC_NUM; j++) {
            result[i][j] = 0;
        }
    }

    for (int i = 0; i < DOC_NUM; i++) {
        for (int j = 0; j < DOC_NUM; j++) {
            crash[i][j] = false;
        }
    }

    vector<vector<float>> sim_matrix = read_matrix();
    for (int t = 0; t < 1000; t++) {
        if (t % 10 == 0) {
            cout << "progress: " << t << "/1000" << endl;
        }

        char sig_file[20];
		sprintf(sig_file, "signatue_mat/%d", t);
        FILE* fp = fopen(sig_file, "r");
        fread(signature, sizeof(unsigned int), SIGNATURE_DIM * DOC_NUM, fp);

        set<pair<int, int>> s;  // 保存当前签名矩阵中发生碰撞的文档对
        
        for (int i = 0; i < DOC_NUM; i++) {
            for (int j = 0; j < DOC_NUM; j++) {
                crash[i][j] = false;
            }
        }

        for (int i = 0; i < b; i++) {
            vector<string> v; // 1000篇文档的每个band分量转化为字符串   
            for (int j = 0; j < DOC_NUM; j++) { 
                string str = "";
                for (int k = i * r; k < (i + 1) * r; k++) {
                    str.append(to_string(signature[k][j]));
                }
                v.push_back(str);
            }

            // 找出发生碰撞的文档对
            for (int j = 0; j < v.size(); j++) {
                for (int k = j + 1; k < v.size(); k++) {
                    if (v[j] == v[k]) {
                        crash[j][k] = true;
                        crash[k][j] = true;
                    }
                } 
            }
        }

        // // 更新碰撞次数
        for (int i = 0; i < DOC_NUM; i++) {
            for (int j = i + 1; j < DOC_NUM; j++) {
                if (crash[i][j]) {
                    result[i][j] += 1;
                    result[j][i] += 1;
                }
            }
        }
    }

    ofstream result_csv;
    result_csv.open("F:\\min_hash-main\\min_hash-main\\result.csv", ios::out);
    for (int i = 0; i < DOC_NUM; i++) {
        for (int j = i + 1; j < DOC_NUM; j++) {
            // cout << "(" << i << "," << j << ") ";
            // cout << "probablity = " << result[i][j] / 10 << "% ";
            // cout << "similarity = " << sim_matrix[i][j] * 100 << "% ";
            // cout << "prediction = " << (1 - pow(1 - pow(sim_matrix[i][j], r), b)) * 100 << "%" << endl;
            result_csv << sim_matrix[i][j] << ",";
            result_csv << result[i][j] / 1000 << endl;
        }
    }
    result_csv.close();
    
}



int main() {
    int b = 5;
    int r = 6;

    LSH(b, r);

    return 0;
}