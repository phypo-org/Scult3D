#ifndef H__Entity__H
#define H__Entity__H


#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>
#include <set>
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>
#include <set>
#include <unordered_set>
#include <vector>


#include "Point3d.h"
#include "Calcul3d.h"
#include "Matrice.h"

#include "EntityVisitor.h"

namespace PP3d {

  class Entity;
  class Point;
  class Line;
  class Facet;
  class Poly;
  class Object;
  class Mat4;

  class EntityVisitorType;

	
  using EntityId      = GLuint; // au moins 32 bits
	
  using VectEntityPtr = std::vector<Entity*>;

							 
  //*********************************************

  class Entity {
		
  protected:

    EntityId          cId=0;
		
    std::set<Entity*> cOwners;

    bool              cIsSelected=false;
    bool              cIsHighlight=false;


    bool              cTreeOpen=false;  // for interface
    
  private:
    void     setId(EntityId iId) { cId = iId; }
	         
  public:
    virtual ShapeType getType() const=0;
		
    Entity()
    {
    }
    virtual ~Entity();

    void deleteAllHieracrchy();
		
    EntityId getId() { return cId; }
    bool     isIdVoid() { return cId==0;}

    bool isTreeOpen() { return cTreeOpen; }
    void setTreeOpen( bool iVal ) { cTreeOpen=iVal;}


  public:
    bool isSelect()                 { return   cIsSelected;}
    void setSelect( bool pFlag )    { cIsSelected  = pFlag;}		
    bool& isHighlight()              { return   cIsHighlight;} // la reference pour pouvoir la remettre a zero ! c'est vraiment moche !
    
    void setHighlight( bool pFlag ) { cIsHighlight =  pFlag;}

    auto & getOwners() { return  cOwners; }
    Entity * firstOwner()
    {
      Entity* lTmp = nullptr;
      for( Entity* lOwner  :  cOwners ) { return lOwner;}
      return lTmp;
    }
    

    //    void   addOwner     ( Entity* pOwner ) { cOwners.emplace( pOwner); }
    void   addOwner     ( Entity* pOwner ) { cOwners.emplace( pOwner); }
    void   removeOwner  ( Entity* pOwner ) { cOwners.erase( pOwner); }
    size_t howManyOwner ( ) { return cOwners.size(); }
    void   clearOwner   ( Entity* pOwner ) { cOwners.erase( pOwner); }
    void   clearAllOwner( ) { cOwners.clear(); }

    virtual bool clear() { clearAllOwner();  return true; }
		
    const char* getStrType() const { return GetStrShapeType( getType()) ; }
		
    friend std::ostream & operator << ( std::ostream & pOs, Entity& pEntity )
    {
      pOs << "Id:"      << pEntity.cId
	  << " Owners:" << pEntity.cOwners.size()
	  << " Type:"   << pEntity.getStrType() 
	  << " " << (void*)&pEntity ;
      return pOs;
    }
		

    virtual void execVisitor( EntityVisitor& pVisit ) = 0;
    virtual void execVisitor( EntityVisitorNode& pVisit ) = 0;

			
    void move   ( Point3d p );
    void scale  ( Point3d p );
    void modify ( Mat4& pMat );

    static Object* FindMyObject();

    friend class DataBase;
    friend class MyRead;
    friend class Line;
    friend class Facet;
    friend class Poly;
    friend class Poly;
  };
	
  using EntityPtr=Entity*;
  using EntityPtrVect = std::vector<Entity*>;
  using EntityPtrSet  = std::set<Entity*>;
  using EntityPtrHash = std::unordered_set<Entity*>;

  template <class SET = std::set<Entity*> >
  static inline void ExecVisitor( SET & ioSet, EntityVisitor & pVisit )    
  {
    for( Entity * lEntity : ioSet )
      lEntity->execVisitor( pVisit );
  }
  template <class SET=std::set<Entity*>>
  static inline void ExecVisitorNode( SET  & ioSet, EntityVisitorNode & pVisit )    
  {
    for( Entity * lEntity : ioSet )
      lEntity->execVisitor( pVisit );
  }

  
  //*********************************************
  class Point :  public Entity {
		
  public:
    Point3d cPt;

  public:
    Point( const Point3d & p )
      :cPt( p )
    {			
    }		
		
    ShapeType getType() const override { return ShapeType::Point;}

    Point3d  get() const { return cPt; }
    Point3d& get()       { return cPt; }
		
    void execVisitor( EntityVisitor& pVisit ) override;

  protected:
    void execVisitor( EntityVisitorNode& pVisit ) override;
    friend class Line;

    bool clear() override { return Entity::clear(); }

    friend std::ostream & operator << ( std::ostream & pOs, Point& pEntity )
    {
      pOs << ((Entity&)pEntity) <<  " " << pEntity.get() ;
      return pOs;
    }
  };
	  
  using PointPtr     = Point*;
  using PointPtrPair = std::pair<PP3d::Point*,PP3d::Point*>;
  using PointPtrVect = std::vector<PP3d::Point*>;
  using PointPtrSet  = std::set<PP3d::Point*>;
  using PointPtrHash = std::unordered_set<PP3d::Point*>;


  inline static void operator *= (PointPtrSet pSet, const Mat4& pMat )
  {
    for( auto lIter = pSet.begin(); lIter != pSet.end(); ++lIter) 
      (*lIter)->cPt *= pMat;
  }
  inline static void operator *= (PointPtrHash pSet, const Mat4& pMat )
  {
    for( auto lIter = pSet.begin(); lIter != pSet.end(); ++lIter) 
      (*lIter)->cPt *= pMat;
  }
  inline static PointPtrPair Reverse(const PointPtrPair& iPair ) {  return PointPtrPair( iPair.second, iPair.first ); }

  //*********************************************

  class Line  :  public Entity {

    PointPtrPair cPoints={nullptr,nullptr};
		
		
  public:
    
    Line( PointPtr lA, PointPtr lB )
    {		 
      cPoints = { lA, lB };
      lA->addOwner( this );
      if( lA != lB )
	lB->addOwner( this );			
    }
    bool isPoint() { return cPoints.first == cPoints.second; } // pour la saisie du premier point d'une facette
    bool isSamePointXYZ() { return cPoints.first->get() == cPoints.second->get(); } // Meme coordonnes xyz de deux points
    bool isSamePointEpsi(long double iEpsi ) { return cPoints.first->get().sameEpsi(iEpsi, cPoints.second->get() ); } // Meme coordonnes xyz de deux points
								 
    ShapeType   getType() const override { return ShapeType::Line;}
		
    PointPtrPair& getPoints() { return cPoints; }
    //    PointPtrPair  getInversedPoints(){ cPoints.PointPtrPair lPair( second(), first(); }
    PointPtr      getFirst()  const { return cPoints.first;}
    PointPtr      getSecond() const { return cPoints.second;}
    PointPtr      first()  const { return cPoints.first;}
    PointPtr      second() const { return cPoints.second;}
    PointPtr&     first() { return cPoints.first;}
    PointPtr&     second(){ return cPoints.second;}
    /*
    void set( PointPtr lA, PointPtr lB )
    {
      if( getFirst() )  getFirst()->removeOwner(this);
      if( getSecond() ) getSecond()->removeOwner(this);
	
      cPoints = { lA, lB };
      lA->addOwner( this );
      if( lA != lB )
	lB->addOwner( this );			
    }
    */
    void setSecond( PointPtr lB )
    {
      
    }

    void execVisitor( EntityVisitor& pVisit )override;
 
  public:
    void execVisitor( EntityVisitorNode& pVisit )override;
		
    void inversePoint() {
      PointPtr lTmp = cPoints.first;
      cPoints.first = cPoints.second;
      cPoints.second = lTmp;
    }
    
    Line* getReverseLine();
    
    friend class Facet;
    friend class Maker;

    friend std::ostream & operator << ( std::ostream & pOs, Line& pEntity )
    {
      pOs << ((Entity&)pEntity) <<  " " << pEntity.first() << " " << pEntity.second() ;
      return pOs;
    }
  };
	
  using LinePtr     = Line*;
  using LinePtrVect = std::vector<Line*>;

  //--------------------------------

  struct FindPointPairVisitor : public EntityVisitor{
    const PointPtrPair & cPtPair;
    std::set<LinePtr>    cInternalSet;
    std::set<LinePtr>  & cSet;  // le set elimine les doublons
    
	  
    virtual void execBeginLine ( LinePtr pLine )
    {
      //  std::cout << "FindPointPairVisitor::execBeginLine "
      //	<<  cPtPair.first << ":" <<  pLine->first()
      //	<< "   =========== " 
      //	<<  cPtPair.second << ":" <<  pLine->second()
      //	<< std::endl;
      
      if( cPtPair.first == pLine->first()
	  && cPtPair.second == pLine->second() )
	{
	  //	  std::cout << "FindPointPairVisitor::execBeginLine OK OK OK OK OK OK "
	  //		    <<  pLine->getId()<< std::endl;
  
	  cSet.insert( pLine );
	}
    }
  public:
    std::set<LinePtr> &getLines() { return cSet;}
	  
    FindPointPairVisitor(const PointPtrPair & iPtPair , std::set<LinePtr> & ioSet )
      :cPtPair( iPtPair )
      ,cSet( ioSet ) {
      //    std::cout << "FindPointPairVisitor::execBeginLine 1111111111111111111111111111111111111111111 "
      //	<< iPtPair.first << "  :  " << iPtPair.second
      //	<< cPtPair.first << "  :  " << cPtPair.second
      //	<<  std::endl
      ;}
	  
    FindPointPairVisitor(const PointPtrPair & iPtPair )
      :cPtPair( iPtPair )
      ,cSet( cInternalSet ) {
      //   std::cout << "FindPointPairVisitor::execBeginLine 2222222222222222222222222222222222222222222 "
      //		<< iPtPair.first << "  :  " << iPtPair.second
      //		<< cPtPair.first << "  :  " << cPtPair.second
      //		<<  std::endl
      ;}
  };								 

  inline Line * MakeLine( const Point3d & iA,   const Point3d & iB )
  {
    return new Line( new Point( iA ), new Point (iB ));
  }

  //*********************************************
  class  Facet :  public Entity{
		
    LinePtrVect cLines;

    Point3d cNorm;
    Point3d cNoNorm;

  public:
    Facet() {;}
    Facet(  LinePtr iLine ) { addLine( iLine ); }
    Facet(  LinePtr iLine1, LinePtr iLine2  ) { addLine( iLine1 ); addLine( iLine2 ); }
    Facet(  LinePtr iLine1, LinePtr iLine2, LinePtr iLine3  ) { addLine( iLine1 ); addLine( iLine2 );addLine( iLine3 ); }

      
    ShapeType getType() const  override { return ShapeType::Facet;}
	
    void closeFacet();

    bool addTrueLine( LinePtr iLine )
    {
      if( iLine->isPoint())
	{
	  //	  std::cout << "addTrueLine - Line is point : same ID" << std::endl;
	  return false;
	}
      
      if( iLine->isSamePointXYZ() )
	{
	  //	  std::cout << "addTrueLine - Line is point : same XYZ" << std::endl;
	  return false;
	}	
      
      addLine( iLine );
      return true;
    }
    bool addTrueLineEpsi( long double iEpsi ,  LinePtr iLine)
    {
      if( iLine->isPoint())
	{
	  //	  std::cout << "addTrueLine - Line is point : same ID" << std::endl;
	  return false;
	}
      
      if( iLine->isSamePointEpsi( iEpsi) )
	{
	  //	  std::cout << "addTrueLine - Line is point : same XYZ" << std::endl;
	  return false;
	}	
      
      addLine( iLine );
      return true;
    }
    void addLine( LinePtr iLine )
    {
      cLines.push_back( iLine );
      iLine->addOwner( this );
    }
    void setLine( size_t iPos, LinePtr iLine )
    {
      cLines[iPos] =  iLine ;
      iLine->addOwner( this );
    }
    void insertLine( size_t iPos, LinePtr iLine )
    {
      cLines.insert( cLines.begin()+iPos, iLine );
      iLine->addOwner( this );
    }
    void addLineAfter( LinePtr iOriginal, LinePtr iNewLine )
    {
      size_t i =0;
      for( size_t i=0; i< cLines.size(); i++ )
	{
	  if( cLines[i] == iOriginal )
	    break;
	}
      if( i < cLines.size() )
	setLine( i+1, iNewLine );
      else
	setLine( i, iNewLine );
    }

    LinePtr getLine(  size_t iPos ) { return cLines[iPos]; }
    LinePtr swapLine(  size_t iPos,  LinePtr iLine )
    {
      LinePtr lTmp =  cLines[iPos];
      iLine->addOwner( this );
      cLines[iPos] = iLine;
      return lTmp;
    }
    bool clear() override { cLines.clear(); return Entity::clear(); }


    LinePtrVect& getLines()   { return cLines;}
    GLuint getNbLines()  const { return (GLuint )cLines.size(); }

    Point3d & getNormal()     { return cNorm; }
		
    void execVisitor( EntityVisitor& pVisit )override;
    void inverseLines( );

  protected:
    void execVisitor( EntityVisitorNode& pVisit )override;
    friend class Poly;
    friend class Maker;
  };
	
  using FacetPtr     = Facet*;
  using FacetPtrVect = std::vector<Facet*>;

  //*********************************************
  class  Poly :  public Entity{
		
    FacetPtrVect cFacets;
		
  public:
    ShapeType getType() const override { return ShapeType::Poly;}
		
    void addFacet( FacetPtr pFacet)
    {
      cFacets.push_back( pFacet );
      pFacet->addOwner( this );
    }

    FacetPtrVect&  getFacets()  { return cFacets; }
    void execVisitor( EntityVisitor& pVisit )override;

    bool clear() override { cFacets.clear();  return Entity::clear(); }

  protected:
    void execVisitor( EntityVisitorNode& pVisit )override;
		
    friend class Object;
    friend class Maker;
  };
  using PolyPtr     = Poly*;
  using PolyPtrVect = std::vector<Poly*>;

  //*********************************************

  template <class MYCLASS>
  class Alloc{   
    static std::vector<MYCLASS> EntityHeap;
    
  public:
    void free( MYCLASS * iPtr, bool iFlagClearOwner = false )
      {
	if( iPtr == nullptr )
	  return;
	
	if( iFlagClearOwner )
	  {
	    iPtr->getOwners().clear();
	  }
	else
	  {
	  if( iPtr->getOwners().size() )
	    return ;
	  }

	
      }
  };

}

#endif
