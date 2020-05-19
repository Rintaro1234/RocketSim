#include "iostream"
using namespace std;

int main(void)
{
	int **CollisionTurns;
	int BullNumber = 4; // 偶数のみ

	// y軸上にボールの数だけ配列を作成
	CollisionTurns = new int*[BullNumber];
	// x軸上にボールの数だけ配列を作成
	for (int i = 0; i < BullNumber; i++) 
	{
		CollisionTurns[i] = new int[BullNumber];
	}

	// 右斜め下方向に数字を入れていく
	int k = 0;
	for (int i = 0; i < BullNumber / 2; i++)
	{
		//　同じ要素の組み合わせはいらないためx軸を一つ右にずらす
		int xx = i + 1;
		int yy = 0;
		for (int j = 0; j < BullNumber; j++)
		{
			// x方向をずらしているため、配列以上に大きくなる。
			// その場合、xxにyyを代入し、yyを0にする。
			// そうすれば、続きの組み合わせが書ける。
			if (BullNumber <= xx)
			{
				if (BullNumber / 2 < i + 2) break;
				xx = yy;
				yy = 0;
			}
			// とりあえず偶数ずれのときはテキトーな値を入れとく
			if (i % 2 == 0)
			{
				// 最初(y = 0)の要素をできるだけ最初に実行したいため小さい値を実行
				// 周期的にずれるため偶数or奇数で判別
				if (j % 2 == 0)
				{
					CollisionTurns[yy][xx] = k;
				}
				else
				{
					CollisionTurns[yy][xx] = k + 1;
				}
			}
			else
			{
				CollisionTurns[yy][xx] = k;
			}
			// 次の場所へ移動
			xx++;
			yy++;
		}
		// 1ずれあたり2進むため＋2する
		k = k + 2;
	}

	// ない部分には取り合ず-1を入れて置き、あとで変換
	for (int i = 0; i < BullNumber; i++)
	{
		CollisionTurns[i][i] = -1;
	}

	for (int i = 0; i < BullNumber / 2; i++)
	{
		int xx = i + 1;
		int yy = 0;
		for (int j = 0; j < BullNumber; j++)
		{
			if (BullNumber <= xx)
			{
				xx = yy;
				yy = 0;
			}
			CollisionTurns[xx][yy] = -1;
			xx++;
			yy++;
		}
	}

	// 表示
	for (int i = 0; i < BullNumber; i++)
	{
		for (int j = 0; j < BullNumber; j++)
		{
			if (CollisionTurns[i][j] == -1)
			{
				cout << "x" << " ,";
			}
			else
			{
				// 二桁以上だと表が見にくくなるため修正(本来不要)
				if(10 <= CollisionTurns[i][j])cout << CollisionTurns[i][j] << ",";
				if(CollisionTurns[i][j] < 10)cout << CollisionTurns[i][j] << " ,";
			}
		}
		cout << endl;
	}

	// メモリ開放
	delete CollisionTurns;
}