#include <stdlib.h>

void PQinit( int maxN);

int PQempty( void);

void PQinsert( int v, int prty[]);

int PQdelmin( int prty[]);

void PQdec( int w, int prty[]);

void exch( int i, int j);

void fixUp( int k, int prty[]);

void fixDown( int k, int prty[]);

void PQfree( );

