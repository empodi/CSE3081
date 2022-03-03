#include "Graph_adj_list.h"
#include <queue>

extern void errorExit(const char *s);

SLList2 pool;
unsigned long UsedMemoryForArray = 0;

void Read_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E) {

	int i;
	int vFront, vRear, eCost;
	SLL* newNode;

	for (i = 0; i < Vnum; i++) {
		V[i].name = i;
		V[i].flag = false;
	}

	for (i = 0; i < Enum; i++) {

		scanf_s("%d %d %d", &vFront, &vRear, &eCost);

		E[i].name = i;
		E[i].vf = vFront;
		E[i].vr = vRear;
		E[i].cost = eCost;
		E[i].flag = false;

		newNode = pool.allocSLL();
		newNode->i = i;

		V[vFront].S.push(newNode);

		newNode = NULL;

		newNode = pool.allocSLL();
		newNode->i = i;

		V[vRear].S.push(newNode);
	}
}

void Free_Graph_adj_list(int Vnum, vertex *V) {

	int i;

	for (i = 0; i < Vnum; i++) {

		while (1) {

			SLL* delNode = V[i].S.pop();

			if (!delNode) break;

			pool.freeSLL(delNode);
		}
	}


}

int BFS_Tree_adj_list(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E     // edge structure array (starting index 0)
) {
	
	using namespace std;

	int costSum = 0;

	queue<int> q;

	q.push(src);
	V[src].flag = true;	

	while (!q.empty()) {		

		int vIdx = q.front();	
		q.pop();				// Ž���� vertex�� pop

		SLL* node = V[vIdx].S.top();	// ���� vertex�� ������ edge 

		while (node) {

			int eIdx = node->i;

			int nextSrc = E[eIdx].vf == vIdx ? E[eIdx].vr : E[eIdx].vf;
			// edge�� ����� �� vertex �� ���� Ž������ vertex�� �ٸ� vertex�� ����

			if (V[nextSrc].flag == false) { // nextSrc vertex �湮���� ���� ���

				E[eIdx].flag = true;
				V[nextSrc].flag = true;	

				q.push(nextSrc);	// nextSrc vertex Ž��

				costSum += E[eIdx].cost;
			}

			node = node->p; // �� ���� edge
		}

	}

	return costSum;
}

// SLL management functions

// stack member fuctions
void sllStack2::push(SLL *p) {
	// insert p to the top of stack ST
	p->p = ST;
	ST = p;
}

SLL *sllStack2::pop(void) {
	// remove and return p at the top of ST

	if (ST == NULL) {
		return ST;
	}

	SLL* p = ST;
	ST = p->p;
	
	return p;
}

SLL *sllStack2::top(void) {
	// return p at the top of ST
	return ST;
}

bool sllStack2::empty(void) {
	// return true if empty
	if (ST == NULL)
		return true;
	else
		return false;
}

// SLList2 member functions
SLL *SLList2::allocSLL(void) {		// allocate an SLL element
	SLL *p;

	if (SLL_pool == NULL) {

		p = (SLL*)malloc(sizeof(SLL));

		if (p == NULL) {
			errorExit("Memory allocation error (allocSLL)");
		}

		p->i = NONE;
		UsedMemoryForSLLs += sizeof(SLL);
	}
	else {
		p = SLL_pool;
		SLL_pool = p->p;
	}
	
	p->p = NULL;
	SLL_cnt++;

	return(p);
}

void SLList2::freeSLL(SLL *p) {		// free *p (move it to SLL_pool
	if (p->i == NONE) {
		errorExit("This is already freed(freeSLL).");
	}

	p->p = SLL_pool;
	SLL_pool = p;
	p->i = NONE;

	SLL_cnt--;
}

void SLList2::freeSLL_pool(void) {	// clear SLL_pool
	SLL *p = SLL_pool;

	while (p) {

		SLL_pool = p->p;
		free(p);

		UsedMemoryForSLLs -= sizeof(SLL);
		p = SLL_pool;
	}

	if (SLL_cnt != 0) {
		errorExit("Non-zero SLL_cnt after cleanup.");
	}
}