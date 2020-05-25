#include <iostream>
#include <string>
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

// 斜めラインの並列処理グループを見るける
// @param N ボールの数
// @param CollisionPair[][] コリジョン判定を行う2個のボールのペアを表す、二次元配列のグリッド
// @param k ライン先頭の位置(x座標)
// @param pairNo 次のグループ番号
// @return 次のグループ番号
int fingCollisionPairSub(int N, int **CollisionPair, int k, int pairNo);

int main(void)
{
	// チェック用の変数
	const int numBalls = 8; // 偶数のみ

	// コリジョン判定を行う2個のボールのペアを、二次元配列のグリッドで表す。
	// 変数内の数字は、何回目の並列処理グループで判定するか。
	// (CollisionPair[2][3] = 5)
	// は、2番目と3番目のボールを、5回目の並列処理グループでコリジョンチェックする、の意味。
	// Note:
	// グリッドの対角成分は重複。下半分は使わない。
	// 例) CollisionPair[4][6] と CollisionPair[6][4] は同じペア。CollisionPair[6][4] は使わない。
	int **CollisionPair = new int*[numBalls];
	for (int i = 0; i < numBalls; i++)
	{
		CollisionPair[i] = new int[numBalls];
		for (int j = 0; j < numBalls; j++) CollisionPair[i][j] = -1;
	}

	// 斜めのラインを順に処理する
	int pairNo = 0;
	for(int k = 1; k <= numBalls / 2; k++)
	{
		pairNo = fingCollisionPairSub(numBalls, CollisionPair, k, pairNo);
	}

	// 確認
	// 1. 全てのボールの組み合わせがコリジョンチェックされているか。
	// Note:
	// グリッドの全てに、何回目の並列処理でチェックされるかの値が入っていればOK。
	// (-1 は実行されないことを表す。)
	// ついでに並列処理の実行回数も調べる。
	bool ok = true;
	int lastParallelNo = 0;
	for(int y = 0; y < numBalls; y++)
	{
		for(int x = y + 1; x < numBalls; x++)
		{
			if(CollisionPair[y][x] == -1)
			{
				cout << "Error:実行されない組み合わせ。Ball#" << y << " - #" << x << endl;
				ok = false;
			}
			if(lastParallelNo < CollisionPair[y][x])
			{
				lastParallelNo = CollisionPair[y][x];
			}
		}
	}
	// 2. 一回の並列処理グループで、一つのボールが2回以上チェックされていたらエラー
	// チェック用フラグ変数使う
	bool *Checked = new bool [numBalls];
	for(int k = 0; k <= lastParallelNo; k++)
	{
		// チェック済みフラグをリセット
		for (int i = 0; i < numBalls; i++) Checked[i] = false;

		// この並列処理でチェックされる組み合わせを列挙して、
		for(int y = 0; y < numBalls; y++)
		{
			for(int x = 0; x < numBalls; x++)
			{
				if(CollisionPair[y][x] == k)
				{
					// この並列処理ですでにチェック済みであればエラー
					if(Checked[y])
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

	// 表示
	if(ok)
	{
		cout << "成功！" << endl;
	}
	// グリッド表示(縦横ヘッダ付き)
	cout << "   ";
	for (int x = 0; x < numBalls; x++)
	{
		cout << ato2i(x) << ".";
	}
	cout << endl;

	for (int y = 0; y < numBalls; y++)
	{
		cout << ato2i(y) << ":";

		for (int x = 0; x < numBalls; x++)
		{
			const int a = CollisionPair[y][x];
			if(x < y)
			{
				cout << "   ";
			}
			else if(y == x)
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

	// メモリ開放
	for (int y = 0; y < numBalls; y++)
	{
		delete [] CollisionPair[y];
	}
	delete [] CollisionPair;
}

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

// 斜めラインの並列処理グループを見るける
// @param N ボールの数
// @param CollisionPair[][] コリジョン判定を行う2個のボールのペアを表す、二次元配列のグリッド
// @param k ライン先頭の位置(x座標)
// @param pairNoBase 次のグループ番号
// @return 次のグループ番号
int fingCollisionPairSub(int N, int **CollisionPair, int k, int pairNoBase)
{
	// 一回一次元上に順番を入れてから表に代入する
	int *pair;
	pair = new int[N];
	// 空にする(-1)
	for (int i = 0; i < N; i++)
	{
		pair[i] = -1;
	}

	int maxVal = k;

	// 値を入れていく
	{
		int p = 0;
		for (bool isPairFull = false; isPairFull == false;)
		{
			int maxK = 0;
			int x = p;
			for (int i = 0; i < N / 2; i++)
			{
				// 今から入れようとする値
				int kk = Wave(1, i);
				// 次のチェック
				int Nx;
				Nx = x + k;
				if (N <= Nx) Nx = Nx - N;

				if (pair[Nx] != kk)
				{
					pair[x] = kk;
					if (maxK < kk) maxK = kk;
				}
				else
				{
					pair[x] = maxK + 1;
					maxK = maxK + 1;
					break;
				}
				x = Nx;
			}
			// 空きがないかチェック
			isPairFull = true;
			for (p = 0; p < N; p++)
			{
				if (pair[p] == -1)
				{
					isPairFull = false;
					break;
				}
			}
		}
	}
	// xからyを求め、そこに一時配列の情報を入れていく
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

		CollisionPair[y][x] = pair[i] + pairNoBase;
		if (maxVal < pair[i] + pairNoBase) maxVal = pair[i] + pairNoBase;
	}
	delete pair;
	return maxVal + 1;
}
