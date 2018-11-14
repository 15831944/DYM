//矩阵运算函数（求逆、相乘、转置）
//add by lys 2015-05-01

#pragma once

#include "stdio.h"
#include "windows.h"
#include "math.h"
#include "malloc.h"

//矩阵求逆a(nn)
int Brinv(double a[], int n)
{
	int *is, *js, i, j, k, l, u, v;
	double d, p;
	is = (int*)malloc(n*sizeof(int));
	js = (int*)malloc(n*sizeof(int));
	for (k = 0; k <= n - 1; k++)
	{
		d = 0.0;
		for (i = k; i <= n - 1; i++)
		for (j = k; j <= n - 1; j++)
		{
			l = i*n + j; p = fabs(a[l]);
			if (p > d) { d = p; is[k] = i; js[k] = j; }
		}
		if (d + 1.0 == 1.0)
		{
			free(is); free(js); /*printf("err**not inv\n");*/
			return(0);
		}
		if (is[k] != k)
		for (j = 0; j <= n - 1; j++)
		{
			u = k*n + j; v = is[k] * n + j;
			p = a[u]; a[u] = a[v]; a[v] = p;
		}

		if (js[k] != k)
		for (i = 0; i <= n - 1; i++)
		{
			u = i*n + k; v = i*n + js[k];
			p = a[u]; a[u] = a[v]; a[v] = p;
		}
		l = k*n + k;
		a[l] = 1.0 / a[l];
		for (j = 0; j <= n - 1; j++)
		if (j != k)
		{
			u = k*n + j; a[u] = a[u] * a[l];
		}
		for (i = 0; i <= n - 1; i++)
		if (i != k)
		for (j = 0; j <= n - 1; j++)
		if (j != k)
		{
			u = i*n + j;
			a[u] = a[u] - a[i*n + k] * a[k*n + j];
		}
		for (i = 0; i <= n - 1; i++)
		if (i != k)
		{
			u = i*n + k; a[u] = -a[u] * a[l];
		}
	}
	for (k = n - 1; k >= 0; k--)
	{
		if (js[k] != k)
		for (j = 0; j <= n - 1; j++)
		{
			u = k*n + j; v = js[k] * n + j;
			p = a[u]; a[u] = a[v]; a[v] = p;
		}
		if (is[k] != k)
		for (i = 0; i <= n - 1; i++)
		{
			u = i*n + k; v = i*n + is[k];
			p = a[u]; a[u] = a[v]; a[v] = p;
		}
	}
	free(is);
	free(js);
	return(1);
}
//矩阵相乘a(mn)*b(nk)=c(mk)
void Brmul(double a[], double b[], int m, int n, int k, double c[])
{
	int i, j, l, u;
	for (i = 0; i <= m - 1; i++)
	for (j = 0; j <= k - 1; j++)
	{
		u = i*k + j; c[u] = 0.0;
		for (l = 0; l <= n - 1; l++)
			c[u] = c[u] + a[i*n + l] * b[l*k + j];
	}
	return;
}
//矩阵转置a(mn)-->b(nm)
void Transp(double a[], int m, int n, double b[])
{
	int i, j;
	for (i = 0; i < m; i++)
	for (j = 0; j < n; j++)
		b[i + m*j] = a[i*n + j];
}