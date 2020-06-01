#include <iostream>
#include <string>
#include <algorithm>
#include "parallelGroup.h"
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

//-----------------------------------------------------------------------------
// 横軸のはみ出しに対する折返しを考慮して、グリッドのインデックスを変換する
void GetWrapIndex(int N, int x, int y, int *xx, int *yy)
{
	// 縦横ともに、要素数N周期で折り返す
	x = (x % N);
	y = (y % N);

	// グリッドの上半分を指すように、ラップする
	if (y <= x)
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

// 斜めラインの並列処理グループを見るける
// @param N ボールの数
// @param CollisionPairTable[][] コリジョン判定を行う2個のボールのペアを表す、二次元配列のグリッド
// @param k ライン先頭の位置(x座標)
// @param pairNo 次のグループ番号
// @return 次のグループ番号
int fingCollisionPairSub(int N, int **CollisionPairTable, int k, int pairNo);

// メモリを解放する
void deleteArray(ParallelGroup *ParallelArray, int N)
{
	for (int i = 0; i < N; i++)
	{
		delete[] ParallelArray[i].array;
	}
	delete[] ParallelArray;
}


//-----------------------------------------------------------------------------
ParallelGroup *parallelGenerator(int numBalls, int *numParallelGroup)
{
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
	for (int k = 1; k <= numBalls / 2; k++)
	{
		numGroup = fingCollisionPairSub(numBalls, CollisionPairTable, k, numGroup);
	}

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

	// メモリ開放
	for (int y = 0; y < numBalls; y++)
	{
		delete[] CollisionPairTable[y];
	}
	delete[] CollisionPairTable;

	*numParallelGroup = numGroup;
	return parallelGroup;
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
	return (i < L) ? 0 : 1;
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
	while (true)
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
	delete[] pair;

	return maxGroupID + 1;
}