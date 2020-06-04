#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

int Wave(int hz, int i);

// ゼロ詰め二桁の10進数文字列を返す
string ato2i(int x)
{
	string str;
	if (x < 10) str = "0";
	str += to_string(x);
	return str;
}

// コリジョンをチェックする、2つのボールの組み合わせ
struct CollisionPair
{
	int idx0 = -1;
	int idx1 = -1;
};

//-----------------------------------------------------------------------------
// 同期無しで並列処理可能な、コリジョンチェックを行うボールの組み合わせのグループ
struct ParallelGroup
{
	CollisionPair *array = nullptr;
	int numPair = 0;
};

//-----------------------------------------------------------------------------
// 横軸のはみ出しに対する折返しを考慮して、グリッドのインデックスを変換する
void GetWrapIndex(int N, int x, int y, int *xx, int *yy)
{
	// 縦横ともに、要素数N周期で折り返す
	x = (x % N);
	y = (y % N);

	// グリッドの上半分を指すように、ラップする
	if(y <= x)
	{
		*xx = x;
		*yy = y;
	}
	else
	{
		*xx = y;
		*yy = x;
	}
}

//-----------------------------------------------------------------------------
// 斜めラインの並列処理グループを見るける
// @param N ボールの数
// @param CollisionPairTable[][] コリジョン判定を行う2個のボールのペアを表す、二次元配列のグリッド
// @param k ライン先頭の位置(x座標)
// @param pairNo 次のグループ番号
// @return 次のグループ番号
int fingCollisionPairSub(int N, int **CollisionPairTable, int k, int pairNo);

// テスト
// @param numBalls ボールの数
// @param CollisionPairTable[][] コリジョン判定を行う2個のボールのペアを表す、二次元配列のグリッド
bool checkCollisionPairTable(const int numBalls, const int *const *CollisionPairTable);

// 表示
// @param numBalls ボールの数
// @param CollisionPairTable[][] コリジョン判定を行う2個のボールのペアを表す、二次元配列のグリッド
void printCollisionPairTable(const int numBalls, const int *const *CollisionPairTable);

// テスト
// @param numBalls ボールの数
// @param numGroups グループの数
// @param groups グループの配列
bool checkParallelGroup(const int numBalls, const int numGroups, const m_ParallelGroup *groups);

// 表示
// @param numGroups グループの数
// @param groups グループの配列
void printParallelGroup(const int numGroups, const m_ParallelGroup *groups);

//-----------------------------------------------------------------------------
int main(void)
{
	// コリジョンチェックを行うボールの数
	const int numBalls = 18; // 偶数のみ

	// コリジョン判定を行う2個のボールのペアを、二次元配列のグリッドで表す。
	// 変数内の数字は、何回目の並列処理グループで判定するか。
	// (CollisionPairTable[2][3] = 5)
	// は、2番目と3番目のボールを、5回目の並列処理グループでコリジョンチェックする、の意味。
	// Note:
	// グリッドの対角成分は重複。下半分は使わない。
	// 例) CollisionPairTable[4][6] と CollisionPairTable[6][4] は同じペア。CollisionPairTable[6][4] は使わない。
	int **CollisionPairTable = new int*[numBalls];
	for (int i = 0; i < numBalls; i++)
	{
		CollisionPairTable[i] = new int[numBalls];
		for (int j = 0; j < numBalls; j++) CollisionPairTable[i][j] = -1;
	}

	// 斜めのラインを順に処理する
	int numGroup = 0;
	for(int k = 1; k <= numBalls / 2; k++)
	{
		numGroup = fingCollisionPairSub(numBalls, CollisionPairTable, k, numGroup);
	}

	// 確認
	bool ok = checkCollisionPairTable(numBalls, CollisionPairTable);

	// 表示
	if(ok)
	{
		cout << "成功！" << endl;
	}
	printCollisionPairTable(numBalls, CollisionPairTable);

	// 実際に使えるように変換
	ParallelGroup *parallelGroup = new ParallelGroup[numGroup];
	for (int i = 0; i < numGroup; i++)
	{
		// 並行処理できるペアの数
		int groupNo = 0;

		// 並行処理できるペアの数を調べる
		for (int x = 0; x < numBalls; x++)
		{
			for (int y = 0; y < numBalls; y++)
			{
				if (CollisionPairTable[y][x] == i) groupNo++;
			}
		}
		parallelGroup[i].array = new CollisionPair[groupNo];
		parallelGroup[i].numPair = groupNo;
		// ペアを入れていく
		for (int a = 0; a < groupNo;)
		{
			for (int x = 0; x < numBalls; x++)
			{
				for (int y = 0; y < numBalls; y++)
				{
					if (CollisionPairTable[y][x] == i)
					{
						parallelGroup[i].array[a].idx0 = x;
						parallelGroup[i].array[a].idx1 = y;
						a++;
					}
				}
			}
		}
	}

	// 確認
	ok = checkParallelGroup(numBalls, numGroup, parallelGroup);
	// 表示
	if (ok)
	{
		cout << "成功！" << endl;
	}
	printParallelGroup(numGroup, parallelGroup);

	// メモリ開放
	for (int y = 0; y < numBalls; y++)
	{
		delete [] CollisionPairTable[y];
	}
	delete [] CollisionPairTable;
}

//-----------------------------------------------------------------------------
// 増加するインデックスに対して周期的な矩形波(0と1)を返す変数
// @param L 0もしくは1が連続する数
// @param i インデックス
int Wave(int L, int i)
{
	// 矩形波の周期は 2L なので、その範囲に折り返す。
	i = i % (L * 2);
	// 矩形波の前半(i < L)は0、後半(L <= i)は1
	return (i < L)? 0: 1;
}

//-----------------------------------------------------------------------------
// 斜めラインの並列処理グループを見るける
// @param N ボールの数
// @param CollisionPair[][] コリジョン判定を行う2個のボールのペアを表す、二次元配列のグリッド
// @param k ライン先頭の位置(x座標)
// @param pairNoBase 次のグループ番号
// @return 次のグループ番号
int fingCollisionPairSub(int N, int **CollisionPair, int k, int pairNoBase)
{
	// 一回一次元上に順番を入れてから表に代入する
	int *pair = new int[N];
	// 空にする(-1)
	for (int i = 0; i < N; i++)
	{
		pair[i] = -1;
	}

	// 値を入れていく
	while(true)
	{
		// チェックをスタートするボールの組み合わせを見つける
		int groupID = pairNoBase;
		int p;
		for (p = 0; p < N; p++)
		{
			// 未チェックのペアからスタート
			if (pair[p] == -1)
			{
				// このペアに含まれるボールが参照される別のペアが未チェック、
				// もしくは別の並列処理グループならOK。
				// そうでないなら別の並列処理グループでスタートする。
				int pn = p - k;
				if (pn < 0) pn = pn + N;
				int pp = p + k;
				if (N <= pp) pp = pp - N;
				while (true)
				{
					if ((/*(pair[pn] == -1) || */(pair[pn] != groupID)) &&
						(/*(pair[pp] == -1) || */(pair[pp] != groupID)))
					{
						break;
					}
					groupID++;
				}
				break;
			}
		}
		// 全てのペアをチェック済みなら完了
		if (p == N) break;

		for (int i = 0; i < N; i++)
		{
			// 今からコリジョンチェックを含めようとする、並列処理グループの番号
			int gid = groupID + Wave(1, i);
			// このペアに含まれるボールが参照される次のペアが未チェック、
			// もしくは別の並列処理グループならOK。
			int pp = p + k;
			if (N <= pp) pp = pp - N;

			if ((pair[p] == -1) && (pair[pp] != gid))
			{
				pair[p] = gid;
			}
			else
			{
				// ダメだった場合には、このスキャンを終了して、次にスタート可能なペアを探し直す。
				break;
			}
			// このペアに含まれつボールが参照される次のペアへ進む。
			p = pp;
		}
	}

	// xからyを求め、そこに一時配列の情報を入れていく
	int maxGroupID = 0;
	int x = k - 1, y = 0;
	for (int i = 0; i < N; i++)
	{
		x = x + 1;
		y = x - k;
		// ボールの数からあふれしていた場合再計算
		if (N <= x)
		{
			k = N - k;
			x = y;
			y = x - k;
		}

		// 折り返した組み合わせがすでに別の並列処理グループに入っている可能性もある。
		// そのような場合は上書きしない。
		if (CollisionPair[y][x] < 0)
		{
			CollisionPair[y][x] = pair[i];
			if (maxGroupID < pair[i]) maxGroupID = pair[i];
		}
	}
	delete [] pair;

	return maxGroupID + 1;
}

//-----------------------------------------------------------------------------
// テスト
// @param numBalls ボールの数
// @param CollisionPairTable[][] コリジョン判定を行う2個のボールのペアを表す、二次元配列のグリッド
bool checkCollisionPairTable(const int numBalls, const int *const *CollisionPairTable)
{
	// 1. 全てのボールの組み合わせがコリジョンチェックされているか。
	// Note:
	// グリッドの全てに、何回目の並列処理でチェックされるかの値が入っていればOK。
	// (-1 は実行されないことを表す。)
	// ついでに並列処理の実行回数も調べる。
	bool ok = true;
	int lastParallelNo = 0;
	for (int y = 0; y < numBalls; y++)
	{
		for (int x = y + 1; x < numBalls; x++)
		{
			if (CollisionPairTable[y][x] == -1)
			{
				cout << "Error:実行されない組み合わせ。Ball#" << y << " - #" << x << endl;
				ok = false;
			}
			if (lastParallelNo < CollisionPairTable[y][x])
			{
				lastParallelNo = CollisionPairTable[y][x];
			}
		}
	}
	// 2. 一回の並列処理グループで、一つのボールが2回以上チェックされていたらエラー
	// チェック用フラグ変数使う
	bool *Checked = new bool[numBalls];
	for (int k = 0; k <= lastParallelNo; k++)
	{
		// チェック済みフラグをリセット
		for (int i = 0; i < numBalls; i++) Checked[i] = false;

		// この並列処理でチェックされる組み合わせを列挙して、
		for (int y = 0; y < numBalls; y++)
		{
			for (int x = y + 1; x < numBalls; x++)
			{
				if (CollisionPairTable[y][x] == k)
				{
					// この並列処理ですでにチェック済みであればエラー
					if (Checked[y])
					{
						cout << "Error:" << k << "回目の並列処理内で複数回参照。Ball#" << y << endl;
						ok = false;
					}
					if (Checked[x])
					{
						cout << "Error:" << k << "回目の並列処理内で複数回参照。Ball#" << x << endl;
						ok = false;
					}
					// チェック済みであることをマーク
					Checked[y] = Checked[x] = true;
				}
			}
		}
	}
	delete[] Checked;

	return ok;
}

//-----------------------------------------------------------------------------
// 表示
// @param numBalls ボールの数
// @param CollisionPairTable[][] コリジョン判定を行う2個のボールのペアを表す、二次元配列のグリッド
void printCollisionPairTable(const int numBalls, const int *const *CollisionPairTable)
{
	// グリッド表示(縦横ヘッダ付き)
	cout << "  |";
	for (int x = 0; x < numBalls; x++)
	{
		cout << ato2i(x) << ".";
	}
	cout << endl;
	cout << "--+";
	for (int x = 0; x < numBalls; x++)
	{
		cout << "---";
	}
	cout << endl;

	for (int y = 0; y < numBalls; y++)
	{
		cout << ato2i(y) << "|";

		for (int x = 0; x < numBalls; x++)
		{
			const int a = CollisionPairTable[y][x];
			if (x < y)
			{
				cout << "   ";
			}
			else if (y == x)
			{
				cout << " - ";
			}
			else if (a == -1)
			{
				cout << "xx,";
			}
			else
			{
				cout << ato2i(a) << ",";
			}
		}
		cout << endl;
	}
}

//-----------------------------------------------------------------------------
// 表示
// @param numGroups グループの数
// @param groups グループの配列
void printParallelGroup(const int numGroups, const ParallelGroup *groups)
{
	cout << "numGroups = " << numGroups << endl;

	for (int i = 0; i < numGroups; i++)
	{
		cout << ato2i(i) << "| ";
		for (int j = 0; j < groups[i].numPair; j++)
		{
			const CollisionPair &pair = groups[i].array[j];
			cout << ato2i(pair.idx1) << "-" << ato2i(pair.idx0) << ", ";
		}
		cout << endl;
	}
}

//-----------------------------------------------------------------------------
// テスト
// @param numBalls ボールの数
// @param numGroups グループの数
// @param groups グループの配列
bool checkParallelGroup(const int numBalls, const int numGroups, const ParallelGroup *groups)
{
	// 1. 各並列処理グループ内で、一つのボールが複数回参照されていないか
	bool ok = true;
	bool *Checked = new bool [numBalls];
	for (int i = 0; i < numGroups; i++)
	{
		// チェック済みフラグをリセット
		for (int j = 0; j < numBalls; j++) Checked[j] = false;

		// この並列処理でチェックされる組み合わせを列挙して、
		const CollisionPair *pair = groups[i].array;
		for (int j = 0; j < groups[i].numPair; j++)
		{
			// この並列処理ですでにチェック済みであればエラー
			if (Checked[pair->idx0])
			{
				cout << "Error:" << i << "回目の並列処理内で複数回参照。Ball#" << pair->idx0 << endl;
				ok = false;
			}
			if (Checked[pair->idx1])
			{
				cout << "Error:" << i << "回目の並列処理内で複数回参照。Ball#" << pair->idx1 << endl;
				ok = false;
			}
			// チェック済みであることをマーク
			Checked[pair->idx0] = Checked[pair->idx1] = true;
			// 次
			pair++;
		}
	}
	delete[] Checked;

	// 2. 複数の並列処理グループで重複したチェックがないか
	int *checkGrid = new int [numBalls * numBalls];
	for (int i = 0; i < numBalls * numBalls; i++) checkGrid[i] = -1;
	for (int i = 0; i < numGroups; i++)
	{
		const CollisionPair *pair = groups[i].array;
		for (int j = 0; j < groups[i].numPair; j++)
		{
			// 組み合わせがすでに別の並列処理グループでチェックされていないか
			int idx0 = min(pair->idx0, pair->idx1);
			int idx1 = max(pair->idx0, pair->idx1);
			int idx = (numBalls * idx0) + idx1;
			if (0 <= checkGrid[idx])
			{
				cout << "Error:並列処理グループ#" << checkGrid[idx] << "と#" << i << "で重複チェック。"
					 << "Ball#" << idx0 << " - #" << idx1 << endl;
				ok = false;
			}
			// 組み合わせに並列処理グループ番号を設定
			checkGrid[idx] = i;
			// 次
			pair++;
		}
	}

	// 3. 全てのボールの組み合わせがコリジョンチェックされているか。
	for (int y = 0; y < numBalls; y++)
	{
		for (int x = y + 1; x < numBalls; x++)
		{
			int idx = (numBalls * y) + x;
			if (checkGrid[idx] < 0)
			{
				cout << "Error:実行されない組み合わせ。Ball#" << y << " - #" << x << endl;
				ok = false;
			}
		}
	}

	delete[] checkGrid;

	return ok;
}

