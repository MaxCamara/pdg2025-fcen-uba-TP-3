//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-05 23:18:19 taubin>
//------------------------------------------------------------------------
//
// dgpTest3.cpp
//
// Software developed for course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Brown University nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL GABRIEL TAUBIN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <string>
#include <iostream>

using namespace std;

#include <wrl/SceneGraphTraversal.hpp>

#include <io/AppLoader.hpp>
#include <io/AppSaver.hpp>
#include <io/LoaderPly.hpp>
#include <io/LoaderStl.hpp>
#include <io/LoaderWrl.hpp>
#include <io/SaverPly.hpp>
#include <io/SaverStl.hpp>
#include <io/SaverWrl.hpp>

#include <core/PolygonMesh.hpp>
#include <core/PolygonMeshTest.hpp>

#include "dgpPrt.hpp"

enum Operation {
  NONE,
  COMPUTE_CC_PRIMAL,
  COMPUTE_CC_DUAL,
  IS_ORIENTED,
  IS_ORIENTABLE,
  ORIENT,
  REMOVE_ISOLATED_VERTICES,
  CUT_THROUGH_SINGULAR_VERTICES,
  CONVERT_TO_MANIFOLD
};

// TODO Mon Mar 6 2023
// - add code to test

// PolygonMesh::computeConnectedComponentsPrimal()
// PolygonMesh::computeConnectedComponentsDual()
// PolygonMesh::isOriented()
// PolygonMesh::isOrientable()
// PolygonMesh::orient()
// PolygonMesh::removeIsolatedVertices()
// PolygonMesh::cutThroughSingularVertices()
// PolygonMesh::convertToManifold()

class Data {
public:
  bool   _debug;
  bool   _binaryOutput;
  bool   _removeProperties;

  // TODO Mon Mar 6 2023
  // - add variables to specify the operation to be performed
  Operation _operation;

  string _inFile;
  string _outFile;
public:
  Data():
    _debug(false),
    _binaryOutput(false),
    _removeProperties(false),
    _operation(NONE),
    _inFile(""),
    _outFile("")
  { }
};

void options(Data& D) {
  cout << "   -d|-debug               [" << tv(D._debug)            << "]" << endl;
  cout << "   -b|-binaryOutput        [" << tv(D._binaryOutput)     << "]" << endl;
  cout << "   -r|-removeProperties    [" << tv(D._removeProperties) << "]" << endl;

  // TODO Mon Mar 6 2023
  // - add line(s) to explain how to specify the operation to be performed

  cout << " -ccp|-ccPrimal            ["
       << tv(D._operation==Operation::COMPUTE_CC_PRIMAL) << "]" << endl;
  cout << " -ccd|-ccDual              ["
       << tv(D._operation==Operation::COMPUTE_CC_DUAL) << "]" << endl;
  cout << "  -od|-oriented            ["
       << tv(D._operation==Operation::IS_ORIENTED) << "]" << endl;
  cout << "  -oe|-orientable          ["
       << tv(D._operation==Operation::IS_ORIENTABLE) << "]" << endl;
  cout << "   -o|-orient              ["
       << tv(D._operation==Operation::ORIENT) << "]" << endl;
  cout << "  -ri|-removeIsolated      ["
       << tv(D._operation==Operation::REMOVE_ISOLATED_VERTICES) << "]" << endl;
  cout << "  -cs|-cutThroughSingular  ["
       << tv(D._operation==Operation::CUT_THROUGH_SINGULAR_VERTICES) << "]" << endl;
  cout << "   -m|-manifold            ["
       << tv(D._operation==Operation::CONVERT_TO_MANIFOLD) << "]" << endl;
}

void usage(Data& D) {
  cout << "USAGE: dgpTest3 [options] inFile outFile" << endl;
  cout << "   -h|-help" << endl;
  options(D);
  cout << endl;
  exit(0);
}

void error(const char *msg) {
  cout << "ERROR: dgpTest3 | " << ((msg)?msg:"") << endl;
  exit(0);
}

//////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {

  Data D;

  if(argc==1) usage(D);

  for(int i=1;i<argc;i++) {
    if(string(argv[i])=="-h" || string(argv[i])=="-help") {
      usage(D);
    } else if(string(argv[i])=="-d" || string(argv[i])=="-debug") {
      D._debug = !D._debug;
    } else if(string(argv[i])=="-b" || string(argv[i])=="-binaryOutput") {
      D._binaryOutput = !D._binaryOutput;
    } else if(string(argv[i])=="-r" || string(argv[i])=="-removeProperties") {
      D._removeProperties = !D._removeProperties;

      // TODO Mon Mar 6 2023
      // - add code to parse the desired operation to be performed
      // - from the command line

    } else if(string(argv[i])=="-ccp" || string(argv[i])=="-ccPrimal") {
      D._operation = Operation::COMPUTE_CC_PRIMAL;

    } else if(string(argv[i])=="-ccd" || string(argv[i])=="-ccDual") {
      D._operation = Operation::COMPUTE_CC_DUAL;

    } else if(string(argv[i])=="-od" || string(argv[i])=="-oriented") {
      D._operation = Operation::IS_ORIENTED;

    } else if(string(argv[i])=="-oe" || string(argv[i])=="-orientable") {
      D._operation = Operation::IS_ORIENTABLE;

    } else if(string(argv[i])=="-o" || string(argv[i])=="-orient") {
      D._operation = Operation::ORIENT;

    } else if(string(argv[i])=="-ri" || string(argv[i])=="-removeIsolated") {
      D._operation = Operation::REMOVE_ISOLATED_VERTICES;

    } else if(string(argv[i])=="-cs" || string(argv[i])=="-cutThroughSingular") {
      D._operation = Operation::CUT_THROUGH_SINGULAR_VERTICES;

    } else if(string(argv[i])=="-m" || string(argv[i])=="-manifold") {
      D._operation = Operation::CONVERT_TO_MANIFOLD;

    } else if(string(argv[i])[0]=='-') {
      error("unknown option");
    } else if(D._inFile=="") {
      D._inFile = string(argv[i]);
    } else if(D._outFile=="") {
      D._outFile = string(argv[i]);
    }
  }

  if(D._inFile =="") error("no inFile");

  // if D._outFile is not specified then no output file will be written
  // if(D._outFile=="") error("no outFile");

  if(D._debug) {
    cout << "dgpTest3 {" << endl;
    cout << endl;
    options(D);
    cout << endl;
    cout << "  inFile  = " << D._inFile << endl;
    cout << "  outFile = " << D._outFile << endl;
    cout << endl;
    fflush(stderr);
  }

  bool success;

  //////////////////////////////////////////////////////////////////////
  // create loader and saver factories
  AppLoader loaderFactory;
  AppSaver  saverFactory;

  // register input file loaders
  LoaderPly* plyLoader = new LoaderPly();
  loaderFactory.registerLoader(plyLoader);
  LoaderStl* stlLoader = new LoaderStl();
  loaderFactory.registerLoader(stlLoader);
  LoaderWrl* wrlLoader = new LoaderWrl();
  loaderFactory.registerLoader(wrlLoader);

  //  If SaverPly::setDefaultDataType is used, it must be called
  //  before the Saver constructor; otherwise SaverPly::setDataType
  //  should be called after to set the proper value for the private
  //  variable SaverPly::_dataType before this instance of SaverPly is
  //  used

  // register output file savers  
  SaverPly* plySaver = new SaverPly();
  saverFactory.registerSaver(plySaver);
  SaverStl* stlSaver = new SaverStl();
  saverFactory.registerSaver(stlSaver);
  SaverWrl* wrlSaver = new SaverWrl();
  saverFactory.registerSaver(wrlSaver);

  SaverStl::FileType stlFt =
    (D._binaryOutput)?SaverStl::FileType::BINARY:SaverStl::FileType::ASCII;
  stlSaver->setFileType(stlFt);

  Ply::DataType plyDt =
    (D._binaryOutput)?Ply::DataType::BINARY_LITTLE_ENDIAN:Ply::DataType::ASCII;
  plySaver->setDataType(plyDt);

  if(D._debug) {
    SaverPly::setOstream(&cout);
    SaverPly::setIndent("    ");
  }

  //////////////////////////////////////////////////////////////////////
  // read SceneGraph

  SceneGraph wrl; // create empty scene graph

  if(D._debug) {
    cout << "  loading inFile {" << endl;
  }

  success = loaderFactory.load(D._inFile.c_str(),wrl);

  if(D._debug) {
    cout << "    success        = " << tv(success)          << endl;
    cout << "  } loading inFile" << endl;
    cout << endl;
  }

  if(success==false) return -1;

  if(D._removeProperties) {
    if(D._debug) cout << "  removing properties {" << endl;
    Node* node;
    SceneGraphTraversal sgt(wrl);
    for(int iIfs=0;(node=sgt.next())!=(Node*)0;iIfs++) {
      Shape* shape = dynamic_cast<Shape*>(node);
      if(shape==(Shape*)0) continue;
      IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(shape->getGeometry());
      if(ifs==(IndexedFaceSet*)0) continue;
      ifs->setNormalPerVertex(true);
      ifs->getNormal().clear();
      ifs->getNormalIndex().clear();
      ifs->setColorPerVertex(true);
      ifs->getColor().clear();
      ifs->getColorIndex().clear();
      ifs->getTexCoord().clear();
      ifs->getTexCoordIndex().clear();
    }
    if(D._debug) cout << "  } removing properties" << endl;
    if(D._debug) cout << endl;
  }

  // print PolygonMesh info before processing
  if(D._debug) {
    cout << "  before processing" << endl;
    PolygonMeshTest(wrl,"  ");
    cout << endl;
  }

  // process
  
  if(D._debug) cout << "  processing {" << endl;

  // TODO Mon Mar 6 2023
  // - add code to perform the desired operation
  // - for each case print some information about the results

  // PolygonMesh::computeConnectedComponentsPrimal()
  // PolygonMesh::computeConnectedComponentsDual()
  // PolygonMesh::isOriented()
  // PolygonMesh::isOrientable()
  // PolygonMesh::orient()
  // PolygonMesh::removeIsolatedVertices()
  // PolygonMesh::cutThroughSingularVertices()
  // PolygonMesh::convertToManifold()

  Node* node;
  SceneGraphTraversal sgt(wrl);

  switch(D._operation) {
  case Operation::COMPUTE_CC_PRIMAL:
    // perform the operation here
    cout << "    calculando componentes conexas del primal graph" << endl;
    for(int iIfs=0;(node=sgt.next())!=(Node*)0;iIfs++) {
      Shape* shape = dynamic_cast<Shape*>(node);
      if(shape==(Shape*)0) continue;
      IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(shape->getGeometry());
      if(ifs==(IndexedFaceSet*)0) continue;

      int nV = ifs->getNumberOfVertices();
      PolygonMesh polygon_mesh(nV, ifs->getCoordIndex());
      vector<int> faceLabel;
      int nCC = polygon_mesh.computeConnectedComponentsPrimal(faceLabel);

      cout << "      IndexedFaceSet iIfs[" << iIfs <<"]:" << endl;
      cout << "        nCC_primal = " << nCC << endl;

      if (nCC == 0) continue;

      int nF = polygon_mesh.getNumberOfFaces();
      for (int iF=0; iF<nF; iF++) {
        cout << "        faceLabel[" << iF << "] = " << faceLabel[iF] << endl;
      }
    }
    break;
  case Operation::COMPUTE_CC_DUAL:
    // perform the operation here
    cout << "    calculando componentes conexas del dual graph" << endl;
    for(int iIfs=0;(node=sgt.next())!=(Node*)0;iIfs++) {
      Shape* shape = dynamic_cast<Shape*>(node);
      if(shape==(Shape*)0) continue;
      IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(shape->getGeometry());
      if(ifs==(IndexedFaceSet*)0) continue;

      int nV = ifs->getNumberOfVertices();
      PolygonMesh polygon_mesh(nV, ifs->getCoordIndex());
      vector<int> faceLabel;
      int nCC = polygon_mesh.computeConnectedComponentsDual(faceLabel);

      cout << "      IndexedFaceSet iIfs[" << iIfs <<"]:" << endl;
      cout << "        nCC_dual = " << nCC << endl;

      if (nCC == 0) continue;

      int nF = polygon_mesh.getNumberOfFaces();
      for (int iF=0; iF<nF; iF++) {
        cout << "        faceLabel[" << iF << "] = " << faceLabel[iF] << endl;
      }
    }
    break;
  case Operation::IS_ORIENTED:
    // perform the operation here
    cout << "    determinando si la malla está consistentemente orientada" << endl;
    for(int iIfs=0;(node=sgt.next())!=(Node*)0;iIfs++) {
      Shape* shape = dynamic_cast<Shape*>(node);
      if(shape==(Shape*)0) continue;
      IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(shape->getGeometry());
      if(ifs==(IndexedFaceSet*)0) continue;

      int nV = ifs->getNumberOfVertices();
      PolygonMesh polygon_mesh(nV, ifs->getCoordIndex());

      cout << "      IndexedFaceSet iIfs[" << iIfs <<"]:" << endl;
      cout << "        isOriented = " << tv(polygon_mesh.isOriented()) << endl;
    }
    break;
  case Operation::IS_ORIENTABLE:
    // perform the operation here
    cout << "    determinando si la malla es orientable" << endl;
    for(int iIfs=0;(node=sgt.next())!=(Node*)0;iIfs++) {
      Shape* shape = dynamic_cast<Shape*>(node);
      if(shape==(Shape*)0) continue;
      IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(shape->getGeometry());
      if(ifs==(IndexedFaceSet*)0) continue;

      int nV = ifs->getNumberOfVertices();
      PolygonMesh polygon_mesh(nV, ifs->getCoordIndex());

      cout << "      IndexedFaceSet iIfs[" << iIfs <<"]:" << endl;
      cout << "        isOrientable = " << tv(polygon_mesh.isOrientable()) << endl;
    }
    break;
  case Operation::ORIENT:
    // perform the operation here
    cout << "    intentando orientar la malla" << endl;
    for(int iIfs=0;(node=sgt.next())!=(Node*)0;iIfs++) {
      Shape* shape = dynamic_cast<Shape*>(node);
      if(shape==(Shape*)0) continue;
      IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(shape->getGeometry());
      if(ifs==(IndexedFaceSet*)0) continue;

      int nV = ifs->getNumberOfVertices();
      vector<int>& coordIndex = ifs->getCoordIndex();
      PolygonMesh polygon_mesh(nV, coordIndex);

      vector<int> ccIndex;
      vector<bool> invert_face;
      int nCC = polygon_mesh.orient(ccIndex, invert_face);

      cout << "      IndexedFaceSet iIfs[" << iIfs <<"]:" << endl;

      if (nCC == 0) {
        cout << "        la malla no es orientable" << endl;
        break;
      }

      cout << "        nCC = " << nCC << endl;

      int nC = polygon_mesh.getNumberOfCorners();
      int iC0, iC1, iF;
      for (iC0=iC1=iF=0; iC1<nC; iC1++) {
        if(coordIndex[iC1]>=0) continue;
        cout << "        invert_face[" << iF << "] = " << tv(invert_face[iF]) << endl;
        cout << "        ccIndex[" << iF << "]     = " << ccIndex[iF] << endl;
        if (invert_face[iF]) {
          int iCfront = iC0;
          int iCback = iC1-1;
          while (iCfront < iCback) {
            int tmp = coordIndex[iCfront];
            coordIndex[iCfront] = coordIndex[iCback];
            coordIndex[iCback] = tmp;
            iCfront++;
            iCback--;
          }
        }
        iC0 = iC1+1;
        iF++;
      }

      cout << "        la malla fue orientada con éxito" << endl;
    }
    break;
  case Operation::REMOVE_ISOLATED_VERTICES:
    // perform the operation here
    cout << "    removiendo vértices aislados" << endl;
    for(int iIfs=0;(node=sgt.next())!=(Node*)0;iIfs++) {
      Shape* shape = dynamic_cast<Shape*>(node);
      if(shape==(Shape*)0) continue;
      IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(shape->getGeometry());
      if(ifs==(IndexedFaceSet*)0) continue;

      int nV = ifs->getNumberOfVertices();
      PolygonMesh polygon_mesh(nV, ifs->getCoordIndex());

      vector<int> coordMap;
      vector<int> coordIndexOut;
      bool success = polygon_mesh.removeIsolatedVertices(coordMap, coordIndexOut);

      cout << "      IndexedFaceSet iIfs[" << iIfs <<"]:" << endl;

      if (!success) {
        cout << "        la malla no tiene vértices aislados" << endl;
        break;
      }

      int nVout = coordMap.size();

      cout << "        cantidad de vértices removidos = " << nV - nVout << endl;
      cout << "        cantidad de vértices restantes = " << nVout << endl;

      cout << "        mapeo de vértices:" << endl;
      for (int iV=0; iV<nVout; iV++) {
        cout << "          coordMap[" << iV << "] = " << coordMap[iV] << endl;
      }

      int nC = coordIndexOut.size();
      cout << "        nuevo coordIndex:" << endl;
      cout << "          [ ";
      for (int iC=0; iC<nC; iC++) {
        cout << coordIndexOut[iC];
        if (coordIndexOut[iC]>=0) {
          cout << " ";
        } else {
          if (iC<nC-1) {
            cout << endl;
            cout << "            ";
          } else {
            cout << " ]" << endl;
          }
        }
      }


    }
    break;
  case Operation::CUT_THROUGH_SINGULAR_VERTICES:
    // perform the operation here
    cout << "    recortando por los vértices singulares" << endl;
    for(int iIfs=0;(node=sgt.next())!=(Node*)0;iIfs++) {
      Shape* shape = dynamic_cast<Shape*>(node);
      if(shape==(Shape*)0) continue;
      IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(shape->getGeometry());
      if(ifs==(IndexedFaceSet*)0) continue;

      int nV = ifs->getNumberOfVertices();
      PolygonMesh polygon_mesh(nV, ifs->getCoordIndex());

      vector<int> vIndexMap;
      vector<int> coordIndexOut;

      polygon_mesh.cutThroughSingularVertices(vIndexMap, coordIndexOut);

      cout << "      IndexedFaceSet iIfs[" << iIfs <<"]:" << endl;

      int nVout = vIndexMap.size();
      if (nVout==0) {
        cout << "        la malla no tiene vértices singulares que separan componentes conexas" << endl;
        break;
      }

      int nI = polygon_mesh.numberOfIsolatedVertices();
      cout << "        cantidad de vértices aislados removidos = " << nI << endl;
      cout << "        nVout = " << nVout << endl;

      cout << "        mapeo de vértices:" << endl;
      for (int iV=0; iV<nVout; iV++) {
        cout << "          coordMap[" << iV << "] = " << vIndexMap[iV] << endl;
      }

      int nC = coordIndexOut.size();
      cout << "        nuevo coordIndex:" << endl;
      cout << "          [ ";
      for (int iC=0; iC<nC; iC++) {
        cout << coordIndexOut[iC];
        if (coordIndexOut[iC]>=0) {
          cout << " ";
        } else {
          if (iC<nC-1) {
            cout << endl;
            cout << "            ";
          } else {
            cout << " ]" << endl;
          }
        }
      }

    }
    break;
  case Operation::CONVERT_TO_MANIFOLD:
    // perform the operation here
    cout << "    convirtiendo la malla a manifold" << endl;
    for(int iIfs=0;(node=sgt.next())!=(Node*)0;iIfs++) {
      Shape* shape = dynamic_cast<Shape*>(node);
      if(shape==(Shape*)0) continue;
      IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(shape->getGeometry());
      if(ifs==(IndexedFaceSet*)0) continue;

      int nV = ifs->getNumberOfVertices();
      PolygonMesh polygon_mesh(nV, ifs->getCoordIndex());

      vector<int> vIndexMap;
      vector<int> coordIndexOut;

      polygon_mesh.convertToManifold(vIndexMap, coordIndexOut);

      cout << "      IndexedFaceSet iIfs[" << iIfs <<"]:" << endl;

      int nVout = vIndexMap.size();
      if (nVout==0) {
        cout << "        la malla ya es manifold" << endl;
        break;
      }

      int nI = polygon_mesh.numberOfIsolatedVertices();
      cout << "        cantidad de vértices aislados removidos = " << nI << endl;
      cout << "        nVout = " << nVout << endl;

      cout << "        mapeo de vértices:" << endl;
      for (int iV=0; iV<nVout; iV++) {
        cout << "          coordMap[" << iV << "] = " << vIndexMap[iV] << endl;
      }

      int nC = coordIndexOut.size();
      cout << "        nuevo coordIndex:" << endl;
      cout << "          [ ";
      for (int iC=0; iC<nC; iC++) {
        cout << coordIndexOut[iC];
        if (coordIndexOut[iC]>=0) {
          cout << " ";
        } else {
          if (iC<nC-1) {
            cout << endl;
            cout << "            ";
          } else {
            cout << " ]" << endl;
          }
        }
      }
    }
    break;
  case Operation::NONE:
  default:
    break;
  }

  if(D._debug) cout << "  } processing" << endl;
  
  // print PolygonMesh info after processing
  if(D._debug) {
    cout << "  after processing" << endl;
    PolygonMeshTest(wrl,"  ");
    cout << endl;
  }

  // write output file
  if(D._outFile!="") {

    if(D._debug) {
      cout << "  saving outFile {" << endl;
    }
        
    success = saverFactory.save(D._outFile.c_str(),wrl);
        
    if(D._debug) {
      cout << "    success        = " << tv(success)          << endl;
      cout << "  }" << endl;
      cout << endl;
    }

  } else {
    if(D._debug) {
      cout << "  no outFile written" << endl;
    }
  }
    
  //////////////////////////////////////////////////////////////////////

  if(D._debug) {
    cout << "} dgpTest3" << endl;
    fflush(stderr);
  }

  return 0;
}
