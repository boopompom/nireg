/*!
  \file fff_graphlib.h
  \brief Graph modelling and processing functions.
  \author Bertrand Thirion
  \date 2004-2006

  This library implements different low- and high-level functions for 
  graph processing.

  low-level functions include: graph "constructor" and "destructor",
  labelling of the vertices according to the connected component they
  belong to, extraction of the main cc, computation of the edge
  degrees. Note that the vertices are referred to as [0..V-1], V being the
  number of vertices
  
  Higher level functions include Construction of the k nearest neighbors, 
  epsilon neighbours graph, Minimum Spanning Tree. For these functions, the 
  dataset is given as a matrix, assuming an implicit Euclidian distance.
  
  Last, Dijkstra's and Floyd's algorithm have been implemented.

  An important choice is that the graph is represented by a sparse
  adjacency matrix coding. In the current state of the structure, the
  later is a 3 vectors structure (A,B,D). E being the number of edges, an
  edge i, i<E,  is defined as the directed [A(i) B(i)] segment; D(i) is 
  a value associated with the edge (e.g. a length or a weight).
  
  The coding is appropriate for very large numbers of vertices with
  sparse connections. It is clearly suboptimal for small, dense
  graphs.
  
  2008/04/02: 
  To be implemented : 
  - quick list handling
  - a suboptimal part in MST
  */

#ifndef fff_GRAPHLIB
#define fff_GRAPHLIB

#ifdef __cplusplus
extern "C" {
#endif

#include "fff_array.h"
#include "fff_vector.h"
#include "fff_matrix.h"
#include "fff_base.h"


  typedef struct fff_graph{
    
    long V;                /*!< Number of vertices of the graph */
    long E;                /*!< Number of Edges of the graph */
    long* eA;                 /*!< edge origins (E) */
    long* eB;                 /*!< edge ends (E) */
    double* eD;              /*!< edge weights (E) */
    
  } fff_graph;

 /*!
    \struct fff_graph
    \brief Sparse graph structure
  */

  /*! 
    \brief Constructor for the fff_graph structure 
    \param v : number of vertices
    \param e : number of edges
  */
  extern fff_graph* fff_graph_new( const long v, const long e );
  /*! 
    \brief Destructor for the fff_graph structure 
    \param thisone the fff_graph structure to be deleted
  */
  extern void fff_graph_delete( fff_graph* thisone );
  /*! 
    \brief Reset function (with partial destruction) for 
    the fff_graph structure 
    \param thisone the fff_graph structure to be reset
    \param v the number of edges to be set
    \param e the number of vertices to be set
  */
  extern void fff_graph_reset( fff_graph** thisone, const long v, const long e );
  /*! 
    \brief Other Constructor for the fff_graph structure 
    \param v the number of edges to be set
    \param e the number of vertices to be set
    \param A the origins of edges to be set
    \param B the ends of edges to be set
    \param D the values of edges to be set
  */
  extern fff_graph* fff_graph_build(const long v, const long e, const long *A, const long* B, const double*D );
    /*! 
    \brief Other Constructor for the fff_graph structure 
    \param v the number of edges to be set
    \param e the number of vertices to be set
    \param A the origins of edges to be set
    \param B the ends of edges to be set
    \param D the values of edges to be set
  */
  extern fff_graph* fff_graph_build_safe(const long v, const long e, const fff_array *A, const fff_array* B, const fff_vector *D );
   /*! 
    \brief Fill a graph structures with data
    \param thisone existing graph
    \param A the origins of edges to be set
    \param B the ends of edges to be set
    \param D the values of edges to be set
  */
  extern void fff_graph_set( fff_graph* thisone , const long *A, const long* B, const double*D );
  /*! 
    \brief Fill a graph structures with data
    \param thisone existing graph
    \param A the origins of edges to be set
    \param B the ends of edges to be set
    \param D the values of edges to be set
  */
  extern void fff_graph_set_safe( fff_graph* thisone , const fff_array *A, const fff_array* B, const fff_vector *D );
  /*! 
    \brief edit the structure of a graph
    \param A the origins of edges
    \param B the ends of edges 
    \param D the values of edges 
    \param thisone the edited graph

    Caveat : It is assumed that sufficient memory has been allocated for A,B,D
    (G->E elements)
  */
  extern void fff_graph_edit(long *A, long* B, double*D, const fff_graph* thisone );
   /*! 
    \brief edit the structure of a graph
    \param A the origins of edges
    \param B the ends of edges 
    \param D the values of edges 
    \param thisone the edited graph
  */
  extern void fff_graph_edit_safe(fff_array *A, fff_array* B, fff_vector *D, const fff_graph* thisone );
 
  
  /*! \brief Normalization of the values of a graph per rows
    \param G graph that is being normalized
	\param SR the row-wise sums
	
    the weights G->eD are normalized so that
    sum {G->eA[j]==i} G->eD[j] = 1 
	SR should be allocated G->V elements
  */

  extern void fff_graph_normalize_rows(fff_graph* G, fff_vector*SR);
  
  /*! \brief Normalization of the values of a graph per columns
    \param G graph that is being normalized
	\param SC the column-wise sums

    the weights G->eD are normalized so that
    sum {G->eB[j]==i} G->eD[j] = 1 
	SC should be allocated G->V elements
  */
  extern void fff_graph_normalize_columns(fff_graph* G, fff_vector* SC);

 /*! \brief Normalization of the values of a graph per columns
    \param G graph that is being normalized
	\param SR the row-wise sums
	\param SC the column-wise sums

    the weights G->eD are normalized symmetrically
	SR and SC should be allocated G->V elements
 */
  extern void fff_graph_normalize_symmetric(fff_graph* G, fff_vector* SR, fff_vector *SC);

  /*! \brief Basic sparse-graph Copy function
       \param G2 copy graph  
       \param G1 original graph

       G2 = G1;
       Note that G2 must have the same number of edges as G1, and
       must be allocated the correct memory size.
  */
  extern void fff_graph_copy(fff_graph* G2,const fff_graph* G1);
  
  /*! \brief Extraction of a subgraph from a given graph
       \param K output graph
       \param G input graph
       \param v list of vertices to be retained in the graph
       
       All the egdges adjacent to a vertex not in v are removed 
       It should be notices that the vertices are relabeled to [0..n-1]
       where n = size(v) 
  */
  extern void fff_get_subgraph(fff_graph **K, const fff_graph *G, const fff_array* v);
 
  /*! \brief Extraction of a subgraph from a given graph
       \param K output graph
       \param G input graph
       \param b is nonzero for each edge to be retained.
       
       b is assumed to have G->V elements.
       All the egdges adjacent to a vertex i such that b[i]=0 are removed 
       It should be notices that the vertices are relabeled to [0..n-1]
       where n = size(v) 
  */
  extern void fff_extract_subgraph(fff_graph **K, const fff_graph *G, long* b);
  
  /*
    \brief Conversion of a graph into a neighboring system
    \param cindices indexes of the neighbors of each vertex
    \param neighb neigbor list
    \param weight weight list
    \param G input graph

    this returns another sparse coding of the graph structure, in which each edge (eA[i],eB[i],eD[i])
    is coded as:
    for j in [cindices[a] cindices[a+1][, (a,eB[j],eD[j]) is an edge of G
    The advantage  is that the coding is sparser, and that the "neighbours of a" are directly given
    by the definition.
    cindices must be allocated G->V+1 elements
    neigh and weight must be allocated G->E elements
  */
  extern long fff_graph_to_neighb(fff_array *cindices, fff_array * neighb, fff_vector* weight, const fff_graph* G);

/*!
    \brief k-nearest neighbours sparse graph construction
    \param G resulting sparse graph
    \param X data matrix. 
    \param k number of nearest neighbours considered.

    This algorithm builds a graph whose vertices are the list of items
    and whose edges  are the symmeterised knn's.
    The data matrix should be dimensioned as (nb items * feature dimension)
    The number of edges is computed by the algorithm,    
    and the structure is built during the function call.
    G has the correct set of edges
    Note that the corresponding adjacency matrix is symmetric ; 
    for each vertex v, the edge
    (v,v,0) is NOT included in he E matrix
    The metric used in the algo is Euclidian.

    The number of edges is returned.
    For the sake of speed, it is advisable to set X
    so that the first coordinate contains more information than 
    the second one, etc. This is simply achieved by SVD.
  */
  extern long fff_graph_knn(fff_graph** G, const fff_matrix* X, const long k); 
  
  /*!
    \brief k-nearest neighbours sparse graph construction
    \param G resulting sparse graph
    \param X source data matrix 
    \param Y target data matrix 
    \param k number of nearest neighbours considered.
    
    This algorithm builds a graph whose vertices are the list of rows of X 
    and whose edges  are the knn of these in the rows of Y 
    The data matrices should be dimensioned as (nb items * feature dimension)
    and size(Y,2) = size(X,2)
    The number of edges is k for each vertex 
    For this reason, G is assumed to have been previously allocated.

    The metric used in the algo is Euclidian.

    The number of edges is returned.
    For the sake of speed, it is advisable to set X and  Y 
    so that the first coordinate contains more information than 
    the second one, etc. This is simply achieved by SVD.
  */
  extern long  fff_graph_cross_knn( fff_graph* G, const fff_matrix* X, const fff_matrix *Y, const long k);
 
 /*!
    \brief eps-neighbours graph construction
    \param G resulting graph
    \param X data matrix. 
    \param eps neighborhood radius  

    This algorithm builds a graph whose vertices are the list of items
    and whose edges  are the points that lie closer than eps.
    The data matrix should be dimensioned as (nb items * feature dimension)
    The number of edges is computed by the algorithm,    
    the graph structure is built during the function call.
    The final structure G has the correct set of edges
    Note that each edge is given twice ; for each vertex v, the edge
    (v,v,0) is NOT included in he E matrix
    The metric used in the algo is Euclidian.

    the number of edges is returned
  */
  extern long fff_graph_eps( fff_graph** G, const fff_matrix* X, const double eps); 

/*!
    \brief eps-neighbours graph construction
    \param G resulting graph
    \param X data matrix. 
    \param Y data matrix. 
    \param eps neighborhood radius
    

    This algorithm builds a graph 
    whose adjacency matrix is the epsilon thresholded distance matrix
    between the rows of X and Y.
    The data matrix should be dimensioned as (nb items * feature dimension)
    The number of edges is computed by the algorithm,    
    and the graph structure is reset during the function call.
    The final structure G has the correct set of edges
  
    The metric used in the algo is Euclidian.

    the number of edges is returned
  */
  extern long fff_graph_cross_eps( fff_graph** G, const fff_matrix* X, const fff_matrix* Y, const double eps);   
 
/*!
    \brief eps-neighbours robust graph construction
    \param G resulting graph
    \param X data matrix. 
    \param Y data matrix. 
    \param eps neighborhood radius
    
   idem fff_graph_cross_eps, but the nearest neighbor is always included
   for each row of X
  */
  extern long fff_graph_cross_eps_robust( fff_graph** G, const fff_matrix* X, const fff_matrix* Y, const double eps);   
 
  /*!
    \brief k-Cartesian-neighbours graph construction
    \param G resulting graph
    \param xyz input coordinates in 3D
    \param k number of neighbors (6,18 or 26)

    This algorithm builds a graph whose adjacency matrix 
    represents the k-neighoring relationship, of the input data xyz.
    xyz is  written as an N*3 matrix 
    G is built inside of the function with N vertices and E edges 
    (E<= (k+1)*N here) during the algo.
    the number of edges is returned
  */
  extern long fff_graph_grid(fff_graph** G, const fff_array* xyz, const long k);
  /*!
    \brief Six-Cartesian-neighbours graph construction
    \param G resulting graph
    \param xyz input coordinates in 3D
    \param N number of points considered

    This algorithm builds a graph whose adjacency matrix 
    represents the 6-neighoring relationship, of the input data xyz.
    xyz is assumed to be written ss x[0],..,x[N-1],y[0],..,y[N-1],
    z[0],..,z[N-1].
    G is built inside of the function with N vertices and E edges 
    (E<= 7*N here) during the algo.
    the number of edges is returned
  */
  extern long fff_graph_grid_six(fff_graph** G,const long* xyz, const long N);
/*!
    \brief eighteen-Cartesian-neighbours graph construction
    \param G resulting graph
    \param xyz input coordinates
    \param N number of points

    This algorithm builds a graph whose adjacency matrix 
    represents the 18-neighoring relationship, of the input data xyz.
    xyz is assumed to be written ss x[0],..,x[N-1],y[0],..,y[N-1],
    z[0],..,z[N-1].
    G is built inside of the function with N vertices and E edges 
    (E<= 19*N here) during the algo.
    the number of edges is returned
  */
  extern long fff_graph_grid_eighteen(fff_graph** G,const long* xyz, const long N);
    /*!
    \brief twenty-six-Cartesian-neighbours graph construction
    \param G resulting graph
    \param xyz input coordinates 
    \param N number of points

    This algorithm builds a graph whose adjacency matrix 
    represents the neighoring relationship, of the input data xyz.
    xyz is assumed to be written ss x[0],..,x[N-1],y[0],..,y[N-1],
    z[0],..,z[N-1].
    G is built inside of the function with N vertices and E edges 
    (E<= 27*N here) during the algo.
    the number of edges is returned
  */
  extern long fff_graph_grid_twenty_six(fff_graph** G,const long* xyz, const long N);
   
 
   /*!
    \brief Minimum Spanning Tree construction
    \param X data matrix. 
    \param G resulting sparse graph

    This algorithm builds a graph whose vertices are the list of items
    and whose edges for the MST of X.
    The data matrix should be dimensioned as (nb items * feature dimension)
    The number of edges is 2*nb vertices-2, due to the symmetry.
    The metric used in the algo is Euclidian.
    The algo used is Boruvska's algorithm. It is not fully optimized yet.

    The length of the MST is returned
  */
  double fff_graph_MST(fff_graph* G,const fff_matrix* X);

/*!
    \brief Minimum Spanning Tree construction from an existing graph
    \param G input graph
    \param K resulting sparse graph

    This algorithm builds a graph whose vertices are the list of items
    The number of edges is 2*nb vertices-2, due to the symmetry.
    The metric used in the algo is Euclidian.
    The algo used is Boruvska's algorithm. It is not fully optimized yet.

    The length of the MST or "skeleton" is returned
  */
  double fff_graph_skeleton(fff_graph* K,const fff_graph* G);

  /*!
    \brief graph labelling by connected components
    \param label resulting labels     
    \param G  sparse graph
 
    Given a graphG (V vertices),
    this algorithm builds a set of labels of size V, where each vertex 
    of one connected component of the graph has a given label
    It is assumed that label has been allocated enough size (G->V sizeof(double))
    It is assumed that the graph is undirected 
    (i.e. connectivity is assessed in the non-directed sense)

    the number of cc's is returned
  */
  extern long fff_graph_cc_label(long* label, const fff_graph* G);
  
  /*!
    \brief Dijkstra's algorithm
    \param dist the computed distance vector
    \param G  graph
    \param seed Dijkstra's algo seed point
    \param infdist infinite distance 
    
    Given a graph G, this algorithm compute Dijkstra's algo on 
    the weights of the graph.
    note that all the edge weights should be positive ! (distance graph)
    seed should be given in the interval 0,..,V-1
    infdist can be chosen typically as the sum of the edge weights
    of the graph.
  */
  extern long fff_graph_Dijkstra(double *dist, const fff_graph* G,const long seed, const double infdist );

/*!
    \brief Dijkstra's algorithm
    \param dist the computed distance vector
    \param G  graph
    \param seed Dijkstra's algo seed point
    
    Given a graph G, this algorithm compute Dijkstra's algo on 
    the weights of the graph. teh positivity of G->eD is checked.
    seed should be given in the interval 0,..,V-1
  */
  extern long fff_graph_dijkstra(double *dist, const fff_graph* G,const long seed);


  /*!
    \brief Dijkstra's algorithm
    \param dist the computed distance vector
    \param G  graph
    \param seeds Dijkstra's algo seed points
    
    Given a graph G, this algorithm compute Dijkstra's algo on 
    the weights of the graph. teh positivity of G->eD is checked.
    seeds should be given in the interval 0,..,V-1
	the null set is now an extended region
  */
  extern int fff_graph_Dijkstra_multiseed( fff_vector* dist, const fff_graph* G, const fff_array* seeds);

  /*!
    \brief Partial Floyd's algorithm
    \param dist the computed distance matrix (seeds*vertices)
    \param G  graph
    \param seeds the set of seed points from which geodesics are computed

    Given a graph G, this algorithm perform's a pseudo Floyd's algo on 
    the weights of the graph, by repetition of Dijkstra's algo
    from the seeds
    seeds should be given in the interval 0,..,V-1
    dist should be of size(nb(seeds),G->V)
  */
  extern long fff_graph_partial_Floyd(fff_matrix *dist, const fff_graph* G,const  long *seeds);

  /*!
    \brief Pseudo Floyd's algorithm
    \param dist the computed distance matrix (vertices*vertices)
    \param G  graph
    
    Given a graph G, this algorithm perform's a pseudo Floyd's algo on 
    the weights of the graph, by repetition of Dijkstra's algo
    from the vertices
    Note that all the edge weights should be positive ! (distance graph)
  */
  extern long fff_graph_Floyd( fff_matrix *dist, const fff_graph* G);

/*!
    \brief geodesic Voronoi algorithm
    \param label is the Voronoi vertices labelling 
    \param G  graph
    \param seeds the set of seed points of geodesic cells

    Given a graph G and seed points , this algorithm perform's a Voronoi
    labelling of the graph vertices, using the graph distance.
    Note that all the edge weights should be positive ! (distance graph)
  */
  extern long fff_graph_voronoi(fff_array *label, const fff_graph* G,const  fff_array *seeds);

  
/*!
    \brief Cliques extraction algorithm based on replicator dynamics
    \param cliques a labelling of the vertices according to the clique they belong to
    \param G  graph
    
    Given a graph with positive weights, this algo recursively 
    searches for the largest clique using RD framework.
    Note that due to stochastic initialization, the results may 
    vary from time to time
  */
  extern long fff_graph_cliques(fff_array *cliques, const fff_graph* G);


#ifdef __cplusplus
}
#endif

#endif
