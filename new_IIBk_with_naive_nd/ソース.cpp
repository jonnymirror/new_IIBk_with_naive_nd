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

//定数開始.
clock_t start;
vector<vector<long long int>> G;//入力されるグラフ.
long long int n;//頂点数.
long long int m;//枝数.
long long int k;//パラメータ(最終的に感染する頂点の数の最大値).
long long int l;//パラメータ(免疫を持たせる頂点の数の最大値).
vector<long long int> T;//関数.
long long int x = 5;//ハッシュ値の計算に用いるhash-multiplier.
long long int mod;//ハッシュ値の計算に用いるmod(頂点数によって変化する).
long long int a = 5;//modの計算に用いる定数.
vector<long long int> Place_of_vertices;//頂点iが属するタイプパーティションを表す配列,Place_of_vertices[i]=-1の時はその頂点はまだどのタイプパーティションにも属していないことを表す,Place_of_vertices[i]=jの時は頂点iがタイプパーティションjに属していることを表す.
vector<vector<long long int>> Type_partitions;//タイプパーティション.
long long int nd = -1;//グラフの近傍多様性の値を格納する変数.
long long int numbers_of_induced_subgraph_vertices;//誘導部分グラフの頂点数.
vector<vector<long long int>> Induced_subgraph;//誘導部分グラフ.
//定数終了.

//2つの頂点u,vの近傍を比較する関数(グラフの隣接リストが値の小さい順にsortしている前提で前から比較していく).
bool check_neighbor(long long int u, long long int v) {
	//cout << "check_neighbor開始" << endl;
	//cout << "u:" << u << "  v:" << v << endl;
	//グラフの隣接リスト確認.
	/*
	for (long long int i = 0; i < G.size(); i++) {
		for (long long int j = 0; j < G[i].size(); j++) {
			cout << G[i][j] << " ";
		}
		cout << endl;
	}
	*/
	vector<long long int> U_neighbourhood;//uの近傍(vは取り除く).
	vector<long long int> V_neighbourhood;//vの近傍(uは取り除く).
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
	//cout << "check_neighbor終了" << endl;
	return true;
}

//近傍多様性を計算する(ナイーブ)
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

//誘導部分グラフを計算する関数.
vector<bool> making_induced_subgraph(vector<long long int> Vertex_Subset) {
	vector<bool> Exist;//誘導部分グラフを計算するのに用いる配列,頂点iが誘導部分グラフの頂点集合に属していたらexist[i]=true,属していなければexist[i]=falseである.
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
			//iとG[i][j]がどちらも誘導部分グラフの頂点集合に属するならば.
			if (Exist[i] && Exist[G[i][j]]) {
				Induced_subgraph[i].push_back(G[i][j]);
			};
		}
	}
	return Exist;
}

//最終的に感染する頂点を求める関数(bitを使わない実装).
vector<bool> who_is_influenced_not_bit(vector<bool> Exist) {
	vector<bool> Influenced;//頂点iが感染していたらinfluenced[i]=true,そうでなければinfluenced[i]=falseである.
	Influenced.resize(n);
	for (long long int i = 0; i < n; i++) {
		Influenced[i] = false;
	}
	//1回目の拡散過程の実装開始.
	for (long long int i = 0; i < n; i++) {
		if (Exist[i] && T[i] == 0)Influenced[i] = true;
	}
	//1回目の拡散過程の実装終了.
	//t回目の拡散過程の実装開始.
	bool changed = false;
	do {
		changed = false;
		for (long long int i = 0; i < n; i++) {
			long long int count = 0;//頂点uの隣接点で感染している頂点の数を数える.
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
	//t回目の拡散過程の実装終了.
	return Influenced;
}

//Y(X)を求める関数.
vector<bool> calculate_YX(vector<bool> Influenced, vector<bool> Exist) {
	vector<bool> YX;//頂点iがY(X)に属するならYX[i]=1,属さないならYX[i]=0.
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

//IIB_kの前処理(Gのタイプパーティション{V_0,V_1,...,V_nd}のそれぞれのV_i={v_{i,1},...,v_{i,|V_i|}}の頂点を閾値の非減少順,例えば,t(v_{i,j})<=t(v_{i,j+1})),のように並べる).
void sort_in_order_of_thresholds() {
	//cout << "sort_in_order_of_thresholds開始" << endl;
	for (long long int i = 0; i < Type_partitions.size(); i++) {
		sort(Type_partitions[i].begin(), Type_partitions[i].end());
	}
	//cout << "sort_in_order_of_thresholds終了" <<endl;
}

//重複組み合わせ列挙.
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
	//cout << "overlapping_combination 開始　s=" << s << " t=" << t << endl;
	if (s == nd && t == 0) {
		//デバック用開始.
		/*
		cout << "A:" << endl;
		for (long long int i = 0; i < nd; i++) {
			cout << A[i] << " ";
		}
		cout << endl;
		*/
		//デバック用終了.
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
		long long int count = 0;//Y(X)の頂点数
		for (long long int i = 0; i < n; i++) {
			if (YX[i])count++;
		}
		//デバック開始.
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
		//デバック終了.
		if (count <= l) {
			//cout << "trueです" << endl;
			exist = true;
			return;
		}
		//cout << "falseです.aaaaaaaaaa" << endl;
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

//IIB_k(G,k,l).//G,k,lはグローバル変数で設定しているので,関数の引数に書いていない.
void IIB_k() {
	//cout << "IIB_k開始" << endl;
	for (long long int f = 0; f < k + 1; f++) {
		//cout << "f=" << f << endl;
		overlapping_combination(0, f);
	}
	return;
}

//メイン関数.
int main() {
	//入力開始.
	ifstream ifs1("johnson8-4-4.txt");

	if (!ifs1) {
		std::cout << "Errer!";
		return 1;
	}

	string s;
	long long int count = 0;
	long long int x = 0;
	long long int y = 0;

	while (getline(ifs1, s, ' ')) {     // スペース（' '）で区切って，格納
		if (count == 0) {//頂点数
			n = stoll(s);
			G.resize(n); //グラフの大きさ確保
			count++;
		}
		else if (count == 1) {//枝数
			m = stoll(s);
			count++;
		}
		else if (count > 1 && count < 2 + 2 * m && count % 2 == 0) {//グラフの枝の端点
			x = stoll(s);
			x--;
			count++;
		}
		else if (count > 1 && count < 2 + 2 * m && count % 2 == 1) {//グラフの枝のもう一つの端点
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

	while (getline(ifs2, s, ' ')) {//頂点の閾値
		x = stoll(s);
		T[count] = x;
		count++;
	}

	ifs2.close();

	mod = a * n * n;//modは定数×n^2.

	for (int i = 0; i < n; i++) {
		Place_of_vertices.push_back(-1);
	}
	//入力終了.

	//入力整理開始.
	for (int i = 0; i < n; i++) {
		sort(G[i].begin(), G[i].end());//グラフの隣接リストを値が小さい順に並べる(近傍多様性が合っているかどうかの確認をする時のために).
	}
	//入力整理終了.
	//入力確認開始.
	/*
	for (long long int i = 0; i < G.size(); i++) {
		for (long long int j = 0; j < G[i].size(); j++) {
			cout << G[i][j] << " ";
		}
		cout << endl;
	}
	cout << "頂点数:" << n << endl;
	cout << "枝数:" << m << endl;
	cout << "k:" << k << endl;
	cout << "l:" << l << endl;
	long long int edge_number = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < G[i].size(); j++) {
			cout << "枝" << edge_number + 1 << "本目:" << i << " " << G[i][j] << endl;
			edge_number++;
		}
	}
	for (int i = 0; i < n; i++) {
		cout << T[i] << endl;
	}
	//多項式確認開始.
	for (int i = 0; i < Monomials.size(); i++) {
		cout << Monomials[i] << endl;
	}
	//多項式確認終了.
	*/
	making_Type_partitions();


	cout << "nd: " << nd << endl;

	A.resize(n);

	sort_in_order_of_thresholds();

	start = clock();    //時間測定開始.

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
				clock_t end = clock();     // 時間測定終了.
				cout << "duration = " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";
				return 0;
			}
			else {
				cout << "No" << endl;
			}
		}
	}
	clock_t end = clock();     // 時間測定終了.
	cout << "duration = " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";
	return 0;
}