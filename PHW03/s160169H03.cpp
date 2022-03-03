#define _CRT_SECURE_NO_WARNINGS
using namespace std;
#include <time.h>
#include <stack>
#include "DBL.h"

//#define NO_PATH_OUT   // comment out this line for path output
void Error_Exit(const char *s);

typedef struct _Vertex {
	dblStack S;		// adj list contains edge index
	int degree;
} Vertex;

typedef struct _Edge {
	int v1, v2;
} Edge;

void graphGeneration(Vertex **V, Edge **E, int *VN, int *EN);
void adjListGenerate(Vertex *V, Edge *E, int VN, int EN);
void deallocGraph(Vertex *V, Edge *E, int VN);
int *Find_Euler(Vertex *V, Edge *E, int VN, int EN, int *flag, int *pathN);

DBList pool;	// DBL storage pool

int main() {
	int T, VN, EN;
	Vertex *V;
	Edge   *E;
	int *path;	// Euler cycle or path
	int pathN;  // path length
	int  flag;	// 0: cycle, 1: path, 2: none
	clock_t start_time, finish_time;

	scanf("%d", &T);	// read # of tests
	for (int t = 1; t <= T; t++) {	// for each test
		graphGeneration(&V, &E, &VN, &EN);

		start_time = clock(); // set the start time

		path = Find_Euler(V, E, VN, EN, &flag, &pathN); // find an Euler path or cycle

		finish_time = clock(); // set finish time
		
		double cmpt = (((double)(finish_time - start_time)) / CLK_TCK)*1000; // compute the time passed
		printf("Test= %d flag= %d VnumInCycle/Path)= %d ", t, flag, pathN);

		if (flag == 0)
			printf("Euler_cycle(exec_time= %.2f msec)\n",cmpt);
		else if (flag == 1)
			printf("Euler_path(exec_time= %.2f msec)\n", cmpt);
		else
			printf("not_Eulerian(exec_time= %.2f msec)\n", cmpt);

#ifndef NO_PATH_OUT
		if (flag != 2) {
			for (int i = 0; i < EN + 1; i++) {
				printf("%d\n", path[i]);
			}
		}
#endif
		if (flag != 2)
			delete[] path;
		deallocGraph(V, E, VN);
	}
	pool.freeDBL_pool();	// clear all the DBL elements

	return 0;
}

int *Find_Euler(Vertex *V, Edge *E, int VN, int EN, int *flag, int *pathN) {
	// input V, VN, E, EN
	// output through paramters
	//   *flag = 0 (Euler cycle), 1 (Euler path), 2 (not Eulerian)
	//   *pathN = size of path[] array
	// output by return
	//   *path = list of vertex ids found(Euler cycle or path)

	stack<int> ST;		// use STL stack as explained in the class
	int *path = NULL;

	int i, pathIdx, oddDegreeVertex = 0, srcVertex = -1;

	for (i = 0; i < VN; i++) {

		if (V[i].degree % 2 == 1) {
			oddDegreeVertex++;
			srcVertex = i;			// get the starting vertex for finding Euler Path
		}
	}

	switch (oddDegreeVertex) {

	case 2:				// Euler path
		*flag = 1;
		break;

	case 0:				// Euler cycle
		*flag = 0;
		srcVertex = 0;
		break;

	default:
		*pathN = 0;
		*flag = 2;
		return path;
	}

	int v, w;
	// v is a vertex and w is a vertex adjacent to v

	pathIdx = 0;

	ST.push(srcVertex);	// start finding Euler path/cycle

	path = new int[EN + 1];
	*pathN = EN + 1;

	while (!ST.empty()) {

		v = ST.top();

		if (V[v].degree == 0) {
			path[pathIdx++] = v;
			ST.pop();
		}
		else {
			DBL* edgeNode = V[v].S.top();
			Edge e = E[edgeNode->d];

			w = (e.v1 == v ? e.v2 : e.v1);	// another vertex incident on current edge

			ST.push(w);

			V[v].S.remove(edgeNode);
			V[w].S.remove(edgeNode->twin);

			pool.freeDBL(edgeNode);
			pool.freeDBL(edgeNode->twin);

			(V[v].degree)--;
			(V[w].degree)--;
		}
	}

	return path;
}

void deallocGraph(Vertex *V, Edge *E, int VN) {
	DBL *p;

	int i;

	for (i = 0; i < VN; i++) {
		while (!V[i].S.empty()) {
			p = V[i].S.pop();
			pool.freeDBL(p);
		}
	}

	delete[] V;
	delete[] E;
}

void graphGeneration(Vertex **V, Edge **E, int *VN, int *EN) {
	scanf("%d %d", VN, EN);	// read # of vertices and edges
	//allocVandEarray(V, E, *VN, *EN);	// vertex and edge array allocation

	*V = new Vertex[*VN];
	*E = new Edge[*EN];
	if (*V == NULL || *E == NULL) {
		Error_Exit("Memory Allocation Error!");
	}

	for (int v = 0; v < *VN; v++) {
		(*V)[v].degree = 0;
	}
	for (int e = 0; e < *EN; e++) {
		scanf("%d %d", &((*E)[e].v1), &((*E)[e].v2));	// read edge information
		++((*V)[(*E)[e].v1].degree);
		++((*V)[(*E)[e].v2].degree);
	}
	adjListGenerate(*V, *E, *VN, *EN);	// create adj list in G=(V,E)
}



void adjListGenerate(Vertex *V, Edge *E, int VN, int EN) {
	// Construct adjacent list in V array
	int v1, v2;
	DBL *adjE1, *adjE2;

	int i;

	for (i = 0; i < EN; i++) {

		v1 = E[i].v1;
		v2 = E[i].v2;

		adjE1 = pool.allocDBL();
		adjE2 = pool.allocDBL();

		adjE1->d = adjE2->d = i;		// d: edge array index
		adjE1->twin = adjE2;
		adjE2->twin = adjE1;

		V[v1].S.push(adjE1);
		V[v2].S.push(adjE2);
	}
}

void Error_Exit(const char *s) {
	printf("%s", s);
	exit(-1);
}

DBL *DBList::allocDBL(void) {		// allocate a DBL element
	DBL *p;
	
	if (DBL_pool == NULL) {

		p = new DBL;

		if (p == NULL) {
			Error_Exit("Memory alloc error(Alloc_DBL");
		}
		UsedMemoryForDBLs += sizeof(DBL);
		p->d = NONE;
	}
	else {
		p = DBL_pool;
		DBL_pool = p->rp;
	}

	p->lp = p->twin = p->rp = NULL;
	++DBL_cnt;

	return(p);
}

void DBList::freeDBL(DBL *p) {
	// move p to pool

	if (p->d == NONE) {
		Error_Exit("This element is already freed(Free_DBL).");
	}
	
	p->rp = DBL_pool;
	p->d = NONE;
	DBL_pool = p;

	--DBL_cnt;		// reduce # of active DBL elements by one 
}

void DBList::freeDBL_pool(void) {
	DBL *p = DBL_pool;

	while (p != NULL) {
		DBL_pool = p->rp;	// get next pointer
		delete p;
		p = DBL_pool;
		UsedMemoryForDBLs -= sizeof(DBL);
	}
	if (DBL_cnt != 0) {
		Error_Exit("Non-zero DBL_cnt after cleanup.");
	}
}

void dblStack::push(DBL *p) {

	if (ST != NULL) ST->lp = p;

	p->rp = ST;
	p->lp = NULL;
	ST = p;
}

DBL *dblStack::pop(void) {	// remove and return p in front of Q
	DBL *p;

	if (ST == NULL) return ST;

	p = ST;
	
	if (ST->rp == NULL)
		ST = NULL;
	else {
		ST = ST->rp;
		ST->lp = NULL;
	}

	return p;
}

void dblStack::remove(DBL *p) {	// delete p from ST

	if (ST == p) {
		ST = p->rp;

		if (p->rp != NULL) {
			p->rp->lp = NULL;
		}
	}
	else {
		p->lp->rp = p->rp;

		if (p->rp != NULL) {
			p->rp->lp = p->lp;
		}
	}
}

DBL *dblStack::top(void) {
	return ST;
}

bool dblStack::empty(void) {
	if (ST == NULL)
		return true;
	else
		return false;
}
