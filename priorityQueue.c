#include <stdlib.h>
#include "header.h"
#include "priorityQueue.h"

/* Este módulo implementa uma fila priorizada em um heap.
// O heap é armazenado num vetor pq[1..N] de vértices. (A posição 0
// do vetor não é usada.) */

static int *pq;
static int N;

/* As prioridades são dadas em um vetor prty indexado por vértices:
// o vértice na posição k de pq tem prty[pq[k]]. O heap é
// caracterizado pela propriedade  prty[pq[k/2]] ≤ prty[pq[k]]
// para k = 2, ..., N. Portanto, o vértice pq[1] tem prioridade
// mínima. */

#define greater( i, j) (prty[pq[i]] > prty[pq[j]] ? 1 : 0)

/* O vetor qp é o "inverso" de pq: para cada vértice v,
// qp[v] é o único índice tal que pq[qp[v]] == v.
// É claro que qp[pq[i]] == i para todo i. */

static int *qp;

void PQinit( int maxN) {
   pq = malloc( (maxN+1) * sizeof (int));
   qp = malloc( maxN * sizeof (int));
   N = 0;
}

/* Supõe-se que teremos sempre N ≤ maxN. */

int PQempty( void) {
   return N == 0;
}

void PQinsert( int v, int prty[]) {
   qp[v] = ++N;
   pq[N] = v;
   fixUp( N, prty);
}

int PQdelmin( int prty[]) {
   exch( 1, N);
   --N;
   fixDown( 1, prty);
   return pq[N+1];
}

void PQdec( int w, int prty[]) {
   fixUp( qp[w], prty);
}

void exch( int i, int j) {
   int t;
   t = pq[i]; pq[i] = pq[j]; pq[j] = t;
   qp[pq[i]] = i;
   qp[pq[j]] = j;
}

void fixUp( int k, int prty[]) {
   while (k > 1 && greater( k/2, k)) {
      exch( k/2, k);
      k = k/2;
   }
}

void fixDown( int k, int prty[]) {
   int j;
   while (2*k <= N) {
      j = 2*k;
      if (j < N && greater( j, j+1)) j++;
      if (!greater( k, j)) break;
      exch( k, j);
      k = j;
   }
}

void PQfree( ) {
   free( pq);
   free( qp);
}