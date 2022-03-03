#include <cstdio>
#include <cstdlib>

int get_Min(int a, int b, int c);
void ED_handler(int**& T, char*& SS, char*& TS, int s_idx, int t_idx, int& ins_cost, int& del_cost, int& sub_cost, char*& SR, char*& OP, char*& TR, int idx);
int get_Trace_Number(int**& T, int i, int j, int& ins_cost, int& del_cost, int& sub_cost);

int get_Min(int a, int b, int c) {

	a = (a < b) ? a : b;
	a = (a < c) ? a : c;

	return a;
}

void Edit_Distance(
	// inputs
	char* SS,		// Source string array
	char* TS,		// Edited string array
	int ins_cost, int del_cost, int sub_cost, // insertion, deletion, substitution cost

	// outputs
	int** Table,    // Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
	char** SR,		// Source string with insertion to T marks '*'
	char** OP,		// Operation sequence('i', 'd', 's', or '.' (nop))
	char** TR,      // Edited string with deletion form S mark '*'
					// NOTE: SR, OP, TR must be allocated inside the function as needed(minimum as possible)
	int* Mem_Allocated)
{
	int i, j;
	int s_len, t_len, op_len;

	op_len = s_len = t_len = 0;

	while (SS[s_len]) s_len++;

	while (TS[t_len]) t_len++;

	for (i = 0; i <= s_len; i++) Table[i][0] = i * del_cost;
	for (i = 0; i <= t_len; i++) Table[0][i] = i * ins_cost;

	for (i = 1; i <= s_len; i++) {
		for (j = 1; j <= t_len; j++) {

			int alpha = (SS[i - 1] == TS[j - 1]) ? 0 : sub_cost;
			Table[i][j] = get_Min(Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + alpha);
		}
	}

	op_len = get_Trace_Number(Table, s_len, t_len, ins_cost, del_cost, sub_cost);

	*SR = new char[op_len + 1];
	*OP = new char[op_len + 1];
	*TR = new char[op_len + 1];
	*Mem_Allocated = 3 * sizeof(char) * op_len;
	(*SR)[op_len] = (*OP)[op_len] = (*TR)[op_len] = '\0';

	//printf("op_len: %d \n", op_len);

	ED_handler(Table, SS, TS, s_len, t_len, ins_cost, del_cost, sub_cost, *SR, *OP, *TR, op_len);
}

int get_Trace_Number(int**& T, int i/*length of SS*/, int j/*length of TS*/, int& ins_cost, int& del_cost, int& sub_cost) {

	int cnt = 0;

	while (i > 0 || j > 0) {

		if (i == 0) j--;
		else if (j == 0) i--;
		else if (T[i][j - 1] + ins_cost == T[i][j] && T[i - 1][j] + del_cost == T[i][j]) {

			if (ins_cost <= del_cost) j--;
			else i--;
		}
		else if (T[i - 1][j] + del_cost == T[i][j]) i--;
		else if (T[i][j - 1] + ins_cost == T[i][j]) j--;
		else {
			i--;
			j--;
		}

		cnt++;
	}

	return cnt;
}

void ED_handler(int**& T, char*& SS, char*& TS, int s_idx, int t_idx, int& ins_cost, int& del_cost, int& sub_cost, char*& SR, char*& OP, char*& TR, int idx) {

	char sel = ' ';
	// sel:  0 - no-op,  1 - sub, 2- ins, 3 - del
	int i = s_idx;
	int j = t_idx;

	idx--;

	while (i > 0 || j > 0) {

		if (i == 0) sel = OP[idx] = 'i';
		else if (j == 0) sel = OP[idx] = 'd';
		else if (T[i][j - 1] + ins_cost == T[i][j] && T[i - 1][j] + del_cost == T[i][j]) {

			if (ins_cost <= del_cost) sel = OP[idx] = 'i';	// insert
			else sel = OP[idx] = 'd';	// delete
		}
		else if (T[i - 1][j] + del_cost == T[i][j]) sel = OP[idx] = 'd';	
		else if (T[i][j - 1] + ins_cost == T[i][j]) sel = OP[idx] = 'i';
		else {
			if (T[i][j] == T[i - 1][j - 1]) sel = OP[idx] = '.';
			else sel = OP[idx] = 's';
		}

		switch (sel) {
		case 'i':
			TR[idx] = TS[--j];
			SR[idx] = '*';
			break;
		case 'd':
			SR[idx] = SS[--i];
			TR[idx] = '*';
			break;
		case 's':
			SR[idx] = SS[--i];
			TR[idx] = TS[--j];
			break;
		case '.':
			SR[idx] = TR[idx] = SS[--i];
			j--;
			break;
		}

		idx--;
	}
}