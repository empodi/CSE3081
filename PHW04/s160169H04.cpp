#include <cstdio>
#include <cstdlib>
#include <float.h>
#include <math.h>

void mergeSort(int low, int high, double arr[], unsigned Xid[], unsigned tmp[]);
void merge(int low, int mid, int high, double arr[], unsigned Xid[], unsigned tmp[]);
double getMinimumDistance(int L, int R, unsigned* pt1, unsigned* pt2, double* X, double* Y, unsigned* tmp, double c_Min);

void mergeSort(int low, int high, double arr[], unsigned Xid[], unsigned tmp[]) {

	int mid;

	if (low < high) {

		mid = (low + high) / 2;
		mergeSort(low, mid, arr, Xid, tmp);
		mergeSort(mid + 1, high, arr, Xid, tmp);
		merge(low, mid, high, arr, Xid, tmp);
	}
}

void merge(int low, int mid, int high, double arr[], unsigned Xid[], unsigned tmp[]) {
	// set Xid according to arr instead of sorting arr itself

	int i, j, k, t;

	i = low, j = mid + 1, k = low;

	while (i <= mid && j <= high) {

		if (arr[Xid[i]] < arr[Xid[j]])
			tmp[k++] = Xid[i++];
		else
			tmp[k++] = Xid[j++];
	}

	if (i > mid)
		for (t = j; t <= high; t++)
			tmp[k++] = Xid[t];

	else
		for (t = i; t <= mid; t++)
			tmp[k++] = Xid[t];

	for (t = low, k = low; t <= high; t++, k++)
		Xid[t] = tmp[k];
}

double getMinimumDistance(int L, int R, unsigned* pt1, unsigned* pt2, double* X, double* Y, unsigned* tmp, double c_Min) {
	//input:	L, R - current leftmost and rightmost indices
	//			pt1, pt2 - closest pair points indices
	//			X, Y - (x,y) positions array
	//			tmp - indices array sorted according to y position (Yid/TMP)
	//			c_Min - current minimum distance value
	//return minimum distance
	int i, j;
	double dist;
	double dMin = c_Min;

	for (i = L; i < R; i++)			// get Minimun Distance
		for (j = i + 1; j <= R; j++) {

			double x_dis = X[tmp[i]] - X[tmp[j]];
			double y_dis = Y[tmp[j]] - Y[tmp[i]];

			if (y_dis > dMin) break;

			dist = sqrt((x_dis * x_dis) + (y_dis * y_dis));

			if (dist < dMin) {
				dMin = dist;
				*pt1 = tmp[i];
				*pt2 = tmp[j];
			}
		}

	return dMin;
}

void sortXid(double* X, unsigned* Xid, unsigned* TMP, unsigned N) {
	// input  : X[]   (x position array of N points)
	//          Xid[] (index array of X (initially [0,1,...,N-1])
	//          TMP[] (temporary array of size N. may or may not be used)
	//          N   number of points to sort
	// output : Xid[] (sorted indices by x position in X[])

	mergeSort(0, N - 1, X, Xid, TMP);
}

double closestPairDC(
	unsigned L, unsigned R,	  // current leftmost and rightmost indices
	unsigned* pt1, unsigned* pt2, // closest pair points indices
	double* X, double* Y,         // (x,y) positions array(input)
	unsigned* Xid,  // point index array(sorted by x coordinates)
	unsigned* Yid,  // point index array(gradually sorted by y coordinates)
	unsigned* TMP,  // temporal index array
	unsigned THR	// threshold value
) {
	// Input : L, R, X[], Y[], Xid[]
	// Output : pt1, pt2 ( the distance between (X[pt1],Y[pt1]) and
	//                     (X[pt2],Y[pt2]) is the closest )
	//          return the distance value
	// Others ; Yid[], TMP[] (scratch arrays for D&C algorithm)

	int i, j, tIdx = 0;	// tIdx is a index # of TMP Array
	double dist, dMin = DBL_MAX;

	if (R - L + 1 <= THR) {	// less than threshold - brute force

		for (i = L; i <= R; i++)
			Yid[i] = Xid[i];			

		mergeSort(L, R, Y, Yid, TMP);	// sort Yid according to Y

		return getMinimumDistance(L, R, pt1, pt2, X, Y, Yid, DBL_MAX);  // get minimum distance by Yid array

	}

	int M = (L + R) / 2;
	unsigned pt_L1, pt_L2, pt_R1, pt_R2;
	double xMid;

	double d_L = closestPairDC(L, M, &pt_L1, &pt_L2, X, Y, Xid, Yid, TMP, THR);
	double d_R = closestPairDC(M + 1, R, &pt_R1, &pt_R2, X, Y, Xid, Yid, TMP, THR);

	if (d_L < d_R) {		// set dMin, pt1, pt2
		dMin = d_L;
		*pt1 = pt_L1;
		*pt2 = pt_L2;
	}
	else {
		dMin = d_R;
		*pt1 = pt_R1;
		*pt2 = pt_R2;
	}

	merge(L, M, R, Y, Yid, TMP);		// rearrange Yid

	xMid = (X[Xid[M]] + X[Xid[M + 1]]) / 2;

	for (i = L; i <= R; i++)
		if (X[Yid[i]] >= xMid - dMin /* lower bound */
			&& X[Yid[i]] <= xMid + dMin /* upper bound */)
			TMP[tIdx++] = Yid[i];

	return getMinimumDistance(0, tIdx - 1, pt1, pt2, X, Y, TMP, dMin);	// get minimum distance by TMP array
}