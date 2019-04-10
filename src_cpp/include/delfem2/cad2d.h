#ifndef CAD2D_h
#define CAD2D_h

#include "delfem2/vec2.h"

#include "delfem2/funcs_gl.h"

class CCadTopo
{
public:
  CCadTopo(){
	nVertex = 0;
  }
  void Clear(){
    nVertex = 0;
    aEdge.clear();
    aFace.clear();
  }
  void AddPolygon(int np){
    const int iv0 = nVertex;
    nVertex += np;
    const int ie0 = aEdge.size();
    for(int iie=0;iie<np;++iie){
      CEdge edge0;
      edge0.iv0 = iv0 + (iie+0)%np;
      edge0.iv1 = iv0 + (iie+1)%np;
      aEdge.push_back(edge0);
    }
    CFace face0;
    for(int iie=0;iie<np;++iie){
      face0.aIE.push_back( std::make_pair(ie0+iie,true ) );
    }
    aFace.push_back(face0);
  }
public:
  class CEdge{
  public:
    int iv0,iv1;
  };
  class CFace{
  public:
    std::vector<int> GetArray_IdVertex(const std::vector<CEdge>& aEdge) const
    {
      std::vector<int> res;
      for(int ie=0;ie<aIE.size();++ie){
        const int ie0 = aIE[ie].first;
        const bool dir = aIE[ie].second;
        const int iv0 = dir ? aEdge[ie0].iv0 : aEdge[ie0].iv1;
        res.push_back(iv0);
      }
      return res;
    }
  public:
    std::vector< std::pair<int,bool> > aIE; // index of edge, is this edge ccw?
  };
public:
  int nVertex;
  std::vector<CEdge> aEdge;
  std::vector<CFace> aFace;
};



class CCad2D_VtxGeo{
public:
  CCad2D_VtxGeo(const CVector2& p) : pos(p){}
public:
  CVector2 pos;
};
class CCad2D_EdgeGeo{
public:
  void GenMesh(unsigned int iedge, const CCadTopo& topo,
               std::vector<CCad2D_VtxGeo>& aVtxGeo);
  double Distance(double x, double y) const;
public:
  CVector2 p0,p1;
  std::vector<CVector2> aP;
};
class CCad2D_FaceGeo{
public:
  //    std::vector<CVector2> aP;
  std::vector<int> aTri;
  std::vector<double> aXY;
public:
  void GenMesh(unsigned int iface0, const CCadTopo& topo, 
               std::vector<CCad2D_EdgeGeo>& aEdgeGeo);
};

//////////////////

class CCad2D
{
public:
  CCad2D(){
    std::cout << "CCAD2D -- construct" << std::endl;
    ivtx_picked = -1;
    is_draw_face = true;
  }
  void Clear(){
    aVtx.clear();
    aEdge.clear();
    aFace.clear();
    topo.Clear();
  }
  void Draw() const;
  // btn-- left:0, middle:2, right:1
  // action-- down:1, up:0
  void Mouse(int btn,int action,int mods,
             const std::vector<double>& src,
             const std::vector<double>& dir,
             double view_height);
  void Motion(const std::vector<double>& src0,
              const std::vector<double>& src1,
              const std::vector<double>& dir);
  std::vector<double> MinMaxXYZ() const;
  void AddPolygon(const std::vector<double>& aXY);
  void Meshing(std::vector<double>& aXY,
               std::vector<int>& aTri,
               double len) const;
  void GetPointsEdge(std::vector<int>& aIdP,
                     const double* pXY, int np,
                     const std::vector<int>& aIE,
                     double tolerance ) const;
  std::vector<double> GetVertexXY_Face(int iface) const;
public:
  CCadTopo topo;
  /////
  std::vector<CCad2D_VtxGeo> aVtx;
  std::vector<CCad2D_EdgeGeo> aEdge;
  std::vector<CCad2D_FaceGeo> aFace;
  int ivtx_picked;
  
  bool is_draw_face;
};



#endif
