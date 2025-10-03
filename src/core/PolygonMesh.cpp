//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-05 23:13:04 taubin>
//------------------------------------------------------------------------
//
// PolygonMesh.cpp
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//     * Neither the name of the Brown University nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GABRIEL
// TAUBIN BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.

#include <iostream>
#include "PolygonMesh.hpp"
#include "Partition.hpp"
#include "Faces.hpp"

// TODO Mon Mar 6 2023
// - merge your code from Assignment 2

PolygonMesh::PolygonMesh(const int nVertices, const vector<int>& coordIndex):
  HalfEdges(nVertices,coordIndex),
  _nPartsVertex(),
  _isBoundaryVertex()
{
  int nV = getNumberOfVertices();
  int nE = getNumberOfEdges(); // Edges method
  // int nF = getNumberOfFaces();
  int nC = getNumberOfCorners();

  // 1) classify the vertices as boundary or internal
  int iV;
  for(iV=0;iV<nV;iV++)
    _isBoundaryVertex.push_back(false);

  // - for edge boundary iE label its two end vertices as boundary
  for(int iE=0; iE<nE; iE++){
    int nIncidentFaces = getNumberOfEdgeHalfEdges(iE);
    if(nIncidentFaces == 1){      //La arista es borde sii tiene una sola cara incidente (i.e., un solo half-edge incidente)
      int V0 = getVertex0(iE);
      int V1 = getVertex1(iE);
      _isBoundaryVertex[V0] = true;
      _isBoundaryVertex[V1] = true;
    }
  }
  
  // 2) create a partition of the corners in the stack
  Partition partition(nC);
  // 3) for each regular edge
  //    - get the two half edges incident to the edge
  //    - join the two pairs of corresponding corners accross the edge
  //    - you need to take into account the relative orientation of
  //      the two incident half edges

  for(int iE=0; iE<nE; iE++){
    int nIncidentFaces = getNumberOfEdgeHalfEdges(iE);
    if(nIncidentFaces == 2){      //La arista es regular sii tiene dos caras incidentes
      int C0 = getEdgeHalfEdge(iE, 0);
      int C1 = getEdgeHalfEdge(iE, 1);
      if(coordIndex[C0] == coordIndex[getNext(C1)]){
        // Las esquinas incidentes a iE, C0 y C1, estan consistentemente orientadas
        partition.join(C0, getNext(C1));
        partition.join(C1, getNext(C0));
      } else {
        //Las esquinas incidentes a iE, c0 y c1, no están consistentemente orientadas
        partition.join(C0, C1);
        partition.join(getNext(C0), getNext(C1));
      }

    }
  }

  // consistently oriented
  /* \                  / */
  /*  \ iC01 <-- iC00  /  */
  /*   X ---- iE ---- X   */
  /*  / iC10 --> iC11  \  */
  /* /                  \ */

  // oposite orientation
  /* \                  / */
  /*  \ iC01 --> iC00  /  */
  /*   X ---- iE ---- X   */
  /*  / iC10 --> iC11  \  */
  /* /                  \ */

  // a decision has to be made about inconsistently oriented faces
  // incident to the same edge, as well as how to deal with singular
  // edges; for the moment let's assume that the mesh does not have
  // singular edges, and that pairs of corners corresponding to the
  // same vertex across inconsistently oriented faces will be joined

  // note that the partition will end up with the corner separators as
  // singletons, but it doesn't matter for the last step, and
  // the partition will be deleteted upon return
  
  // 4) count number of parts per vertex
  //    - initialize _nPartsVertex array to 0's
  //    - for each corner iC which is a representative of its subset, 
  //    - get the corresponding vertex index iV and increment _nPartsVertex[iV]
  //    - note that all the corners in each subset share a common
  //      vertex index, but multiple subsets may correspond to the
  //      same vertex index, indicating that the vertex is singular
  for(int i=0; i<nV; i++){
    _nPartsVertex.push_back(0);
  }
  for(int iC=0; iC<nC; iC++){
    if(partition.find(iC)==iC && coordIndex[iC]>=0){
      iV = coordIndex[iC];
      _nPartsVertex[iV]++;
    }
  }
}


int PolygonMesh::getNumberOfFaces() const {
  int nF = 0;
  for(int i=0; i<getNumberOfCorners(); i++){
    if(_coordIndex[i]<0) nF++;
  }
  return nF;
}

int PolygonMesh::getNumberOfEdgeFaces(const int iE) const {
  return getNumberOfEdgeHalfEdges(iE);
}

int PolygonMesh::getEdgeFace(const int iE, const int j) const {
  int iC = getEdgeHalfEdge(iE, j);
  return getFace(iC);               //Si los parámetros son inválidos, getEdgeHalfEdge retorna -1 y getFace(-1) retorna -1
}

bool PolygonMesh::isEdgeFace(const int iE, const int iF) const {
  bool result = false;
  int nHE = getNumberOfEdgeHalfEdges(iE);
  for(int i=0; i<nHE; i++){
    int iC = getEdgeHalfEdge(iE, i);
    if(getFace(iC)==iF){
      result = true;
      break;
    }
  }
  return result;
}

// classification of vertices

bool PolygonMesh::isBoundaryVertex(const int iV) const {
  int nV = getNumberOfVertices();
  return (0<=iV && iV<nV)?_isBoundaryVertex[iV]:false;
}

bool PolygonMesh::isSingularVertex(const int iV) const {
  int nV = getNumberOfVertices();
  return (0<=iV && iV<nV && _nPartsVertex.size()>0 && _nPartsVertex[iV]>1);
}

bool PolygonMesh::isInternalVertex(const int iV) const{
  int nV = getNumberOfVertices();
  return (0<=iV && iV<nV)?(!isBoundaryVertex(iV)):false;
}

// properties of the whole mesh

bool PolygonMesh::isRegular() const {
  bool result = true;
  int nE = getNumberOfEdges();
  int nV = getNumberOfVertices();
  for(int iE=0; iE<nE; iE++){
    if(isSingularEdge(iE)){
      result = false;
      break;
    }
  }
  if(result){
    for(int iV=0; iV<nV; iV++){
      if(isSingularVertex(iV)){
        result = false;
        break;
      }
    }
  }
  return result;
}

bool PolygonMesh::hasBoundary() const {
  bool result = false;
  int nE = getNumberOfEdges();
  for(int iE=0; iE<nE; iE++){
    if(isBoundaryEdge(iE)){
      result = true;
      break;
    }
  }
  return result;
}

//////////////////////////////////////////////////////////////////////
// CONNECTED COMPONENTS

// connected components of the primal graph
// - returns number of connected components nCC
// - fills the faceLabel array with connected component number iCC
// - for each face; 0<=iCC<nCC
int PolygonMesh::computeConnectedComponentsPrimal(vector<int>& faceLabel) const {
  int nCCprimal = 0;
  faceLabel.clear();

  // HINTS :
  //
  // - use the edges of the primal graph to compute a partition of the
  //   vertices
  //
  // - nCC is equal to the final number of parts in the partition
  //
  // - since all the vertices of each face must belong to the the same
  //   part, there is no ambiguity in assigning connected component
  //   numbers to the faces
  //
  // - to assign component numbers to the parts in the range 0<=iCC<nCC 
  //   you can use an array vToCC of size nV initialized to -1's
  //
  // - first, assign component numbers to the vertices which are
  //   representative of the parts
  // - second, assign component numbers to all the other vertices
  // - finally, fill the faceLabel array using the component number
  //   associated with the first vertex of each face

  int nV = getNumberOfVertices();
  int nE = getNumberOfEdges();
  int nF = getNumberOfFaces();
  Partition vertexPartition(nV);

  for(int iE=0; iE<nE; iE++){
    int V0 = getVertex0(iE);
    int V1 = getVertex1(iE);
    vertexPartition.join(V0, V1);
  }

  nCCprimal = vertexPartition.getNumberOfParts();

  vector<int> vToCC(nV, -1);
  int componentNumber = 0;
  for (int iV=0; iV<nV; iV++) {
    if (vertexPartition.find(iV) == iV) {
      //iV es el vértice representativo de su partición
      vToCC[iV] = componentNumber;
      componentNumber++;
    }
  }
  for (int iV=0; iV<nV; iV++) {
    int representative = vertexPartition.find(iV);
    if (representative != iV) {
      //iV no es el índice representativo de su partición
      vToCC[iV] = vToCC[representative];
    }
  }

  Faces faces(nV, _coordIndex);
  for (int iF=0; iF<nF; iF++) {
    int vertex = faces.getFaceVertex(iF, 0); //Obtengo el primer vértice de la cara
    faceLabel.push_back(vToCC[vertex]);
  }
  
  return nCCprimal;
}

// connected components of the dual graph
// - returns number of connected components nCC
// - fills the faceLabel array with connected component number iCC
// - for each face; 0<=iCC<nCC
int PolygonMesh::computeConnectedComponentsDual(vector<int>& faceLabel) const {
  int nCCdual = 0;
  faceLabel.clear();

  int iF,iFt,iFR,iC0,iC1,iC,iCt,iP;

  // HINTS :
  //
  // - use the edges of the dual graph to compute a partition of the
  //   vertices
  //
  // - you can generate the dual graph explicitly, or just traverse
  //   the half edges looking for regular edges
  //
  // - nCC is equal to the final number of parts in the partition
  //
  // - component number assignment is similar to the primal case, but
  //   easier here, since there is no need to transfer from vertices
  //   to faces

  int nC = getNumberOfCorners();
  int nF = getNumberOfFaces();
  Partition facePartition(nF);
  for(iF=iC0=iC1=0;iC1<nC;iC1++) {
    if(_coordIndex[iC1]>=0) continue;
    for(iC=iC0; iC<iC1; iC++){
      int vSrc = getSrc(iC);
      int vDst = getDst(iC);
      int iE = getEdge(min(vSrc, vDst), max(vSrc, vDst));
      bool regularEdge = isRegularEdge(iE);
      if (regularEdge) {
        iCt = getTwin(iC);
        iFt = getFace(iCt);
        facePartition.join(iF, iFt);
      }
    }
    iC0 = iC1+1;
    iF++;
  }

  nCCdual = facePartition.getNumberOfParts();

  vector<int> faceToCC(nF, -1);
  int componentNumber = 0;
  for (int iF=0; iF<nF; iF++) {
    if (facePartition.find(iF) == iF) {
      //iF es la cara representativa de su partición
      faceToCC[iF] = componentNumber;
      componentNumber++;
    }
  }
  for (int iF=0; iF<nF; iF++) {
    int representative = facePartition.find(iF);
    if (representative != iF) {
      //iF no es la cara representativa de su partición
      faceToCC[iF] = faceToCC[representative];
    }
  }

  for (int iF=0; iF<nF; iF++) {
    faceLabel.push_back(faceToCC[iF]);
  }
  
  return nCCdual;
}

// ORIENTATION
  
// determines if the mesh is oriented
// - a mesh is oriented if all regular edges are consistently oriented
// - by definition, a mesh with singular edges is not oriented
// - note that isolated singular vertices play no role in this
//   definition (since cuting through them does not affect
//   orientation)
bool PolygonMesh::isOriented() const {
  if(hasSingularEdges()) return false;

  // HINTS :
  //
  // - traverse the list of half edges, and check whether or not the
  //   regualr edges are consistently oriented
  // - as soon as you find one edge which is not consistently oriented
  //   you can reurn false
  // - if no inconsistently oriented edge is found, return true
  bool oriented = true;
  int nC = getNumberOfCorners();
  for (int iC=0; iC<nC; iC++) {
    if (_coordIndex[iC]==-1) continue;
    if (!HalfEdges::isOriented(iC)) {
      oriented = false;
      break;
    }
  }
  return oriented;
}

// determines if the mesh is orientable
// - a mesh is orientable if a choice of orientation can be made for
//   each face so that, after the face orientation changes are made,
//   the resulting mesh is oriented
// - by definition, a mesh with singular edges is not orientable
// - note that isolated singular vertices play no role in this
//   definition (since cuting through them does not affect
//   orientation)
bool PolygonMesh::isOrientable() const {
  if(hasSingularEdges()) return false;
  if(isOriented()) return true;
  // here the mesh is not oriented but only has regular and boundary edges

  // you need to implement a mesh traversal algorithm using these variables

  int nC = getNumberOfCorners();
  int nF = getNumberOfFaces();
  vector<bool> face_was_visited(nF,false);
  vector<bool> invert_face(nF,false);
  vector<int> face_root;
  vector<int> corner_stack;

  // pseudo-code
  //
  // while (not all the faces have been visited) {
  //  - select a root face iF_root
  //  - set the face as visited, and preserve its original orientation  
  //    push all the half edge indices of the face onto the corner_stack
  //  while ( the corner_stack is not empty) {
  //   - pop a half edge index iC until you find one which has a twin
  //     iCt contained in a face iFt; let iF be the face containing iC
  //   - determine whether or not iFt should be inverted if the edge
  //     is crossed to make the two half edges consistently oriented
  //   if (the face iFt had been visited before) {
  //     if ( the computed expected orientation is different from the
  //          one assigned on the first visit) {
  //       - we have found a loop in the dual graph which cannot be
  //         oriented; as a result, the mesh is not oriented
  //       - return false;
  //      }
  //    } else if (the face iFt had not been visited before) {
  //      - save the computed expected orientation in the invert_face array
  //      - push all the remaining half edges of the face iFt onto the corner_stack
  //    }
  //   - this is the end of the connected component in the dual graph
  //     containing iF_root
  //   - if not all the faces have been visited, we can repeat until
  //     all the connected components are accounted for }
  //  }

  int nV = getNumberOfVertices();
  Faces faces(nV, _coordIndex);

  int iF_root = 0;

  while (iF_root < nF) {
    face_was_visited[iF_root] = true;

    //Agrego todas las esquinas de la cara al stack
    int iC0 = faces.getFaceFirstCorner(iF_root);
    corner_stack.push_back(iC0);
    int iCNext = getNext(iC0);
    while (iCNext != iC0) {
      corner_stack.push_back(iCNext);
      iCNext = getNext(iCNext);
    }

    while (!corner_stack.empty()) {
      //Saco la siguiente esquina del stack y me fijo si tiene una esquina twin válida (es decir, si es un half-edge incidente a una arista regular)
      int iC = corner_stack[corner_stack.size()-1];
      corner_stack.pop_back();
      int iCt = getTwin(iC);
      if (iCt >= 0) {
        int iF = getFace(iC);
        int iFt = getFace(iCt);

        bool consistentlyOriented = (getSrc(iC) == getDst(iCt));

        //Como iF es la cara de la esquina obtenida del stack, sabemos que ya la visitamos y por lo tanto ya decidimos si se tiene que invertir o no
        //Si la cara iF se tiene que invertir, entonces la cara iFt se tiene que invertir solo si actualmente estan consistentemente orientadas
        //Si la cara iF no se tiene que invertir, entonces la cara iFt se tiene que invertir solo si no están consistentemente orientadas
        bool shouldInvert;
        if (invert_face[iF]) {
          shouldInvert = consistentlyOriented;
        } else {
          shouldInvert = !consistentlyOriented;
        }

        if (face_was_visited[iFt]) {
          if (shouldInvert != invert_face[iFt]) return false;
        } else {
          face_was_visited[iFt] = true;

          invert_face[iFt] = shouldInvert;

          corner_stack.push_back(iCt);
          iCNext = getNext(iCt);
          while (iCNext != iCt) {
            corner_stack.push_back(iCNext);
            iCNext = getNext(iCNext);
          }
        }
      }
    }

    //Busco un nuevo valor iF_root de una cara que no se haya visitado. Si todas las caras se visitaron, iF_root queda con valor nF y se rompe el ciclo
    for (iF_root=0; iF_root<nF; iF_root++) {
      if (face_was_visited[iF_root] == false) break;
    }
  }

  //Si llego a este punto, ya verifiqué que la malla es orientable
  return true;
}

// orient
// - implementation requires a dual graph traversal algorithm
// - the number of connected components nCC of the dual graph are
//   determined as a by product
// - if multiple orientations are posible choose one
// - fills the ccIndex array, of size equal to the number of faces
//   nF, with the connected component number iCC assigned to each face
// - the values stored in the ccIndex shold be in the range 0<=iCC<nCC
// - fills the output invert_face, with values required to produce
//   an oriented mesh
// - the size of output invert_face array should equal to the number
//   of faces
// - returns the number of connected components nCC if successful,
//   and 0 if the mesh is not orientable
// - if not successful, the output arrays should be empty as well
int PolygonMesh::orient(vector<int>& ccIndex, vector<bool>& invert_face) {
  int nCC = 0;
  ccIndex.clear();
  invert_face.clear();
  if(hasSingularEdges()) return 0;
  // if(isOriented()) return true;
  // mesh is not oriented but only has regular and boundary edges
  int nC = getNumberOfCorners();
  int nF = getNumberOfFaces();
  vector<bool> face_was_visited(nF,false);
  vector<int>& face_root = ccIndex;
  vector<int> corner_stack;

  // HINTS :
  //
  // - a minor variation of isOrientable()
  // - note that you cannot return right away if isOriented()==true
  //   since we need to partition the faces into connected components,
  //   and fill the ccIndex and invert_face array

  //inicializo ccIndex e invert_face para que tengan tamaño nF
  for (int iF=0; iF<nF; iF++) {
    ccIndex.push_back(-1);
    invert_face.push_back(false);
  }

  int nV = getNumberOfVertices();
  Faces faces(nV, _coordIndex);

  int iF_root = 0;

  while (iF_root < nF) {
    //En cada iteración del while exploro las caras de una nueva componente conexa
    nCC++;
    face_was_visited[iF_root] = true;
    //El ccIndex de cada cara visitada en el while va a ser igual a la cantidad de componentes conexas descubiertas en el momento menos uno
    ccIndex[iF_root] = nCC-1;


    //Agrego todas las esquinas de la cara al stack
    int iC0 = faces.getFaceFirstCorner(iF_root);
    corner_stack.push_back(iC0);
    int iCNext = getNext(iC0);
    while (iCNext != iC0) {
      corner_stack.push_back(iCNext);
      iCNext = getNext(iCNext);
    }

    while (!corner_stack.empty()) {
      //Saco la siguiente esquina del stack y me fijo si tiene una esquina twin válida (es decir, si es un half-edge incidente a una arista regular)
      int iC = corner_stack[corner_stack.size()-1];
      corner_stack.pop_back();
      int iCt = getTwin(iC);
      if (iCt >= 0) {
        int iF = getFace(iC);
        int iFt = getFace(iCt);

        bool consistentlyOriented = (getSrc(iC) == getDst(iCt));

        //Como iF es la cara de la esquina obtenida del stack, sabemos que ya la visitamos y por lo tanto ya decidimos si se tiene que invertir o no
        //Si la cara iF se tiene que invertir, entonces la cara iFt se tiene que invertir solo si actualmente estan consistentemente orientadas
        //Si la cara iF no se tiene que invertir, entonces la cara iFt se tiene que invertir solo si no están consistentemente orientadas
        bool shouldInvert;
        if (invert_face[iF]) {
          shouldInvert = consistentlyOriented;
        } else {
          shouldInvert = !consistentlyOriented;
        }

        if (face_was_visited[iFt]) {
          if (shouldInvert != invert_face[iFt]) {
            //Si encuentro que la malla no es orientable, vacío los vectores del output y devuelvo 0
            ccIndex.clear();
            invert_face.clear();
            return 0;
          }
        } else {
          face_was_visited[iFt] = true;

          //Cuando visito una cara, reflejo en ccIndex que forma parte de la componente conexa que se está explorando actualmente
          ccIndex[iFt] = nCC-1;

          invert_face[iFt] = shouldInvert;

          corner_stack.push_back(iCt);
          iCNext = getNext(iCt);
          while (iCNext != iCt) {
            corner_stack.push_back(iCNext);
            iCNext = getNext(iCNext);
          }
        }
      }
    }

    //Busco un nuevo valor iF_root de una cara que no se haya visitado. Si todas las caras se visitaron, iF_root queda con valor nF y se rompe el ciclo
    for (iF_root=0; iF_root<nF; iF_root++) {
      if (face_was_visited[iF_root] == false) break;
    }
  }

  return nCC;
}

//////////////////////////////////////////////////////////////////////
// MANIFOLD

// determine how many isolated vertices the mesh has
int PolygonMesh::numberOfIsolatedVertices() {
  int nV_isolated = 0;

  // HINTS :
  //
  // - isolated vertices are those not contained in the coordIndex array
  // - it is sufficient to count how many faces are incident to each vertex

  vector<int> isolatedVertices = {};
  getIsolatedVertices(isolatedVertices);
  nV_isolated = isolatedVertices.size();

  return nV_isolated;
}

// get array of isolated vertex indices
void PolygonMesh::getIsolatedVertices(vector<int>& isolated_vertex) {
  isolated_vertex.clear();

  // HINTS :
  //
  // - same as the previous method, but returning the indices of the
  //   isolated vertices in an array

  int nV = getNumberOfVertices();
  int nE = getNumberOfEdges();
  vector<bool> isIsolated(nV, true);

  for (int iE=0; iE<nE; iE++) {
    int V0 = getVertex0(iE);
    int V1 = getVertex1(iE);
    isIsolated[V0] = false;
    isIsolated[V1] = false;
  }

  for (int iV=0; iV<nV; iV++) {
    if (isIsolated[iV]) isolated_vertex.push_back(iV);
  }


}

// remove isolated vertices
// - the new number of vertices nVout should be <= that the original
//   number of vertices nV
// - fills the coordMap array, of size nVout, with an input vertex
//   input in the range 0<=iV<nV for each output vertex index
//   0<=iVout<nVout
// - fills the output coordIndexOut array with vertex indices in the
//   output range 0<=iVout<nVout
// - the output coordIndexOut should be of the same size as the
//   input coordIndex array
// - returns true if one or more isolated vertices have been removed,
//   and false if no isolated vertices have been found
// - if no isolated vertices are found, the output arrays should be
//   empty as well
bool PolygonMesh::removeIsolatedVertices
(vector<int>& coordMap, vector<int>& coordIndexOut) {
  coordMap.clear();
  coordIndexOut.clear();

  // HINTS :
  //
  // - determine which vertices are not isolated, and fill the coordMap
  //   array with them
  // - build another array of size nV, initialized to -1's
  //   where you store the location of each non-isolated vertex in the
  //   coordMap array
  // - use this array to fill the coordIndexOut array from coordIndex

  vector<int> isolatedVertices = {};
  getIsolatedVertices(isolatedVertices);
  if (isolatedVertices.empty()) return false;

  int nV = getNumberOfVertices();
  vector<int> newVertex(nV, 0);

  for (int iV : isolatedVertices) {
    newVertex[iV] = -1;
  }

  int vertexCounter = 0;
  for (int iV = 0; iV<nV; iV++) {
    if (newVertex[iV]==-1) continue;
    newVertex[iV] = vertexCounter;
    coordMap.push_back(iV);
    vertexCounter++;
  }

  int nC = getNumberOfCorners();
  for (int iC=0; iC<nC; iC++) {
    if (_coordIndex[iC]==-1) {
      coordIndexOut.push_back(-1);
    } else {
      int iV = _coordIndex[iC];
      int newiV = newVertex[iV];
      coordIndexOut.push_back(newiV);
    }
  }

  return true;
}

// cut through singular vertices
// - should only cut through singular vertices which belong to
//   different connected components of the dual graph
// - it should also remove isolated vertices
// - singular edges should not be modified
// - it should work on non-orientable meshes of any kind
// - the new number of vertices nVout should be => that the original
//   number of vertices nV
// - if nVout==nV, should return empty vIndexMap and coordIndexOut
// - otherwise
// - fills the vIndexMap array, of size nVout, with an input vertex
//   input in the range 0<=iV<nV for each output vertex index
//   0<=iVout<nVout
// - fills the output coordIndexOut array with vertex
//   indices in the output range 0<=iVout<nVout
// - the output coordIndexOut should be of the same size as the
//   input coordIndex array
void PolygonMesh::cutThroughSingularVertices
(vector<int>& vIndexMap, vector<int>& coordIndexOut) {
  vIndexMap.clear();
  coordIndexOut.clear();

  // HINTS :
  //
  // - construct a partition of the corners by joining pairs of
  //   matching corners across regular and singular edges
  //
  // - this can be done remembering the first half edge associated to
  //   each edge in an array; no join operations are performed on the
  //   first half edge visit to the edge
  //
  // - when a subsequent half edge is associated with an edge which
  //   has a valid first half edge corner assigned, the new half edge
  //   and the first half edge of the edge result into two join operations
  //
  // - since the partition still contains the face separators as
  //   singletons, the number of output vertices is equal to the
  //   number of parts minus the number of faces
  //
  // - if the number of output vertices is equal to the number of
  //   input vertices, there is no more work to do
  //
  // - initialize the output coordIndex array with nC -1's
  //
  // - first pass through the coordIndex array
  //    - fill the vIndexMap array
  //    - fill the coordIndexOut array for root corner indices of the
  //      partition
  //
  // - second pass through the coordIndex array
  //    - fill the coordIndexOut array for non-root corner indices of
  //      the partition
  int nC = getNumberOfCorners();
  int nE = getNumberOfEdges();
  Partition partition(nC);
  vector<int> firstIncidentHalfEdge(nE, -1);

  for (int iC=0; iC<nC; iC++) {
    if (_coordIndex[iC] == -1) continue;
    int vSrc = getSrc(iC);
    int vDst = getDst(iC);
    int iE = getEdge(min(vSrc, vDst), max(vSrc, vDst));
    if (firstIncidentHalfEdge[iE] == -1) {
      firstIncidentHalfEdge[iE] = iC;
    } else {
      int iCt = firstIncidentHalfEdge[iE];
      if (getSrc(iC) == getDst(iCt)) {
        partition.join(iC, getNext(iCt));
        partition.join(getNext(iC), iCt);
      } else {
        partition.join(iC, iCt);
        partition.join(getNext(iC), getNext(iCt));
      }
    }
  }

  int nF = getNumberOfFaces();
  int nV = getNumberOfVertices();
  int nVout = partition.getNumberOfParts() - nF;
  if (nVout == nV) return;

  for (int iC=0; iC<nC; iC++) {
    coordIndexOut.push_back(-1);
  }

  int vertexCounter=0;
  for (int iC; iC<nC; iC++) {
    if (_coordIndex[iC] == -1) continue;
    if (partition.find(iC) == iC) {
      coordIndexOut[iC] = vertexCounter;
      vIndexMap.push_back(_coordIndex[iC]);
      vertexCounter++;
    }
  }

  for (int iC; iC<nC; iC++) {
    if (_coordIndex[iC] == -1) continue;
    int iCroot = partition.find(iC);
    if (iCroot != iC) {
      coordIndexOut[iC] = coordIndexOut[iCroot];
    }
  }

}

// convert to manifold
// - removes isolated vertices, cuts through singular vertices and
//   through singular edges
// - the new number of vertices nVout should be => that the original
//   number of vertices nV
// - if nVout==nV, should return empty vIndexMap and coordIndexOut
// - otherwise
// - fills the vIndexMap array, of size nVout, with an input vertex
//   input in the range 0<=iV<nV for each output vertex index
//   0<=iVout<nVout
// - fills the output coordIndexOut array with vertex
//   indices in the output range 0<=iVout<nVout
// - the output coordIndexOut should be of the same size as the
//   input coordIndex array

void PolygonMesh::convertToManifold
(vector<int>& vIndexMap, vector<int>& coordIndexOut) {
  bool success = false;
  vIndexMap.clear();
  coordIndexOut.clear();

  // HINTS :
  //
  // - similar to the previous method, but pairs of join operations
  //   are performed only o only consistently oriented regular edges
  //
  // - if the mesh is not oriented, it may be cut into multiple
  //   connected components, and or holes will be created
  //
  // - to prevent these problems the orient() method should be calle
  // - before this one

  int nC = getNumberOfCorners();
  int nE = getNumberOfEdges();
  Partition partition(nC);
  vector<int> firstIncidentHalfEdge(nE, -1);

  for (int iC=0; iC<nC; iC++) {
    if (_coordIndex[iC] == -1) continue;
    int vSrc = getSrc(iC);
    int vDst = getDst(iC);
    int iE = getEdge(min(vSrc, vDst), max(vSrc, vDst));
    if (isSingularEdge(iE)) continue;
    if (firstIncidentHalfEdge[iE] == -1) {
      firstIncidentHalfEdge[iE] = iC;
    } else {
      int iCt = firstIncidentHalfEdge[iE];
      if (getSrc(iC) == getDst(iCt)) {
        partition.join(iC, getNext(iCt));
        partition.join(getNext(iC), iCt);
      } else {
        partition.join(iC, iCt);
        partition.join(getNext(iC), getNext(iCt));
      }
    }
  }

  int nF = getNumberOfFaces();
  int nV = getNumberOfVertices();
  int nVout = partition.getNumberOfParts() - nF;
  if (nVout == nV) return;

  for (int iC=0; iC<nC; iC++) {
    coordIndexOut.push_back(-1);
  }

  int vertexCounter=0;
  for (int iC; iC<nC; iC++) {
    if (_coordIndex[iC] == -1) continue;
    if (partition.find(iC) == iC) {
      coordIndexOut[iC] = vertexCounter;
      vIndexMap.push_back(_coordIndex[iC]);
      vertexCounter++;
    }
  }

  for (int iC; iC<nC; iC++) {
    if (_coordIndex[iC] == -1) continue;
    int iCroot = partition.find(iC);
    if (iCroot != iC) {
      coordIndexOut[iC] = coordIndexOut[iCroot];
    }
  }
}
