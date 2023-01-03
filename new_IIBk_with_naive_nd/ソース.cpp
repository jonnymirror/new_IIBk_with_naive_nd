#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <intrin.h>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>  

using namespace std;

//�萔�J�n.
clock_t start;
vector<vector<long long int>> G;//���͂����O���t.
long long int n;//���_��.
long long int m;//�}��.
long long int k;//�p�����[�^(�ŏI�I�Ɋ������钸�_�̐��̍ő�l).
long long int l;//�p�����[�^(�Ɖu���������钸�_�̐��̍ő�l).
vector<long long int> T;//�֐�.
long long int x = 5;//�n�b�V���l�̌v�Z�ɗp����hash-multiplier.
long long int mod;//�n�b�V���l�̌v�Z�ɗp����mod(���_���ɂ���ĕω�����).
long long int a = 5;//mod�̌v�Z�ɗp����萔.
vector<long long int> Place_of_vertices;//���_i��������^�C�v�p�[�e�B�V������\���z��,Place_of_vertices[i]=-1�̎��͂��̒��_�͂܂��ǂ̃^�C�v�p�[�e�B�V�����ɂ������Ă��Ȃ����Ƃ�\��,Place_of_vertices[i]=j�̎��͒��_i���^�C�v�p�[�e�B�V����j�ɑ����Ă��邱�Ƃ�\��.
vector<vector<long long int>> Type_partitions;//�^�C�v�p�[�e�B�V����.
long long int nd = -1;//�O���t�̋ߖT���l���̒l���i�[����ϐ�.
long long int numbers_of_induced_subgraph_vertices;//�U�������O���t�̒��_��.
vector<vector<long long int>> Induced_subgraph;//�U�������O���t.
//�萔�I��.

//2�̒��_u,v�̋ߖT���r����֐�(�O���t�̗אڃ��X�g���l�̏���������sort���Ă���O��őO�����r���Ă���).
bool check_neighbor(long long int u, long long int v) {
	//cout << "check_neighbor�J�n" << endl;
	//cout << "u:" << u << "  v:" << v << endl;
	//�O���t�̗אڃ��X�g�m�F.
	/*
	for (long long int i = 0; i < G.size(); i++) {
		for (long long int j = 0; j < G[i].size(); j++) {
			cout << G[i][j] << " ";
		}
		cout << endl;
	}
	*/
	vector<long long int> U_neighbourhood;//u�̋ߖT(v�͎�菜��).
	vector<long long int> V_neighbourhood;//v�̋ߖT(u�͎�菜��).
	for (long long int i = 0; i < G[u].size(); i++) {
		if (G[u][i] != v)U_neighbourhood.push_back(G[u][i]);
	}
	for (long long int i = 0; i < G[v].size(); i++) {
		if (G[v][i] != u)V_neighbourhood.push_back(G[v][i]);
	}
	/*
	cout << "U_neighbourhood" << endl;
	for (long long int i = 0; i < U_neighbourhood.size(); i++) {
		cout << U_neighbourhood[i] << " ";
	}
	cout << endl;
	cout << "V_neighbourhood" << endl;
	for (long long int i = 0; i < U_neighbourhood.size(); i++) {
		cout << V_neighbourhood[i] << " ";
	}
	cout << endl;
	*/
	if (U_neighbourhood.size() != V_neighbourhood.size())return false;
	for (long long int i = 0; i < U_neighbourhood.size(); i++) {
		if (U_neighbourhood[i] != V_neighbourhood[i])return false;
	}
	//cout << "check_neighbor�I��" << endl;
	return true;
}

//�ߖT���l�����v�Z����(�i�C�[�u)
void making_Type_partitions() {
	long long int count = 0;
	for (long long int i = 0; i < n; i++) {
		for (long long int j = i; j < n; j++) {
			if (check_neighbor(i, j)) {
				if (Place_of_vertices[i] == -1) {
					Place_of_vertices[i] = count;
					count++;
				}
				Place_of_vertices[j] = Place_of_vertices[i];
			}
		}
	}
	nd = count;
	Type_partitions.resize(count);
	for (long long int i = 0; i < n; i++) {
		Type_partitions[Place_of_vertices[i]].push_back(i);
	}
}

//�U�������O���t���v�Z����֐�.
vector<bool> making_induced_subgraph(vector<long long int> Vertex_Subset) {
	vector<bool> Exist;//�U�������O���t���v�Z����̂ɗp����z��,���_i���U�������O���t�̒��_�W���ɑ����Ă�����exist[i]=true,�����Ă��Ȃ����exist[i]=false�ł���.
	Exist.resize(n);
	for (long long int i = 0; i < n; i++) {
		Exist[i] = false;
	}
	for (long long int i = 0; i < Vertex_Subset.size(); i++) {
		Exist[Vertex_Subset[i]] = true;
	}
	for (int i = 0; i < Induced_subgraph.size(); i++) {
		Induced_subgraph[i].clear();
		Induced_subgraph[i].shrink_to_fit();
	}
	Induced_subgraph.resize(n);
	for (long long int i = 0; i < n; i++) {
		for (long long int j = 0; j < G[i].size(); j++) {
			//i��G[i][j]���ǂ�����U�������O���t�̒��_�W���ɑ�����Ȃ��.
			if (Exist[i] && Exist[G[i][j]]) {
				Induced_subgraph[i].push_back(G[i][j]);
			};
		}
	}
	return Exist;
}

//�ŏI�I�Ɋ������钸�_�����߂�֐�(bit���g��Ȃ�����).
vector<bool> who_is_influenced_not_bit(vector<bool> Exist) {
	vector<bool> Influenced;//���_i���������Ă�����influenced[i]=true,�����łȂ����influenced[i]=false�ł���.
	Influenced.resize(n);
	for (long long int i = 0; i < n; i++) {
		Influenced[i] = false;
	}
	//1��ڂ̊g�U�ߒ��̎����J�n.
	for (long long int i = 0; i < n; i++) {
		if (Exist[i] && T[i] == 0)Influenced[i] = true;
	}
	//1��ڂ̊g�U�ߒ��̎����I��.
	//t��ڂ̊g�U�ߒ��̎����J�n.
	bool changed = false;
	do {
		changed = false;
		for (long long int i = 0; i < n; i++) {
			long long int count = 0;//���_u�̗אړ_�Ŋ������Ă��钸�_�̐��𐔂���.
			if (Exist[i] && !Influenced[i]) {
				for (long long int j = 0; j < Induced_subgraph[i].size(); j++) {
					if (Influenced[Induced_subgraph[i][j]])count++;
				}
				if (count >= T[i]) {
					Influenced[i] = true;
					changed = true;
				}
			}
		}
	} while (changed);
	//t��ڂ̊g�U�ߒ��̎����I��.
	return Influenced;
}

//Y(X)�����߂�֐�.
vector<bool> calculate_YX(vector<bool> Influenced, vector<bool> Exist) {
	vector<bool> YX;//���_i��Y(X)�ɑ�����Ȃ�YX[i]=1,�����Ȃ��Ȃ�YX[i]=0.
	YX.resize(n);
	for (long long int i = 0; i < n; i++) {
		YX[i] = false;
	}
	for (long long int i = 0; i < n; i++) {
		if (!Exist[i]) {
			long long int count = 0;
			for (long long int j = 0; j < G[i].size(); j++) {
				if (Influenced[G[i][j]])count++;
			}
			if (count >= T[i])YX[i] = true;
		}
	}
	return YX;
}

//IIB_k�̑O����(G�̃^�C�v�p�[�e�B�V����{V_0,V_1,...,V_nd}�̂��ꂼ���V_i={v_{i,1},...,v_{i,|V_i|}}�̒��_��臒l�̔񌸏���,�Ⴆ��,t(v_{i,j})<=t(v_{i,j+1})),�̂悤�ɕ��ׂ�).
void sort_in_order_of_thresholds() {
	//cout << "sort_in_order_of_thresholds�J�n" << endl;
	for (long long int i = 0; i < Type_partitions.size(); i++) {
		sort(Type_partitions[i].begin(), Type_partitions[i].end());
	}
	//cout << "sort_in_order_of_thresholds�I��" <<endl;
}

//�d���g�ݍ��킹��.
vector<long long int> A;
bool exist = false;
void overlapping_combination(long long int s, long long int t) {
	//cout << "s: " << s << " t: " << t << endl;
	clock_t now = clock();
	if ((double)(now - start) / CLOCKS_PER_SEC >= 3600) {
		cout << (double)(now - start) / CLOCKS_PER_SEC << "sec." << endl;
		cout << "Timeout!" << endl;
		return;
	}
	if (exist)return;
	//cout << "overlapping_combination �J�n�@s=" << s << " t=" << t << endl;
	if (s == nd && t == 0) {
		//�f�o�b�N�p�J�n.
		/*
		cout << "A:" << endl;
		for (long long int i = 0; i < nd; i++) {
			cout << A[i] << " ";
		}
		cout << endl;
		*/
		//�f�o�b�N�p�I��.
		/*
		cout << "Type_partition:" << endl;
		for (long long int i = 0; i < nd; i++) {
			for (long long int j = 0; j < A[i]; j++) {
				cout << Type_partitions[i][j] << " ";
			}
			cout << endl;
		}
		*/
		vector<long long int> Vertex_Subset;
		for (long long int i = 0; i < nd; i++) {
			for (long long int j = 0; j < A[i]; j++) {
				if (A[i] > Type_partitions[i].size())return;
				Vertex_Subset.push_back(Type_partitions[i][j]);
			}
		}
		vector<bool> Exist;
		Exist = making_induced_subgraph(Vertex_Subset);
		vector<bool> Influenced;
		Influenced = who_is_influenced_not_bit(Exist);
		vector<bool> YX;
		YX = calculate_YX(Influenced, Exist);
		long long int count = 0;//Y(X)�̒��_��
		for (long long int i = 0; i < n; i++) {
			if (YX[i])count++;
		}
		//�f�o�b�N�J�n.
		/*
		cout << "Vertex_Subset" << endl;
		for (long long int i = 0; i < Vertex_Subset.size(); i++) {
			cout << Vertex_Subset[i]+1 << " ";
		}
		cout << endl;
		cout << "Exist" << endl;
		for (long long int i = 0; i < n; i++) {
			cout << Exist[i] << " ";
		}
		cout << endl;
		cout << "Influenced" << endl;
		for (long long int i = 0; i < n; i++) {
			cout << Influenced[i] << " ";
		}
		cout << endl;
		cout << "YX" << endl;
		for (long long int i = 0; i < n; i++) {
			cout << YX[i] << " ";
		}
		cout << endl;
		cout << "count=" << count << endl;
		*/
		//cout << "l=" << l << endl;
		//cout << "k=" << k << endl;
		//�f�o�b�N�I��.
		if (count <= l) {
			//cout << "true�ł�" << endl;
			exist = true;
			return;
		}
		//cout << "false�ł�.aaaaaaaaaa" << endl;
		return;
	}
	if (s == nd - 1) {
		A[s] = t;
		overlapping_combination(s + 1, 0);
	}
	else {
		for (long long int i = 0; i <= t; i++) {
			//cout << "i=" << i << endl;
			A[s] = i;
			overlapping_combination(s + 1, t - i);
		}
	}
	return;
}

//IIB_k(G,k,l).//G,k,l�̓O���[�o���ϐ��Őݒ肵�Ă���̂�,�֐��̈����ɏ����Ă��Ȃ�.
void IIB_k() {
	//cout << "IIB_k�J�n" << endl;
	for (long long int f = 0; f < k + 1; f++) {
		//cout << "f=" << f << endl;
		overlapping_combination(0, f);
	}
	return;
}

//���C���֐�.
int main() {
	//���͊J�n.
	ifstream ifs1("johnson8-4-4.txt");

	if (!ifs1) {
		std::cout << "Errer!";
		return 1;
	}

	string s;
	long long int count = 0;
	long long int x = 0;
	long long int y = 0;

	while (getline(ifs1, s, ' ')) {     // �X�y�[�X�i' '�j�ŋ�؂��āC�i�[
		if (count == 0) {//���_��
			n = stoll(s);
			G.resize(n); //�O���t�̑傫���m��
			count++;
		}
		else if (count == 1) {//�}��
			m = stoll(s);
			count++;
		}
		else if (count > 1 && count < 2 + 2 * m && count % 2 == 0) {//�O���t�̎}�̒[�_
			x = stoll(s);
			x--;
			count++;
		}
		else if (count > 1 && count < 2 + 2 * m && count % 2 == 1) {//�O���t�̎}�̂�����̒[�_
			y = stoll(s);
			y--;
			count++;
			G[x].push_back(y);
			G[y].push_back(x);
		}
	}

	ifs1.close();

	ifstream ifs2("johnson8-4-4_threshold.txt");

	if (!ifs2) {
		std::cout << "Error!";
		return 1;
	}

	T.resize(n);
	count = 0;

	while (getline(ifs2, s, ' ')) {//���_��臒l
		x = stoll(s);
		T[count] = x;
		count++;
	}

	ifs2.close();

	mod = a * n * n;//mod�͒萔�~n^2.

	for (int i = 0; i < n; i++) {
		Place_of_vertices.push_back(-1);
	}
	//���͏I��.

	//���͐����J�n.
	for (int i = 0; i < n; i++) {
		sort(G[i].begin(), G[i].end());//�O���t�̗אڃ��X�g��l�����������ɕ��ׂ�(�ߖT���l���������Ă��邩�ǂ����̊m�F�����鎞�̂��߂�).
	}
	//���͐����I��.
	//���͊m�F�J�n.
	/*
	for (long long int i = 0; i < G.size(); i++) {
		for (long long int j = 0; j < G[i].size(); j++) {
			cout << G[i][j] << " ";
		}
		cout << endl;
	}
	cout << "���_��:" << n << endl;
	cout << "�}��:" << m << endl;
	cout << "k:" << k << endl;
	cout << "l:" << l << endl;
	long long int edge_number = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < G[i].size(); j++) {
			cout << "�}" << edge_number + 1 << "�{��:" << i << " " << G[i][j] << endl;
			edge_number++;
		}
	}
	for (int i = 0; i < n; i++) {
		cout << T[i] << endl;
	}
	//�������m�F�J�n.
	for (int i = 0; i < Monomials.size(); i++) {
		cout << Monomials[i] << endl;
	}
	//�������m�F�I��.
	*/
	making_Type_partitions();


	cout << "nd: " << nd << endl;

	A.resize(n);

	sort_in_order_of_thresholds();

	start = clock();    //���ԑ���J�n.

	for (long long int parameter_sum = 0; parameter_sum < n + 1; parameter_sum++) {
		for (long long int i = 0; i < parameter_sum + 1; i++) {
			k = i;
			l = parameter_sum - k;
			cout << "k: " << k << " l: " << l << endl;
			IIB_k();
			/*
			for (long long int j = 0; j < Induced_subgraph.size(); j++) {
				cout << "j: " << j<< endl;
				for (long long int jj = 0; jj < Induced_subgraph[j].size(); jj++) {
					cout << Induced_subgraph[j][jj] << " ";
				}
				cout << endl;
			}
			*/
			if (exist) {
				cout << "Yes" << endl;
				clock_t end = clock();     // ���ԑ���I��.
				cout << "duration = " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";
				return 0;
			}
			else {
				cout << "No" << endl;
			}
		}
	}
	clock_t end = clock();     // ���ԑ���I��.
	cout << "duration = " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";
	return 0;
}