#pragma once

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

ParallelGroup *parallelGenerator(int numBalls, int *numParallelGroup);
void deleteArray(ParallelGroup *ParallelArray, int N);