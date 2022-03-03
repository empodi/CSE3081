
#include "Graph_adj_array.h"

void Read_Graph_adj_array(int Vnum, vertex V[], int Enum, edge E[]) {
	// read graph information
	// V와 E의 name field는 각각 자신의 인덱스를 저장한다
	// V[].flag과 E[].flag은 반드시 false로 설정해야 한다.

	// ***이 함수를 작성한다

	/*
	typedef struct _edge {
		int  name;
		int  vf, vr;  // e = (vf, vr) where vf, vr are vertex array indices
		int  cost;    // edge cost
		bool flag;    // true if in an SP, false otherwise
		int  fp, rp;  // adj list ptr of forward and reverse (-1 if none)
	} edge;

	typedef struct _vertex {
		int  name;
		bool flag;
		int  f_hd, r_hd; // adj list header(-1 if none)
	} vertex;
	*/

	int i;
	int vFront, vRear, eCost;

	for (i = 0; i < Vnum; i++) {

		V[i].name = i;
		V[i].flag = false;
		V[i].f_hd = V[i].r_hd = NONE;
	}

	for (i = 0; i < Enum; i++) {

		scanf_s("%d %d %d", &vFront, &vRear, &eCost);

		E[i].name = i;
		E[i].vf = vFront;
		E[i].vr = vRear;
		E[i].cost = eCost;
		E[i].flag = false;

		E[i].fp = V[vFront].f_hd == NONE ? NONE : V[vFront].f_hd;

		E[i].rp = V[vRear].r_hd == NONE ? NONE : V[vRear].r_hd;

		V[vFront].f_hd = V[vRear].r_hd = i;
	}
}

int DFS_Tree_adj_array(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex* V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge* E     // edge structure array (starting index 0) 
) {
	// DFS를 사용하여 DFS tree에 속하는 에지의 flag을 true로 설정한다.
	// DFS 시작 vertex는 입력 파일에서 지정된다(src).
	// DFS tree에 속한 에지의 cost 합을 return 한다(모든 각 edge cost가 1이면 unique)
	// recursive 함수로 작성해도 무방하다.

	// ***이 함수를 작성한다***

	V[src].flag = true;	

	int costSum = 0;

	int eIdx = V[src].f_hd;	// src_vertex의 front edge index

	while (eIdx != NONE) {

		int edge_vr = E[eIdx].vr;	// 현재 edge의 rear vertex

		if (!V[edge_vr].flag) {	// 현재 edge의 rear vertex를 방문하지 않은 경우

			V[edge_vr].flag = true;
			E[eIdx].flag = true;

			costSum += E[eIdx].cost;
			costSum += DFS_Tree_adj_array(edge_vr, Vnum, V, Enum, E);
		}

		eIdx = E[eIdx].fp;	// 현재 vertex와 연결된 다른 edge
	}

	return costSum;
}
