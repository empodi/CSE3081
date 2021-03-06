#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>

#define NONE -1

typedef struct elm_edge {
	int  vf, vr;  // e = (vf, vr) where vf, vr are vertex array indices
	int  cost;    // edge cost
	bool flag;    // true if the edge is in the SP spanning tree, false otherwise 
	int  fp, rp;  // adj list ptr of forward and reverse (-1 if none)
} edge;

typedef struct elm_vertex {
	int  f_hd, r_hd;	// adj list header(-1 if none)
	int  distance;		// distance from src to this vertex
	int  heap_idx;		// minHeap index
	bool inS;			// true if SP length found(true when move from V-S to S) 
} vertex;

void swap(int* a, int* b);
void heap_insert(int* hIdx, int* minHeap, vertex* V, int vertex);
int heap_delete(int* hIdx, int* minHeap, vertex* V);
void heap_up(int child, int* minHeap, vertex* V);
int build_spanning_tree(int src, int Vnum, vertex* V, edge* E);

//void heap_adjust(int changeVal, int* minHeap, vertex* V);
//int heap_delete(int* hIdx, int* minHeap, vertex* V);
//void heap_insert(int* heap_len, int* minHeap, vertex* V, int vertex);

void swap(int* A, int* B) {	// swap A and B

	int temp = *A;
	*A = *B;
	*B = temp;
}

int build_spanning_tree (
	int src, /* source vertex index */
	int Vnum /* Vertex array size */, 
	vertex* V, edge* E /* vertex and edge array */ ) {
	/* 
		build spanning tree using distance values obtained by Dijkstra Algorithm 
		Output: cost sum and edges(set flag == true) of the mininum spanning tree
	*/

	int i, cost = 0;

	for (i = 0; i < Vnum; i++) {

		if (i == src) continue;	// skip source vertex

		int minEdge = -1;	// store the edge index with least cost from current vertex
		int minCost = INT_MAX;

		int eIdx = V[i].f_hd;	// calculate from front vertex

		while (eIdx != NONE) {

			if (!E[eIdx].flag && V[E[eIdx].vr].distance + E[eIdx].cost == V[i].distance)
				// check if the edge is visited and check if the cost matches with distance value

				if (E[eIdx].cost < minCost) {	// change minEdge and minCost if less value comes up
					minEdge = eIdx;
					minCost = E[eIdx].cost;
				}

			eIdx = E[eIdx].fp;
		}

		eIdx = V[i].r_hd;	// calculate from rear vertex

		while (eIdx != NONE) {

			if (!E[eIdx].flag && V[E[eIdx].vf].distance + E[eIdx].cost == V[i].distance)
				if (E[eIdx].cost < minCost) {
					minEdge = eIdx;
					minCost = E[eIdx].cost;
				}

			eIdx = E[eIdx].rp;
		}

		if (minEdge >= 0) {	// if minEdge is found
			E[minEdge].flag = true;
			cost += E[minEdge].cost;
		}
	}

	return cost;
}

int SPT_Dijkstra(
	int src,	// source vertex index
	// graph structure array
	// 1. the adjacency list structure is the same as PHW02
	// 2. additional fields are added for Dijkstra's algorithm(see .h file)
	int Vnum, vertex* V,	// Vertex array size and the array
	int Enum, edge* E,		// Edge array size and the array

	int* minHeap	// array for min heap (array size = Vnum+1)
		// heap index range is 1 ~ (Vnum - 1) note: src must not in the initial heap
		// just arry is passed. must construct min-heap in this function

	// OUTPUT
	// 1. V[].distance : shortest path length from src to this vertex
	// 2. shortest path spanning tree : set E[].flag = true if the edge is in the tree
	// 3. return the sum of edge costs in the shortest path spanning tree.
	//    should be small as possible (think a way to make it small)
) {
	int treeCost = 0;
	// *** 이 함수를 작성하자 ***
	// 반드시 min-heap을 사용하여 O((n+m)logn) 알고리즘을 구현해야 한다(아니면 trivial한 프로그램임)
	// heap 연산 등 필요한 함수는 자유롭게 작성하여 추가한다.
	// 그러나 global 변수, dynamic array 등은 추가로 사용하지 않는다(실제로 필요 없다)

	int i, heap_idx = 0;

	V[src].distance = 0;
	V[src].inS = true;

	int eIdx = V[src].f_hd;

	while (eIdx != NONE) {		// set the distance value from source vertex through f_hd
		V[E[eIdx].vr].distance = E[eIdx].cost;	
		eIdx = E[eIdx].fp;
	}

	eIdx = V[src].r_hd;

	while (eIdx != NONE) {		// set the distance value from source vertex through r_hd
		V[E[eIdx].vf].distance = E[eIdx].cost;
		eIdx = E[eIdx].rp;
	}

	for (i = 0; i < Vnum; i++) {	// put the distance vaule to minHeap
		if (i != src)
			heap_insert(&heap_idx, minHeap, V, i);
	}

	for (i = 0; i < Vnum - 2; i++) {	// getting the distance value from src vertex

		int u = heap_delete(&heap_idx, minHeap, V);
		V[u].inS = true;

		eIdx = V[u].f_hd;

		while (eIdx != NONE) {	// calculating distance value from f_hd

			int w = E[eIdx].vr;

			int path_cost = V[u].distance + E[eIdx].cost;

			if (!V[w].inS && path_cost < V[w].distance) {
				V[w].distance = path_cost;
				heap_up(V[w].heap_idx, minHeap, V);
			}

			eIdx = E[eIdx].fp;
		}

		eIdx = V[u].r_hd;

		while (eIdx != NONE) {	// calculating distance value from r_hd

			int w = E[eIdx].vf;

			int path_cost = V[u].distance + E[eIdx].cost;

			if (!V[w].inS && path_cost < V[w].distance) {
				V[w].distance = path_cost;
				heap_up(V[w].heap_idx, minHeap, V);
			}

			eIdx = E[eIdx].rp;
		}

	}

	treeCost = build_spanning_tree(src, Vnum, V, E);	// get spanning tree using distance value

	return treeCost;
}

void heap_insert(
	int* hIdx /* heap index to insert */, 
	int* minHeap /* array for min heap (array size = Vnum+1) */, 
	vertex* V, int vertex /* Vertex array size and the array */
) {
	/* insert new element to the heap */

	minHeap[++(*hIdx)] = vertex;
	V[vertex].heap_idx = *hIdx;

	heap_up(*hIdx, minHeap, V);	// move-up operation after inserting new element
}

int heap_delete(
	int* hIdx, /* heap index to insert */
	int* minHeap, /* array for min heap (array size = Vnum+1) */
	vertex* V /* Vertex array */
) {
	/* delete the minimum element(index = 1) from the heap */

	int ret = minHeap[1];	// return the miminum element in heap

	V[ret].heap_idx = -1;	// set deleted heap_idx -1 for cross reference

	minHeap[1] = minHeap[(*hIdx)--];

	int parent = 1;
	int child = 2;

	while (child <= *hIdx) {

		if (child < *hIdx && V[minHeap[child]].distance > V[minHeap[child + 1]].distance)
			child++;

		if (V[minHeap[parent]].distance <= V[minHeap[child]].distance) break;

		swap(&minHeap[parent], &minHeap[child]);	// swap minHeap indices
		swap(&V[minHeap[parent]].heap_idx, &V[minHeap[child]].heap_idx);	
		// swap heap_idx member of vertex structure to maintain cross-reference

		parent = child;
		child *= 2;
	}

	return ret;
}

void heap_up(
	int child, /* child index from which the move-up operation starts */
	int* minHeap, /* array for min heap (array size = Vnum+1) */
	vertex* V	/* Vertex array */
) {
	/* maintain the heap structure - get the child index and move it up if necessary */

	int parent = child / 2;

	while (child != 1 && V[minHeap[parent]].distance > V[minHeap[child]].distance) {

		swap(&minHeap[parent], &minHeap[child]);	// swap minHeap indices
		swap(&V[minHeap[parent]].heap_idx, &V[minHeap[child]].heap_idx);
		// swap heap_idx member of vertex structure to maintain cross-reference

		child = parent;
		parent /= 2;
	}
}

void Read_Graph(int Vnum, vertex* V, int Enum, edge* E) {
	// Graph 자료구조를 만드는 함수
	// *** 이 함수를 추가하자 ***
	// PHW02의 Read_Graph_adj_array()를 이 과제의 자료구조에 맞춰 살짝 수정하여 사용한다
	// 즉, Read_Graph_adj_array()에서 불필요한 내용을 제거한 후 사용.

	int i;
	int vFront, vRear, eCost;

	for (i = 0; i < Vnum; i++) {

		V[i].distance = INT_MAX;
		V[i].inS = false;
		V[i].f_hd = V[i].r_hd = NONE;
		V[i].heap_idx = NONE;
	}

	for (i = 0; i < Enum; i++) {

		scanf_s("%d %d %d", &vFront, &vRear, &eCost);

		E[i].vf = vFront;
		E[i].vr = vRear;
		E[i].cost = eCost;
		E[i].flag = false;

		E[i].fp = V[vFront].f_hd == NONE ? NONE : V[vFront].f_hd;

		E[i].rp = V[vRear].r_hd == NONE ? NONE : V[vRear].r_hd;

		V[vFront].f_hd = V[vRear].r_hd = i;
	}

}

// the following functions are for testing if the submitted program is correct.
int  Tree_Check(int Vnum, vertex* V, int Enum, edge* E, int* visited);
bool SPT_test(int src, int Vnum, vertex* V, int Enum, edge* E, int* minHeap);
void Error_Exit(const char* s);

int main(void) {
	int		src;
	vertex* V;		int Vnum;
	edge* E;		int Enum;
	int* minHeap;
	int    Tree_cost;
	int    Tnum; 		// # of test cases
	clock_t start, finish;
	double cmpt;

	scanf_s("%d", &Tnum);		// read # of tests

	for (int t = 0; t < Tnum; t++) {
		scanf_s("%d %d %d", &Vnum, &Enum, &src);
		V = new vertex[Vnum];
		E = new edge[Enum];
		minHeap = new int[Vnum + 1];	// heap array allocation
		if (V == NULL || E == NULL || minHeap == NULL) {
			Error_Exit("Memory Allocation Error");
		}
		Read_Graph(Vnum, V, Enum, E);

		/**/start = clock();	// start timer

		Tree_cost = SPT_Dijkstra(src, Vnum, V, Enum, E, minHeap);	// code by students

		/**/finish = clock();	// stop timer
		cmpt = ((double)(finish - start)) / (double)CLK_TCK;

		// 아래 Tree_Check와 SPT_test 함수는 첨부한 SPT_test.obj 파일에 있다.
		// 이 테스트에서 오류로 인하여 프로그램이 정지하면 뭔가 잘못된 것이다(제출해도 0점)
		if (Tree_Check(Vnum, V, Enum, E, minHeap) == 0) {
			Error_Exit("   ERROR The result is not a spanning tree");
		}
		if (SPT_test(src, Vnum, V, Enum, E, minHeap) == false) {
			Error_Exit("** Something wrong in applying Dijkstra's");
		}
		if (t != 0)
			printf("\n");

		printf("**T%d (Dijkstra) (V = %d, E = %d, time = %.3f sec) Tree Cost = %d\n",
			t + 1, Vnum, Enum, cmpt, Tree_cost);

		delete[] minHeap; delete[] V; delete[] E;
	}
	return 0;
}

void Error_Exit(const char* s) {
	printf("%s\n", s);
	exit(-1);
}
