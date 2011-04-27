#include "fffpy.h"
#include <fff_graphlib.h>
#include "fff_array.h"

#define PY_ARRAY_UNIQUE_SYMBOL PyArray_API


/* doc */ 

static char graph_knn_doc[] = 
" (A,B,D) = graph_knn(X,k)\n\
  Building the k-nearest-neighbours graph of the data \n\
INPUT:\n\
The array X is assumed to be a n*p feature matrix \n\
where n is the number of features \n\
and p is the dimension of the features \n\
It is assumed that the features are embedded in a (locally) Euclidian space \n\
k is the number of neighbours considered\n\
OUTPUT:\n\
The edges of the resulting (directed) graph are defined through the triplet of 1-d arrays \n\
A,B,D such that [A[e] B[e]] are the vertices D[e] = ||A[e]-B[e]|| Euclidian.\n\
NB:\n\
- the knn system is symmeterized: if (A[e] B[e] D[e]) is one of the edges \n\
then (B[e] A[e] D[e]) is another edge \n\
- trivial neighbours (v v 0) are not included.\n\
- for the sake of speed it is advisable to give a PCA-preprocessed matrix X.\n\
  ";

static char graph_eps_doc[] = 
" (A,B,D) = graph_eps(X,eps)\n\
Building the epsilon-nearest-neighbours graph of the data\n\
INPUT:\n\
The array X is assumed to be a n*p feature matrix \n\
where n is the number of features \n\
and p is the dimension of the features \n\
It is assumed that the features are embedded in a (locally) Euclidian space \n\
epsilon is the number of neighbourood size considered \n\
OUTPUT:\n\
The edges of the resulting (directed) graph are defined through the triplet of 1-d arrays \n\
A,B,D such that [A[e] B[e]] are the vertices D[e] = ||A[e]-B[e]|| Euclidian.\n\
NB:\n\
- trivial neighbours (v v 0) are included.\n\
- for the sake of speed it is advisable to give a PCA-preprocessed matrix X.\n\
";

static char graph_cross_knn_doc[] = 
" (A,B,D) = graph_cross_knn(X,Y,k)\n\
  Building the cross-knn graph of the data \n\
INPUT:\n\
The arrays X  and Y is assumed to be a n1*p and n2*p feature matrices \n\
where (n1,n2) are the number of features in either candy \n\
and p is the dimension of the features \n\
It is assumed that the features are embedded in a (locally) Euclidian space \n\
 is the number of neighbours considered \n\
OUTPUT:\n\
The edges of the resulting (directed) graph are defined through the triplet of 1-d arrays \n\
A,B,D such that [A[e] B[e]] are the vertices D[e] = ||A[e]-B[e]|| Euclidian.\n\
NB:\n\
- for the sake of speed it is advisable to give PCA-preprocessed matrices X and Y. \n\
  ";

static char graph_cross_eps_doc[] = 
" (A,B,D) = graph_cross_eps(X,Y,eps)\n\
  Building the cross_eps graph of the data \n\
INPUT:\n\
The arrays X  and Y is assumed to be a n1*p and n2*p feature matrices \n\
where (n1,n2) are the number of features in either candy \n\
and p is the dimension of the features \n\
It is assumed that the features are embedded in a (locally) Euclidian space \n\
epsilon is the number of neighbourood size considered \n\
OUTPUT:\n\
The edges of the resulting (directed) graph are defined through the triplet of 1-d arrays \n\
A,B,D such that [A[e] B[e]] are the vertices D[e] = ||A[e]-B[e]|| Euclidian.\n\
NB:\n\
- for the sake of speed it is advisable to give PCA-preprocessed matrices X and Y. \n\
  ";

static char graph_cross_eps_robust_doc[] = 
" (A,B,D) = graph_cross_eps_robust(X,Y,eps)\n\
  Building the cross_eps graph of the data \n\
INPUT:\n\
The arrays X  and Y is assumed to be a n1*p and n2*p feature matrices \n\
where (n1,n2) are the number of features in either candy \n\
and p is the dimension of the features \n\
It is assumed that the features are embedded in a (locally) Euclidian space \n\
epsilon is the number of neighbourood size considered \n\
OUTPUT:\n\
The edges of the resulting (directed) graph are defined through the triplet of 1-d arrays \n\
A,B,D such that [A[e] B[e]] are the vertices D[e] = ||A[e]-B[e]|| Euclidian.\n\
NB:\n\
- for the sake of speed it is advisable to give PCA-preprocessed matrices X and Y. \n\
  ";

static char graph_mst_doc[] = 
" (A,B,D) = graph_mst(X)\n\
  Building the MST of the data \n\
INPUT:\n\
The array X is assumed to be a n*p feature matrix \n\
where n is the number of features \n\
and p is the dimension of the features \n\
It is assumed that the features are embedded in a (locally) Euclidian space \n\
OUTPUT:\n\
The edges of the resulting (directed) graph are defined through the triplet of 1-d arrays \n\
A,B,D such that [A[e] B[e]] are the vertices D[e] = ||A[e]-B[e]|| Euclidian.\n\
NB:\n\
- The edge system is symmeterized: if (A[e] B[e] D[e]) is one of the edges \n\
then (B[e] A[e] D[e]) is another edge \n\
- As a consequence, the graph comprises (2n-2) edges \n\
  ";

static char graph_skeleton_doc[] = 
" (A,B,D) = graph_mst(A1,B1,D1)\n\
  Building the MST of the data \n\
INPUT:\n\
The array X is assumed to be a n*p feature matrix \n\
where n is the number of features \n\
and p is the dimension of the features \n\
It is assumed that the features are embedded in a (locally) Euclidian space \n\
OUTPUT:\n\
The edges of the resulting (directed) graph are defined through the triplet of 1-d arrays \n\
A,B,D such that [A[e] B[e]] are the vertices D[e] = ||A[e]-B[e]|| Euclidian.\n\
NB:\n\
- The edge system is symmeterized: if (A[e] B[e] D[e]) is one of the edges \n\
then (B[e] A[e] D[e]) is another edge \n\
- As a consequence, the graph comprises (2n-2) edges \n\
  ";

static char graph_3d_grid_doc[] = 
" (A,B,D) = graph_3d_grid(XYZ,k)\n\
  Building the 6-nn, 18-nn or 26nn of the data, \n\
which are sampled on a three-dimensional grid \n\
 INPUT:\n\
- The array XYZ is assumed to be a n*3 coordinate matrix \n\
which are assumed to have integer values.  \n\
- k (=6 or 18 or 26) is the neighboring system considered \n\
 OUTPUT:\n\
The edges of the resulting (directed) graph are defined through the triplet of 1-d arrays \n\
A,B,D such that [A[e] B[e]] are the vertices D[e] = ||A[e]-B[e]|| Euclidian.\n\
NB:\n\
- The edge system is symmeterized: if (A[e] B[e] D[e]) is one of the edges \n\
then (B[e] A[e] D[e]) is another edge \n\
- trivial edges are included, and have distance 0.\n\
  ";

static char graph_normalize_doc[] = 
" a,b,d,s = graph_normalize(a,b,d,c=0,V)\n\
Normalize the graph according to the index c\n\
Normalization means that the sum of the edges values \n\
that go into or out each vertex must sum to 1 \n\
INPUT:\n\
- The edges of the input graph are defined through the couple of 1-d arrays \n\
A,B such that [A[e] B[e]] are the vertices and D[e] an associated attribute \n\
(distance/weight/affinity) \n\
- c is an index that designates the array \n\
according to which D is normalized \n\
It is an optional argument, by default c = 0\n\
c == 0 => for each vertex a, sum{A[e]=a} D[e]=1 \n\
c == 1 => for each vertex b, sum{B[e]=b} D[e]=1 \n\
c == 2 => a symmetric normalization is performed \n\
- V is the numner of vertices of the graph\n\
It is an optional argument, by default v = max(max(a),max(b))+1\n\
OUTPUT:\n\
- The arrays A,B,D normalized as required \n\
- s: the values sum{A[e]=a} D[e](resp sum{B[e]=b} D[e]) \n\
- if c==2, t= sum{B[e]=a} D[e]\n\
note that when sum(A[e]=a) D[e]=0, nothing is performed \n\
";

static char graph_cc_doc[] = 
" label = graph_cc(a,b,d,V)\n\
  returns the connected components as labels.\n\
  the graph is assumed symmetric \n\
  INPUT:\n\
- The edges of the input graph are defined through the couple of 1-d arrays \n\
A,B such that [A[e] B[e]] are the vertices and D[e] an associated attribute \n\
(distance/weight/affinity) \n\
- V is the numner of vertices of the graph\n\
It is an optional argument, by default v = max(max(a),max(b))+1\n\
OUTPUT:\n\
- label is a length V label vector\n\
  ";

static char graph_dijkstra_doc[] = 
" dg = graph_dijkstra(a,b,d,seed,V)\n\
  returns all the geodesic distances starting from seed\n\
  d>=0 is mandatory c\n\
 INPUT:\n\
- The edges of the input graph are defined through the couple of 1-d arrays \n\
A,B such that [A[e] B[e]] are the vertices and D[e] an associated attribute \n\
(distance/weight/affinity) \n\
- seed is the edge from which the distances are computed \n\
- V is the numner of vertices of the graph\n\
It is an optional argument, by default v = max(max(a),max(b))+1\n\
OUTPUT:\n\
- the graph distance dg from the seed to any edge \n\
  ";

static char graph_dijkstra_multiseed_doc[] = 
" dg = graph_dijkstra_multiseed(a,b,d,seed,V)\n\
  returns all the geodesic distances starting from seed\n\
  d>=0 is mandatory c\n\
 INPUT:\n\
- The edges of the input graph are defined through the couple of 1-d arrays \n\
A,B such that [A[e] B[e]] are the vertices and D[e] an associated attribute \n\
(distance/weight/affinity) \n\
- seed is the vector of edges from which the distances are computed \n\
- V is the numner of vertices of the graph\n\
It is an optional argument, by default v = max(max(a),max(b))+1\n\
OUTPUT:\n\
- the graph distance dg from the seed to any edge \n\
  ";

static char graph_floyd_doc[] = 
" dg = graph_floyd(a,b,d,seed=NULL,V)\n\
  returns all the geodesic distances starting from seeds\n\
  d>=0 is mandatory and checked in the function c\n\
INPUT:\n\
- The edges of the input graph are defined through the couple of 1-d arrays \n\
A,B such that [A[e] B[e]] are the vertices and D[e] an associated attribute \n\
(distance/weight/affinity) \n\
- seed is an arry of  edges from which the distances are computed \n\
if seed==NULL, then every edge is a seed point\n\
- V is the numner of vertices of the graph\n\
It is an optional argument, by default v = max(max(a),max(b))+1\n\
OUTPUT:\n\
- the graph distance dg from each seed to any edge \n\
Note that it has size (nbseed,nbedges)\n\
  ";

static char graph_voronoi_doc[] = 
" label = graph_voronoi(a,b,d,seed,V)\n\
  performs a voronoi labelling of the graph \n\
  d>=0 is mandatory ; it is checked in the function c\n \
INPUT:\n\
- The edges of the input graph are defined through the couple of 1-d arrays \n\
A,B such that [A[e] B[e]] are the vertices and D[e] an associated attribute \n\
(distance/weight/affinity) \n\
- seed is an arry of  edges from which the distances are computed \n\
- V is the numner of vertices of the graph\n\
It is an optional argument, by default v = max(max(a),max(b))+1\n\
OUTPUT:\n\
- the voronoi labelling of the vertices \n\
Note that it has the size (nbseed)\n\
  ";

static char graph_rd_doc[] = 
" cliques = graph_voronoi(a,b,d,V)\n\
  performs a clique extraction of the graph using replicator dynamics  \n\
  d>=0 is mandatory ; it is checked in the function c\n \
INPUT:\n\
- The edges of the input graph are defined through the couple of 1-d arrays \n\
A,B such that [A[e] B[e]] are the vertices and D[e] an associated attribute \n\
(distance/weight/affinity) \n\
- V is the numner of vertices of the graph\n\
It is an optional argument, by default v = max(max(a),max(b))+1\n\
OUTPUT:\n\
- the labelling of the vertices according to the clique they belong to \n\
Note that it has the size (V)\n\
  ";

static char module_doc[] = 
" Graph routines.\n\
Author: Bertrand Thirion (INRIA Futurs, Orsay, France), 2004-2008.";

static double _fff_array_max1d(const fff_array *farray);
static double _fff_array_max1d(const fff_array *farray)
{
  // returns the index of the max value on a supposedly 1D array
  // quick and dirty implementation
  long i,n = farray->dimX;
  double val,max = (double) fff_array_get1d(farray,0);
  
  for (i=0 ; i<n ; i++){
	val = (double) fff_array_get1d(farray,i);
	if (val>max)
	  max = val;
  }
  return max;
}

/****************************************************************
 ************ Part 1 : creating graphs **************************
 ***************************************************************/

static PyObject* graph_knn(PyObject* self, PyObject* args)
{

  PyArrayObject *x, *a, *b, *d;
  int E,k;

  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!i:graph_knn", 
			  &PyArray_Type, &x, 
			  &k); 
    if (!OK) Py_RETURN_NONE; 

  /* prepare C arguments */
  fff_matrix* X = fff_matrix_fromPyArray( x ); 
  fff_graph *G;

  /* do the job */
  E = fff_graph_knn(&G, X, k); 

  fff_array *A = fff_array_new1d(FFF_LONG,E);
  fff_array *B = fff_array_new1d(FFF_LONG,E);
  fff_vector *D = fff_vector_new(E);

  fff_graph_edit_safe(A,B,D,G);
  fff_graph_delete(G);
  fff_matrix_delete(X);

  /* get the results as python arrrays*/
  a = fff_array_toPyArray( A );
  b =  fff_array_toPyArray( B );
  d = fff_vector_toPyArray( D );

  /* Output tuple */
  PyObject* ret = Py_BuildValue("NNN", 
				a, 
				b,
				d); 

  return ret;
}

static PyObject* graph_cross_knn(PyObject* self, PyObject* args)
{
  PyArrayObject *x, *y, *a, *b, *d;
  int k;

  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!O!i:graph_crossknn", 
			  &PyArray_Type, &x, 
			  &PyArray_Type, &y, 
			  &k); 
  if (!OK) Py_RETURN_NONE; 
 

  /* prepare C arguments */
  fff_matrix* X = fff_matrix_fromPyArray( x ); 
  fff_matrix* Y = fff_matrix_fromPyArray( y ); 
  int V = X->size1; 
  int E = k*V;
  fff_graph *G = fff_graph_new(V,E);
  fff_array *A = fff_array_new1d(FFF_LONG,E);
  fff_array *B = fff_array_new1d(FFF_LONG,E);
  fff_vector *D = fff_vector_new(E);
  
  /* do the job */
  E = fff_graph_cross_knn(G, X, Y, k);  
  fff_graph_edit_safe(A,B,D,G);
  fff_graph_delete(G);
  fff_matrix_delete(X);
  fff_matrix_delete(Y);

  /* get the results as python arrrays*/
  a = fff_array_toPyArray( A );
  b = fff_array_toPyArray( B );
  d = fff_vector_toPyArray( D );
  

  /* Output tuple */
  PyObject* ret = Py_BuildValue("NNN", 
				a, 
				b,
				d); 
  
  return ret;
}

static PyObject* graph_eps(PyObject* self, PyObject* args)
{
  PyArrayObject *x, *a, *b, *d;
  int E;
  double eps;

  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!d:graph_eps", 
			  &PyArray_Type, &x, 
			  &eps); 
  if (!OK) Py_RETURN_NONE; 
  
  /* prepare C arguments */
  fff_matrix* X = fff_matrix_fromPyArray( x ); 
  fff_graph *G;
  
  /* do the job */
  E = fff_graph_eps(&G, X, eps); 
  fff_array *A = fff_array_new1d(FFF_LONG,E);
  fff_array *B = fff_array_new1d(FFF_LONG,E);
  fff_vector *D = fff_vector_new(E);
  
  fff_graph_edit_safe(A,B,D,G);
  fff_graph_delete(G);
  fff_matrix_delete(X);

  /* get the results as python arrrays*/
  a = fff_array_toPyArray( A );
  b =  fff_array_toPyArray( B );
  d = fff_vector_toPyArray( D );
  
  /* Output tuple */
  PyObject* ret = Py_BuildValue("NNN", 
				a, 
				b,
				d); 
  
  return ret;
}

static PyObject* graph_cross_eps(PyObject* self, PyObject* args)
{
  PyArrayObject *x, *y, *a, *b, *d;
  int E;
  double eps;

  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!O!d:graph_cross_eps", 
			  &PyArray_Type, &x, 
			  &PyArray_Type, &y,
			  &eps); 
  if (!OK) Py_RETURN_NONE; 
  
  /* prepare C arguments */
  fff_matrix* X = fff_matrix_fromPyArray( x );
  fff_matrix* Y = fff_matrix_fromPyArray( y );
  fff_graph *G;
  
  /* do the job */
  E = fff_graph_cross_eps(&G, X, Y, eps); 
  fff_array *A = fff_array_new1d(FFF_LONG,E);
  fff_array *B = fff_array_new1d(FFF_LONG,E);
  fff_vector *D = fff_vector_new(E);
  
  fff_graph_edit_safe(A,B,D,G);
  fff_graph_delete(G);
  fff_matrix_delete(X);
  fff_matrix_delete(Y);

  /* get the results as python arrrays*/
  a = fff_array_toPyArray( A );
  b =  fff_array_toPyArray( B );
  d = fff_vector_toPyArray( D );
  
  /* Output tuple */
  PyObject* ret = Py_BuildValue("NNN", 
				a, 
				b,
				d); 
  
  return ret;
}

static PyObject* graph_cross_eps_robust(PyObject* self, PyObject* args)
{
  PyArrayObject *x, *y, *a, *b, *d;
  int E;
  double eps;

  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!O!d:graph_cross_eps_robust", 
			  &PyArray_Type, &x, 
			  &PyArray_Type, &y,
			  &eps); 
  if (!OK) Py_RETURN_NONE; 
  
  /* prepare C arguments */
  fff_matrix* X = fff_matrix_fromPyArray( x );
  fff_matrix* Y = fff_matrix_fromPyArray( y );
  fff_graph *G;
  
  /* do the job */
  E = fff_graph_cross_eps_robust(&G, X, Y, eps); 
  fff_array *A = fff_array_new1d(FFF_LONG,E);
  fff_array *B = fff_array_new1d(FFF_LONG,E);
  fff_vector *D = fff_vector_new(E);
  
  fff_graph_edit_safe(A,B,D,G);
  fff_graph_delete(G);
  fff_matrix_delete(X);
  fff_matrix_delete(Y);

  /* get the results as python arrrays*/
  a = fff_array_toPyArray( A );
  b =  fff_array_toPyArray( B );
  d = fff_vector_toPyArray( D );
  
  /* Output tuple */
  PyObject* ret = Py_BuildValue("NNN", 
				a, 
				b,
				d); 
  
  return ret;
}

static PyObject* graph_mst(PyObject* self, PyObject* args)
{
  PyArrayObject *x, *a, *b, *d;
  
  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!:graph_mst", 
			  &PyArray_Type, &x); 
  if (!OK) Py_RETURN_NONE; 
  
  /* prepare C arguments */
  fff_matrix* X = fff_matrix_fromPyArray( x );
  int V = X->size1; 
  int E = 2*(V-1);
  fff_graph *G = fff_graph_new(V,E);
  fff_array *A = fff_array_new1d(FFF_LONG,E);
  fff_array *B = fff_array_new1d(FFF_LONG,E);
  fff_vector *D = fff_vector_new(E);

  /* do the job */
  fff_graph_MST(G, X);   
  fff_graph_edit_safe(A,B,D,G);
  fff_graph_delete(G);
  fff_matrix_delete(X);

  /* get the results as python arrrays*/
  a = fff_array_toPyArray( A );
  b =  fff_array_toPyArray( B );
  d = fff_vector_toPyArray( D );
  
  /* Output tuple */
  PyObject* ret = Py_BuildValue("NNN", 
				a, 
				b,
				d); 
  
  return ret;
}

static PyObject* graph_skeleton(PyObject* self, PyObject* args)
{
  PyArrayObject *a, *b, *d;
  int V;
  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!O!O!|i:graph_skeleton", 
			     &PyArray_Type, &a,
			     &PyArray_Type, &b,
			     &PyArray_Type, &d,
							 &V); 
  if (!OK) Py_RETURN_NONE; 
  
  /* prepare C arguments */
  fff_array* A = fff_array_fromPyArray( a ); 
  fff_array* B = fff_array_fromPyArray( b );
  fff_vector* D = fff_vector_fromPyArray(d);
  int E = A->dimX;

  /* do the job */
  fff_graph *G = fff_graph_build_safe(V,E,A,B,D);
  
  fff_array_delete(A);
  fff_array_delete(B);
  fff_vector_delete(D);

  E = 2*(V-1);
  fff_graph *K = fff_graph_new(V,E);

 /* do the job */
  fff_graph_skeleton(K, G);   
  A = fff_array_new1d(FFF_LONG,E);
  B = fff_array_new1d(FFF_LONG,E);
  D = fff_vector_new(E);  
  
  fff_graph_edit_safe(A,B,D,K);
  fff_graph_delete(G);
  fff_graph_delete(K);

  /* get the results as python arrrays*/
  a = fff_array_toPyArray( A );
  b =  fff_array_toPyArray( B );
  d = fff_vector_toPyArray( D );
  
  /* Output tuple */
  PyObject* ret = Py_BuildValue("NNN", 
				a, 
				b,
				d); 
  
  return ret;
}

static PyObject* graph_3d_grid(PyObject* self, PyObject* args)
{
  PyArrayObject *xyz, *a, *b, *d;
  int E,k=18;

  /* Parse input */ 
  
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!|i:graph_3d_grid", 
			  &PyArray_Type, &xyz, 
			  &k); 
  if (!OK) Py_RETURN_NONE; 
  
  fff_array* XYZ = fff_array_fromPyArray( xyz );   
  fff_graph *G;

  E = fff_graph_grid(&G,XYZ, k);
  
  if (E == -1) {
      FFF_WARNING("Graph creation failed");
      Py_RETURN_NONE;
  }


  fff_array_delete(XYZ);

  fff_array *A = fff_array_new1d(FFF_LONG,E);
  fff_array *B = fff_array_new1d(FFF_LONG,E);
  fff_vector *D = fff_vector_new(E);

  fff_graph_edit_safe(A,B,D,G);
  fff_graph_delete(G);
  /* get the results as python arrrays*/
  a = fff_array_toPyArray( A );
  b = fff_array_toPyArray( B );
  d = fff_vector_toPyArray( D );
  
  /* Output tuple */
  PyObject* ret = Py_BuildValue("NNN", 
				a, 
				b,
				d); 
  
  return ret;
} 

/****************************************************************
 ************ Part 2 : graph analysis ***************************
 ***************************************************************/

static PyObject* graph_normalize(PyObject* self, PyObject* args)
{
  PyArrayObject *a, *b, *d, *s, *t;
  int eA,eB,V=0;
  int c=0;

  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!O!O!|ii:graph_normalize", 
			     &PyArray_Type, &a,
			     &PyArray_Type, &b,
			     &PyArray_Type, &d,
			     &c,			     
                             &V
                             ); 
  if (!OK) Py_RETURN_NONE; 
    
  /* prepare C arguments */
  fff_array* A = fff_array_fromPyArray( a ); 
  fff_array* B = fff_array_fromPyArray( b );
  fff_vector* D = fff_vector_fromPyArray(d);
  int E = A->dimX;
  if (V<1){
     eA = (int)_fff_array_max1d(A)+1;
     eB = (int)_fff_array_max1d(B)+1;
     if (eA>V) V = eA;
     if (eB>V) V = eB;
   }

  /* do the job */
  fff_graph *G = fff_graph_build_safe(V,E,A,B,D);
  fff_vector* S = fff_vector_new(V);
  fff_vector* T = NULL;
  
  if (c>2) c=0;

  switch (c)
    {
    case 0:{
      fff_graph_normalize_rows(G,S);
      break;}
    case 1:{
      fff_graph_normalize_columns(G,S);
      break;}
	case 2:{
	  T = fff_vector_new(V);
      fff_graph_normalize_symmetric(G,S,T);
      break;}
    }
  fff_graph_edit_safe(A,B,D,G);
  fff_graph_delete(G);
  
  /* get the results as python arrrays*/
  s = fff_vector_toPyArray( S );
  a = fff_array_toPyArray( A );
  b = fff_array_toPyArray( B );
  d = fff_vector_toPyArray( D );

  /* Output tuple */
  PyObject* ret;
  if (c<2)ret = Py_BuildValue("NNNN",a,b,d,s);
  else{
	t = fff_vector_toPyArray( T );
	ret = Py_BuildValue("NNNNN",a,b,d,s,t);
  }
	
  return ret;
}

static PyArrayObject* graph_cc(PyObject* self, PyObject* args)
{
  PyArrayObject *a, *b, *d, *l;
  int eA,eB,V=0;

  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!O!O!|i:graph_cc", 
			     &PyArray_Type, &a,
			     &PyArray_Type, &b,
			     &PyArray_Type, &d,
			     &V
			     ); 
  if (!OK) return NULL; 
    
  /* prepare C arguments */
  fff_array* A = fff_array_fromPyArray( a ); 
  fff_array* B = fff_array_fromPyArray( b );
  fff_vector* D = fff_vector_fromPyArray(d);
  int E = A->dimX;
  if (V<1){
     eA = (int)_fff_array_max1d(A)+1;
     eB = (int)_fff_array_max1d(B)+1;
     if (eA>V) V = eA;
     if (eB>V) V = eB;
   }
  fff_array *label = fff_array_new1d(FFF_LONG,V);
  
  /* do the job */
  fff_graph *G = fff_graph_build_safe(V,E,A,B,D);
  fff_array_delete(A);
  fff_array_delete(B);
  fff_vector_delete(D);
  
  fff_graph_cc_label(label->data,G);
  fff_graph_delete(G);
  
  /* get the results as python arrrays*/
  l = fff_array_toPyArray( label );
  
  return l;
}

static PyArrayObject* graph_dijkstra(PyObject* self, PyObject* args)
{
  PyArrayObject *a, *b, *d, *m;
  int seed;
  int eA, eB, V = 0;

  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!O!O!i|i:graph_dijkstra", 

			     &PyArray_Type, &a,
			     &PyArray_Type, &b,
			     &PyArray_Type, &d,
			     &seed,
			     &V
			     ); 
  if (!OK) return NULL; 
    
  /* prepare C arguments */
  fff_array* A = fff_array_fromPyArray( a ); 
  fff_array* B = fff_array_fromPyArray( b );
  fff_vector* D = fff_vector_fromPyArray(d);
  int E = A->dimX;
  if (V<1){
     eA = (int)_fff_array_max1d(A)+1;
     eB = (int)_fff_array_max1d(B)+1;
     if (eA>V) V = eA;
     if (eB>V) V = eB;
  }

  fff_vector *gd = fff_vector_new(V);
  
  /* do the job */
  fff_graph *G = fff_graph_build_safe(V,E,A,B,D);
  fff_array_delete(A);
  fff_array_delete(B);
  fff_vector_delete(D);
  

  fff_graph_dijkstra(gd->data, G, seed);
  fff_graph_delete(G);
  
  /* get the results as python arrrays*/
  m = fff_vector_toPyArray( gd);
  
  return m;
}

static PyArrayObject* graph_dijkstra_multiseed(PyObject* self, PyObject* args)
{
  PyArrayObject *a, *b, *d, *m, *seed;
  int eA, eB, V = 0;

  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!O!O!O!|i:graph_dijkstra_multiseed", 
			     &PyArray_Type, &a,
			     &PyArray_Type, &b,
			     &PyArray_Type, &d,
			     &PyArray_Type, &seed,
			     &V
			     ); 
  if (!OK) return NULL; 
    
  /* prepare C arguments */
  fff_array* A = fff_array_fromPyArray( a ); 
  fff_array* B = fff_array_fromPyArray( b );
  fff_vector* D = fff_vector_fromPyArray(d);
  fff_array* Seed = fff_array_fromPyArray( seed );
  int E = A->dimX;
  if (V<1){
     eA = (int)_fff_array_max1d(A)+1;
     eB = (int)_fff_array_max1d(B)+1;
     if (eA>V) V = eA;
     if (eB>V) V = eB;
  }

  fff_vector *gd = fff_vector_new(V);
  
  /* do the job */
  fff_graph *G = fff_graph_build_safe(V,E,A,B,D);
  fff_array_delete(A);
  fff_array_delete(B);
  fff_vector_delete(D);
  
  fff_graph_Dijkstra_multiseed(gd, G, Seed);
  fff_graph_delete(G);
  fff_array_delete(Seed);
  
  /* get the results as python arrrays*/
  m = fff_vector_toPyArray( gd);
  
  return m;
}

static PyArrayObject* graph_floyd(PyObject* self, PyObject* args)
{
  PyArrayObject *a, *b, *d, *m, *seed;
  int eA, eB, V = 0;
  int ns=0;
  seed = NULL;
  fff_matrix *gd ;
  fff_array* seeds = NULL;

  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!O!O!|O!i:graph_floyd", 
			     &PyArray_Type, &a,
			     &PyArray_Type, &b,
			     &PyArray_Type, &d,
			     &PyArray_Type, &seed,
			     &V
			     ); 
  if (!OK) return NULL;   

  /* prepare C arguments */
  fff_array* A = fff_array_fromPyArray( a ); 
  fff_array* B = fff_array_fromPyArray( b );
  fff_vector* D = fff_vector_fromPyArray(d);
  int E = A->dimX;
  if (V<1){
     eA = (int)_fff_array_max1d(A)+1;
     eB = (int)_fff_array_max1d(B)+1;
     if (eA>V) V = eA;
     if (eB>V) V = eB;
   }
  /* do the job */
  
  fff_graph *G = fff_graph_build_safe(V,E,A,B,D);
  fff_array_delete(A);
  fff_array_delete(B);
  fff_vector_delete(D);
  
  if (seed==NULL){
    gd = fff_matrix_new(V,V); 
    fff_graph_Floyd(gd,G);
  }
  else{ 
    seeds = fff_array_fromPyArray( seed );
    ns = seeds->dimX;
    
    gd = fff_matrix_new(ns,V);
        
    fff_graph_partial_Floyd(gd,G,seeds->data);
    
    fff_array_delete(seeds);
    
    }
  fff_graph_delete(G);
  
  /* get the results as python arrrays*/
  m = fff_matrix_toPyArray( gd);
  
  return m;
}

static PyArrayObject* graph_voronoi(PyObject* self, PyObject* args)
{
  PyArrayObject *a, *b, *d, *l, *seed;
  int eA, eB, V = 0;
  fff_array* seeds;

  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!O!O!O!|i:graph_voronoi", 
			     &PyArray_Type, &a,
			     &PyArray_Type, &b,
			     &PyArray_Type, &d,
			     &PyArray_Type, &seed,
			     &V
			     ); 
  if (!OK) return NULL;   

  /* prepare C arguments */
  fff_array* A = fff_array_fromPyArray( a ); 
  fff_array* B = fff_array_fromPyArray( b );
  fff_vector* D = fff_vector_fromPyArray(d);
  int E = A->dimX;
  if (V<1){
     eA = (int)_fff_array_max1d(A)+1;
     eB = (int)_fff_array_max1d(B)+1;
     if (eA>V) V = eA;
     if (eB>V) V = eB;
   }
  /* do the job */
  
  fff_graph *G = fff_graph_build_safe(V,E,A,B,D);
  fff_array_delete(A);
  fff_array_delete(B);
  fff_vector_delete(D);
  
  fff_array *label = fff_array_new1d(FFF_LONG,V); 
  seeds = fff_array_fromPyArray( seed );
  
  fff_graph_voronoi(label,G,seeds);
  
  fff_array_delete(seeds);
  fff_graph_delete(G);
  
  /* get the results as python arrrays*/
  l = fff_array_toPyArray( label);
  
  return l;
}

static PyArrayObject* graph_rd(PyObject* self, PyObject* args)
{
  PyArrayObject *a, *b, *d, *l;
  int eA, eB, V = 0;
  
  /* Parse input */ 
  /* see http://www.python.org/doc/1.5.2p2/ext/parseTuple.html*/
  int OK = PyArg_ParseTuple( args, "O!O!O!|i:graph_rd", 
			     &PyArray_Type, &a,
			     &PyArray_Type, &b,
			     &PyArray_Type, &d,
			     &V
			     ); 
  if (!OK) return NULL;   

  /* prepare C arguments */
  fff_array* A = fff_array_fromPyArray( a ); 
  fff_array* B = fff_array_fromPyArray( b );
  fff_vector* D = fff_vector_fromPyArray(d);
  int E = A->dimX;
  if (V<1){
     eA = (int)_fff_array_max1d(A)+1;
     eB = (int)_fff_array_max1d(B)+1;
     if (eA>V) V = eA;
     if (eB>V) V = eB;
   }
  /* do the job */
  
  fff_graph *G = fff_graph_build_safe(V,E,A,B,D);
  fff_array_delete(A);
  fff_array_delete(B);
  fff_vector_delete(D);
  
  fff_array *label = fff_array_new1d(FFF_LONG,V); 
    
  fff_graph_cliques(label,G);
  
  fff_graph_delete(G);
  /* get the results as python arrrays*/
  l = fff_array_toPyArray( label);
  
  return l;
}


static PyMethodDef module_methods[] = {
   {"graph_knn",           /* name of func when called from Python */
   (PyCFunction)graph_knn,             /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_knn_doc},        /* doc string */
  {"graph_eps",           /* name of func when called from Python */
   (PyCFunction)graph_eps,             /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_eps_doc},        /* doc string */
   {"graph_cross_knn",    /* name of func when called from Python */
   (PyCFunction)graph_cross_knn,       /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_cross_knn_doc},  /* doc string */
  {"graph_cross_eps",     /* name of func when called from Python */
   (PyCFunction)graph_cross_eps,       /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_cross_eps_doc},  /* doc string */
  {"graph_cross_eps_robust",     /* name of func when called from Python */
   (PyCFunction)graph_cross_eps_robust,       /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_cross_eps_robust_doc},  /* doc string */
  {"graph_mst",           /* name of func when called from Python */
   (PyCFunction)graph_mst,             /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_mst_doc},        /* doc string */
  {"graph_3d_grid",        /* name of func when called from Python */
   (PyCFunction)graph_3d_grid,          /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_3d_grid_doc},        /* doc string */
   {"graph_normalize",        /* name of func when called from Python */
   (PyCFunction)graph_normalize,          /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_normalize_doc},        /* doc string */
  {"graph_cc",        /* name of func when called from Python */
   (PyCFunction)graph_cc,          /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_cc_doc},        /* doc string */
  {"graph_dijkstra",        /* name of func when called from Python */
   (PyCFunction)graph_dijkstra,          /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_dijkstra_doc},        /* doc string */
  {"graph_dijkstra_multiseed", /* name of func when called from Python */
   (PyCFunction)graph_dijkstra_multiseed, /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_dijkstra_multiseed_doc},        /* doc string */
  {"graph_floyd",        /* name of func when called from Python */
   (PyCFunction)graph_floyd,          /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_floyd_doc},        /* doc string */
   {"graph_voronoi",        /* name of func when called from Python */
   (PyCFunction)graph_voronoi,          /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_voronoi_doc},        /* doc string */
   {"graph_rd",        /* name of func when called from Python */
   (PyCFunction)graph_rd,          /* corresponding C function */
   METH_KEYWORDS,          /* ordinary (not keyword) arguments */
   graph_rd_doc},        /* doc string */
   {"graph_skeleton",        /* name of func when called from Python */
    (PyCFunction)graph_skeleton,          /* corresponding C function */
    METH_KEYWORDS,          /*ordinary (not keyword) arguments */
    graph_skeleton_doc},        /* doc string */
    {NULL, NULL,0,NULL}

};


void init_graph(void)
{
  Py_InitModule3("_graph", module_methods, module_doc);
  fffpy_import_array();
  import_array();   /* required NumPy initialization */
}
