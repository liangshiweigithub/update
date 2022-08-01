//
// linear algebra math classes and functions
// Vector classes: Vec3f, Vec3d (float and double)
// Matrix classes: Mtx3f, Mtx3d
//                                       (x)
// vectors on column major form, i.e., v=(y)
//                                       (z)
// Author: Tomas Möller
// History: 1991 started (used to be good (on the PC which had some extra C++ features))
//          2000 May, added so that you can write vector[X]=5.0;
//          2000 May, started to add the Vec4 template class

#ifndef VECMATH_H
#define VECMATH_H

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <float.h>


//#include "types.h"

//*** This is potentially dangerous. Fix some day*******
#define X 0
#define Y 1
#define Z 2
#define W 3

#define R 0
#define G 1
#define B 2
#define A 3 /* alpha */
//******************************************************

/* fix this later */
/* #include <limits> */
/* using namespace std; */



template<class CoordType> class Vec2
{
 public:
   CoordType vec[2];
   // constructors and desctructor
   Vec2(); // empty constructor: coords are undefined!!!! =INF if debug
   Vec2(CoordType xc, CoordType yc);
   Vec2(const Vec2 &v);
   ~Vec2() { }
   void set(const Vec2 &v);
   void set(CoordType xc, CoordType yc);
   CoordType length(void) const;                   // length of vector
   CoordType length2(void) const;                  // squared length of vector
   // normalize vector (only if length>0.0), returns *this
   const Vec2<CoordType>& normalize(void);         
   CoordType normalizeIf(void);    // normalize vector (only if length>0.0)
   void operator=(const Vec2 &v);                  // operator: assignment
   Vec2<CoordType> operator*(CoordType t) const;   // operator: scaling by scalar
   Vec2<CoordType> operator+(const Vec2 &v) const; // operator: addition 
   Vec2<CoordType> operator-(const Vec2 &v) const; // operator: subtraction
   Vec2<CoordType> operator-(void) const;          // unary -
   Vec2<CoordType> operator^(const Vec2 &v) const; // elementwise multiplication
   // elementwise multiplication with functio name instead of operator
   Vec2<CoordType> mult(const Vec2 &v) const;
   void operator+=(const Vec2 &v);                 // operator: +=
   void operator-=(const Vec2 &v);                 // operator: -=
   void operator*=(CoordType t);                   // operator: *= (scaling)
   CoordType operator*(const Vec2 &v) const;       // operator: dot product
   // dot product with function name instead of operator
   CoordType dot(const Vec2 &v) const;    
   Vec2<CoordType> lerp(CoordType a,const Vec2 &v) const; // returns a*this+(1-a)*v
   bool operator==(const Vec2 &v) const;    // equality
   bool operator!=(const Vec2 &v) const;    // inequality
   CoordType& operator[](unsigned short index); 
   const CoordType& operator[](unsigned short index) const; 
   void debugprint(void) const;// print coords
};

template<class CoordType>
class Vec3
{
 public:
   CoordType vec[3];
   // constructors and desctructor
   Vec3();   // empty constructor: coords are undefined!!!! =INF if debug
   Vec3(CoordType xc, CoordType yc, CoordType zc);
   Vec3(const Vec3 &v);

   ~Vec3() { }
   void set(const Vec3 &v);
   void set(CoordType xc, CoordType yc, CoordType zc);
   CoordType length(void) const;                   // length of vector
   CoordType length2(void) const;                  // squared length of vector
   // normalize vector (only if length>0.0), returns *this
   const Vec3<CoordType>& normalize(void);         
   CoordType normalizeIf(void);                    // normalize vector (only if length>0.0)
   void operator=(const Vec3 &v);                  // operator: assignment
   Vec3<CoordType> operator*(CoordType t) const;   // operator: scaling by scalar
   Vec3<CoordType> operator+(const Vec3 &v) const; // operator: addition 
   Vec3<CoordType> operator-(const Vec3 &v) const; // operator: subtraction
   Vec3<CoordType> operator-(void) const;          // unary -
   Vec3<CoordType> operator^(const Vec3 &v) const; // elementwise multiplication
   // elementwise multiplication with functio name instead of operator
   Vec3<CoordType> mult(const Vec3 &v) const;
   void operator+=(const Vec3 &v);                 // operator: +=
   void operator-=(const Vec3 &v);                 // operator: -=
   void operator*=(CoordType t);                   // operator: *= (scaling)
   CoordType operator*(const Vec3 &v) const;       // operator: dot product
   // dot product with function name instead of operator
   CoordType dot(const Vec3 &v) const;    
   Vec3<CoordType> operator%(const Vec3 &v) const; // operator: cross product
   // cross product with function name instead of operator
   Vec3<CoordType> cross(const Vec3 &v) const;     
   Vec3<CoordType> lerp(CoordType a,const Vec3 &v) const; // returns a*this+(1-a)*v
   bool operator==(const Vec3 &v) const;            // equality
   bool operator!=(const Vec3 &v) const;            // inequality
   // if index=0 then x, index=1 then y, index=2 then z
   CoordType& operator[](unsigned short index); 
   const CoordType& operator[](unsigned short index) const; 
   Vec3<CoordType> perpVector(void) const;          // create a vector that is perp to this
   unsigned int toRGBA(void) const;
   void debugprint(void) const;                     // print coords
};

// not finished yet...
template<class CoordType>
class Vec4
{
 public:
   CoordType vec[4];
   // constructors and desctructor
   Vec4();   // empty constructor: coords are undefined!!!! =INF if debug
   Vec4(CoordType xc, CoordType yc, CoordType zc, CoordType wc);
   Vec4(const Vec4 &v);
   ~Vec4() { }
   void set(const Vec4 &v);
   void set(CoordType xc, CoordType yc, CoordType zc, CoordType wc);
   //CoordType length(void) const;                   // length of vector
   //CoordType length2(void) const;                  // squared length of vector
   // normalize vector (only if length>0.0), returns *this
   const Vec4<CoordType>& normalize(void);         
   //CoordType normalizeIf(void);                    // normalize vector (only if length>0.0)
   void operator=(const Vec4 &v);                  // operator: assignment
   //Vec3<CoordType> operator*(CoordType t) const;   // operator: scaling by scalar
   //Vec3<CoordType> operator+(const Vec3 &v) const; // operator: addition 
   //Vec3<CoordType> operator-(const Vec3 &v) const; // operator: subtraction
   //Vec3<CoordType> operator-(void) const;          // unary -
   //Vec3<CoordType> operator^(const Vec3 &v) const; // elementwise multiplication
   // elementwise multiplication with functio name instead of operator
   //Vec3<CoordType> mult(const Vec3 &v) const;
   //void operator+=(const Vec3 &v);                 // operator: +=
   //void operator-=(const Vec3 &v);                 // operator: -=
   //void operator*=(CoordType t);                   // operator: *= (scaling)
   //CoordType operator*(const Vec3 &v) const;       // operator: dot product
   // dot product with function name instead of operator
   //CoordType dot(const Vec3 &v) const;    
   //Vec3<CoordType> operator%(const Vec3 &v) const; // operator: cross product
   // cross product with function name instead of operator
   //Vec3<CoordType> cross(const Vec3 &v) const;     
   //Vec3<CoordType> lerp(CoordType a,const Vec3 &v) const; // returns a*this+(1-a)*v
   //bool operator==(const Vec3 &v) const;            // equality
   //bool operator!=(const Vec3 &v) const;            // inequality
   // if index=0 then x, index=1 then y, index=2 then z
   CoordType& operator[](unsigned short index); 
   const CoordType& operator[](unsigned short index) const; 
   void debugprint(void) const;                     // print coords
};

// Mtx3 -- the 3x3 matrix class
//
// the element order of a matrix (done like this to facilitate OpenGL glLoadMatrix etc, 
// this is mostly important for 4x4 matrices
// ( m[0][0]  m[1][0]  m[2][0] )  
// ( m[0][1]  m[1][1]  m[2][1] )
// ( m[0][2]  m[1][2]  m[2][2] )
// 
// vectors are colum major, i.e. a matrix vector mult is: w=M*v, where w,v are vectors, M matrix
//

template<class CoordType>
class Mtx3
{
 public:
   union
   {
      CoordType mtx[3][3];
      CoordType array[3*3];
   };
   // constructors and desctructor
   Mtx3(); // empty constructor: matrix is undefined!!!! =INF if debug
   Mtx3(CoordType m00, CoordType m10, CoordType m20,
	CoordType m01, CoordType m11, CoordType m21,
	CoordType m02, CoordType m12, CoordType m22);
   Mtx3(const Mtx3 &m);
   ~Mtx3() { }
   void set(const Mtx3 &m);
   void set(CoordType m00, CoordType m10, CoordType m20,
	    CoordType m01, CoordType m11, CoordType m21,
	    CoordType m02, CoordType m12, CoordType m22);
   void set(int x,int y,CoordType m);
   void operator=(const Mtx3 &m);                  // operator: assignment
   // if index=0 then col vec 0, index=1 then col vec 1, else col vec 2
   Vec3<CoordType> operator[](unsigned short index) const;

   Mtx3<CoordType> operator+(const Mtx3 &m) const; // operator: addition 
   Mtx3<CoordType> operator-(const Mtx3 &m) const; // operator: subtraction
   Mtx3<CoordType> operator-(void) const;          // unary -
   Mtx3<CoordType> operator*(const Mtx3 &m) const; // operator: matrix matrix mult
   Vec3<CoordType> operator*(const Vec3<CoordType> &v) const; // operator: matrix vector mult
   void operator+=(const Mtx3 &m);                 // operator: +=
   void operator-=(const Mtx3 &m);                 // operator: -=
   void operator*=(const Mtx3 &m);                 // operator: *= 
   bool operator==(const Mtx3 &m) const;    // equality
   bool operator!=(const Mtx3 &m) const;    // inequality
   void loadIdentity(void);// load matrix with identity matrix
   void rotX(CoordType radians);// creates a rotation matrix, "radians" around X-axis
   void rotY(CoordType radians);// creates a rotation matrix, "radians" around Y-axis
   void rotZ(CoordType radians);// creates a rotation matrix, "radians" around Z-axis
   // creates rotation matrix, rotates "radians" around "axis", axis must be normalized
   void rotAxis(const Vec3<CoordType> &axis,CoordType radians);     
   // rotation matrix, from from-vector to to-vector (from, to must be normalized)
   void vecRotVec(Vec3<CoordType> &from,Vec3<CoordType> &to); 
   // creates a scaling matrix
   void scale(CoordType scaleX,CoordType scaleY,CoordType scaleZ); 
   void transpose(void);
   bool invert(void);   // returns true if invertible (and inverts), otherwise no action
   void invertOrtho(void);

   bool QLAlgorithm(CoordType afDiag[3], CoordType afSubDiag[3]);
   void tridiagonal(CoordType afDiag[3], CoordType afSubDiag[3]);
   void eigenSolveSymmetric(CoordType afEigenvalue[3],
			    Vec3<CoordType> akEigenvector[3]) const;

   void debugprint(void) const;   // print coords

//static const Mtx3<float> identity;           // could not get this to work
};

template<class CoordType>
class Mtx4
{
 public:
   union
   {
      CoordType mtx[4][4];
      CoordType array[4*4];
   };
   // constructors and desctructor
   Mtx4(); // empty constructor: matrix is undefined!!!! =INF if debug
   Mtx4(CoordType m00, CoordType m10, CoordType m20, CoordType m30,
	CoordType m01, CoordType m11, CoordType m21, CoordType m31,
	CoordType m02, CoordType m12, CoordType m22, CoordType m32,
	CoordType m03, CoordType m13, CoordType m23, CoordType m33);
   Mtx4(const Mtx4 &m);
   ~Mtx4() { }
   void set(const Mtx4 &m);
   void set(CoordType m00, CoordType m10, CoordType m20, CoordType m30,
	    CoordType m01, CoordType m11, CoordType m21, CoordType m31,
	    CoordType m02, CoordType m12, CoordType m22, CoordType m32,
	    CoordType m03, CoordType m13, CoordType m23, CoordType m33);
   void operator=(const Mtx4 &m);                  // operator: assignment
   Mtx4<CoordType> operator+(const Mtx4 &m) const; // operator: addition 
   Mtx4<CoordType> operator-(const Mtx4 &m) const; // operator: subtraction
   Mtx4<CoordType> operator-(void) const;          // unary -
   Mtx4<CoordType> operator*(const Mtx4 &m) const; // operator: matrix matrix mult
   void operator+=(const Mtx4 &m);                 // operator: +=
   void operator-=(const Mtx4 &m);                 // operator: -=
   void operator*=(const Mtx4 &m);                 // operator: *= 
   bool operator==(const Mtx4 &m) const;    // equality
   bool operator!=(const Mtx4 &m) const;    // inequality
   Vec3<CoordType> multPnt(const Vec3<CoordType> &v) const; // operator: matrix point mult
   Vec3<CoordType> multVec(const Vec3<CoordType> &v) const; // operator: matrix vector mult
   Vec3<CoordType> multDivW(const Vec3<CoordType> &v, CoordType vectorW) const; // operator: "matrix point with translation and division of W afterwards" mult
   void loadIdentity(void);// load matrix with identity matrix
   void rotX(CoordType radians);// creates a rotation matrix, "radians" around X-axis
   void rotY(CoordType radians);// creates a rotation matrix, "radians" around Y-axis
   void rotZ(CoordType radians);// creates a rotation matrix, "radians" around Z-axis
   void rotAxis(Vec3<CoordType> &axis,CoordType radians);     // creates rotation matrix, rotates "radians" around "axis", axis must be normalized
   void vecRotVec(Vec3<CoordType> &from,Vec3<CoordType> &to); // rotation matrix, from from-vector to to-vector (from, to must be normalized)
   void billboardMatrix(Vec3<CoordType> &fromView, Vec3<CoordType> &fromUp,Vec3<CoordType> &toView,Vec3<CoordType> toUp);
   void translate(CoordType tx, CoordType ty, CoordType tz);
   void scale(CoordType scaleX,CoordType scaleY,CoordType scaleZ); // creates a scaling matrix
   void transpose(void);
   bool invert(void);// returns true if invertible (and inverts), otherwise no action
   void invertOrtho(void);// simply the transpose of the upper left 3x3 matrix
   void invertOrthoTrans(void);// inverse of orthogonal 3x3 plus translation
   void debugprint(void) const;// print coords
   
//static const Mtx4<float> identity;           // could not get this to work
};







#define SWAP(A,B) {CoordType c=A; A=B; B=c;}


///////////////////////////////////////////////////////
//              Inline functions                     //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
//                Vec2 functions                     //
///////////////////////////////////////////////////////

template<class CoordType>
inline Vec2<CoordType>::Vec2()
{
#if _DEBUG
//   x=y=numeric_limits<CoordType>::infinity();
#endif
}


template<class CoordType>
inline Vec2<CoordType>::Vec2(CoordType xc, CoordType yc)
{
   vec[0]=xc;
   vec[1]=yc;
}

template<class CoordType>
inline Vec2<CoordType>::Vec2(const Vec2 &v)   
{
   vec[0]=v[0];
   vec[1]=v[1];
}

template<class CoordType>
inline void Vec2<CoordType>::set(const Vec2 &v)
{
   vec[0]=v[0];
   vec[1]=v[0];
}

template<class CoordType>
inline void Vec2<CoordType>::set(CoordType xc,CoordType yc)
{
   vec[0]=xc;
   vec[1]=yc;
}

// length of vector
template<class CoordType>
inline CoordType Vec2<CoordType>::length(void) const       
{
   return (CoordType)sqrt(vec[0]*vec[0]+vec[1]*vec[1]);
}

// squared length of vector
template<class CoordType>
inline CoordType Vec2<CoordType>::length2(void) const       
{
   return vec[0]*vec[0]+vec[1]*vec[1];
}

// normalize vector if length>0.0
template<class CoordType>
inline const Vec2<CoordType>& Vec2<CoordType>::normalize(void)
{
   CoordType len2=vec[0]*vec[0]+vec[1]*vec[1]; 
/*   assert(len2>numeric_limits<CoordType>::epsilon()); */
/*   if(len2>numeric_limits<CoordType>::epsilon()) */
   if(len2>0.0000001)  /* fix this */
   {
      CoordType invlen=(CoordType)(1.0/sqrt(len2));
      vec[0]*=invlen;
      vec[1]*=invlen;
   }
   return *this;
}

// normalize vector if length>0.0, returns length
template<class CoordType>
inline CoordType Vec2<CoordType>::normalizeIf(void)        
{
   CoordType len2=vec[0]*vec[0]+vec[1]*vec[1];
/*   if(len2>numeric_limits<CoordType>::epsilon()) */
   if(len2>0.0000001)  /* fix this */
   {
      CoordType len=(CoordType)sqrt(len2);
      CoordType invlen=1.0/len;
      vec[0]*=invlen;
      vec[1]*=invlen;
      return len;
   }
   else
   {
      return 0.0;
   }
}

// operator: assignment
template<class CoordType>
inline void Vec2<CoordType>::operator=(const Vec2 &v)   
{
   vec[0]=v[0];
   vec[1]=v[1];
}

// operator: scaling by scalar
template<class CoordType>
inline Vec2<CoordType> Vec2<CoordType>::operator*(CoordType t) const  
{
   return Vec2<CoordType>(t*vec[0], t*vec[1]);
}

// operator: addition
template<class CoordType>
inline Vec2<CoordType> Vec2<CoordType>::operator+(const Vec2 &v) const  
{
   return Vec2<CoordType>(vec[0]+v[0], vec[1]+v[1]);
}

// operator: subtraction
template<class CoordType>
inline Vec2<CoordType> Vec2<CoordType>::operator-(const Vec2 &v) const  
{
   return Vec2<CoordType>(vec[0]-v[0], vec[1]-v[1]);
}	

// operator: unary -
template<class CoordType>
inline Vec2<CoordType> Vec2<CoordType>::operator-(void) const  
{
   return Vec2<CoordType>(-vec[0], -vec[1]);
}

// operator: elementwise multiplication
template<class CoordType>
inline Vec2<CoordType> Vec2<CoordType>::operator^(const Vec2 &v) const  
{
   return Vec2<CoordType>(vec[0]*v[0], vec[1]*v[1]);
}

// elementwise multiplication, same as ^ operator
template<class CoordType>
inline Vec2<CoordType> Vec2<CoordType>::mult(const Vec2 &v) const  
{
   return Vec2<CoordType>(vec[0]*v[0], vec[1]*v[1]);
}

// operator: +=
template<class CoordType>
inline void Vec2<CoordType>::operator+=(const Vec2 &v) 
{
   vec[0]+=v[0];
   vec[1]+=v[1];
}

// operator: -=
template<class CoordType>
inline void Vec2<CoordType>::operator-=(const Vec2 &v) 
{
   vec[0]-=v[0];
   vec[1]-=v[1];
}

// operator: *=
template<class CoordType>
inline void Vec2<CoordType>::operator*=(CoordType t) 
{
   vec[0]*=t;
   vec[1]*=t;
}

// operator: dot product
template<class CoordType>
inline CoordType Vec2<CoordType>::operator*(const Vec2 &v) const 
{
   return vec[0]*v[0]+vec[1]*v[1];
}

// dot product (same as operator *)
template<class CoordType>
inline CoordType Vec2<CoordType>::dot(const Vec2 &v) const 
{
   return vec[0]*v[0]+vec[1]*v[1];
}

// returns a*this+(1-a)*v LERP=Linear intERPolation
template<class CoordType>
inline Vec2<CoordType> Vec2<CoordType>::lerp(CoordType a,const Vec2 &v) const 
{
   return Vec2<CoordType>(a*vec[0]+(1.0-a)*v[0], a*vec[1]+(1.0-a)*v[1]);
}

// equality
template<class CoordType>
inline bool Vec2<CoordType>::operator==(const Vec2 &v) const
{
   return vec[0]==v[0] && vec[1]==v[1];
}

// inequality
template<class CoordType>
inline bool Vec2<CoordType>::operator!=(const Vec2 &v) const
{
   return vec[0]!=v[0] || vec[1]!=v[1];
}

// if index=0 then x, index=1 then y
template<class CoordType>
inline CoordType& Vec2<CoordType>::operator[](unsigned short index)
{
   assert(index<2);
   return vec[index];
}

// if index=0 then x, index=1 then y
template<class CoordType>
inline const CoordType& Vec2<CoordType>::operator[](unsigned short index) const
{
   assert(index<2);
   return vec[index];
}

// print coords
template<class CoordType>
inline void Vec2<CoordType>::debugprint(void) const 
{
   fprintf( stderr, "(%2.4f, %2.4f)\n", (float)vec[0], (float)vec[1] );
}


///////////////////////////////////////////////////////
//                Vec3 functions                     //
///////////////////////////////////////////////////////
template<class CoordType>
inline Vec3<CoordType>::Vec3()
{
//#if _DEBUG
/*x=y=z=numeric_limits<CoordType>::infinity(); */
//#endif
}


template<class CoordType>
inline Vec3<CoordType>::Vec3(CoordType xc, CoordType yc, CoordType zc)
{
   vec[0]=xc;
   vec[1]=yc;
   vec[2]=zc;
}


template<class CoordType>
inline Vec3<CoordType>::Vec3(const Vec3 &v)   
{
   vec[0]=v[0];
   vec[1]=v[1];
   vec[2]=v[2];
}

template<class CoordType>
inline void Vec3<CoordType>::set(const Vec3 &v)
{
   vec[0]=v[0];
   vec[1]=v[1];
   vec[2]=v[2];
}

template<class CoordType>
inline void Vec3<CoordType>::set(CoordType xc,CoordType yc,CoordType zc)
{
  vec[0]=xc;
  vec[1]=yc;
  vec[2]=zc;
}

template<class CoordType>
inline CoordType Vec3<CoordType>::length(void) const       // length of vector
{
  return (CoordType)sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
}

template<class CoordType>
inline CoordType Vec3<CoordType>::length2(void) const       // squared length of vector
{
  return vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2];
}

template<class CoordType>
inline const Vec3<CoordType>& Vec3<CoordType>::normalize(void)        // normalize vector if length>0.0
{
  CoordType len2=vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2];
  /*assert(len2>numeric_limits<CoordType>::epsilon()); */
  /*if(len2>numeric_limits<CoordType>::epsilon()) */
  if(len2>0.00000000000001)
  {
    CoordType invlen=(CoordType)(1.0/sqrt(len2));
    vec[0]*=invlen;
    vec[1]*=invlen;
    vec[2]*=invlen;
  }
  return *this;
}

template<class CoordType>
inline CoordType Vec3<CoordType>::normalizeIf(void)        // normalize vector if length>0.0, returns length
{
   CoordType len2=vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2];
   /*if(len2>numeric_limits<CoordType>::epsilon()) */
   if(len2>0.0000001)
   {
     CoordType len=(CoordType)sqrt(len2);
     CoordType invlen=1.0/len;
     vec[0]*=invlen;
     vec[1]*=invlen;
     vec[2]*=invlen;
     return len;
   }
   else
   {
     return 0.0;
   }
}

template<class CoordType>
inline void Vec3<CoordType>::operator=(const Vec3 &v)   // operator: assignment
{
  vec[0]=v[0];
  vec[1]=v[1];
  vec[2]=v[2];
}


template<class CoordType>
inline Vec3<CoordType> Vec3<CoordType>::operator*(CoordType t) const  // operator: scaling by scalar
{
  return Vec3<CoordType>(t*vec[0], t*vec[1], t*vec[2]);
}

template<class CoordType>
inline Vec3<CoordType> Vec3<CoordType>::operator+(const Vec3 &v) const  // operator: addition
{
  return Vec3<CoordType>(vec[0]+v[0], vec[1]+v[1], vec[2]+v[2]);
}

template<class CoordType>
inline Vec3<CoordType> Vec3<CoordType>::operator-(const Vec3 &v) const  // operator: subtraction
{
  return Vec3<CoordType>(vec[0]-v[0], vec[1]-v[1], vec[2]-v[2]);
}

template<class CoordType>
inline Vec3<CoordType> Vec3<CoordType>::operator-(void) const  // operator: unary -
{
  return Vec3<CoordType>(-vec[0], -vec[1], -vec[2]);
}

template<class CoordType>
inline Vec3<CoordType> Vec3<CoordType>::operator^(const Vec3 &v) const  // operator: elementwise multiplication
{
  return Vec3<CoordType>(vec[0]*v[0], vec[1]*v[1], vec[2]*v[2]);
}

template<class CoordType>
inline Vec3<CoordType> Vec3<CoordType>::mult(const Vec3 &v) const  // elementwise multiplication, same as ^ operator
{
  return Vec3<CoordType>(vec[0]*v[0], vec[1]*v[1], vec[2]*v[2]);
}

template<class CoordType>
inline void Vec3<CoordType>::operator+=(const Vec3 &v) // operator: +=
{
  vec[0]+=v[0];
  vec[1]+=v[1];
  vec[2]+=v[2];
}

template<class CoordType>
inline void Vec3<CoordType>::operator-=(const Vec3 &v) // operator: -=
{
  vec[0]-=v[0];
  vec[1]-=v[1];
  vec[2]-=v[2];
}

template<class CoordType>
inline void Vec3<CoordType>::operator*=(CoordType t) // operator: *=
{
  vec[0]*=t;
  vec[1]*=t;
  vec[2]*=t;
}

template<class CoordType>
inline CoordType Vec3<CoordType>::operator*(const Vec3 &v) const // operator: dot product
{
  return vec[0]*v[0]+vec[1]*v[1]+vec[2]*v[2];
}

template<class CoordType>
inline CoordType Vec3<CoordType>::dot(const Vec3 &v) const // dot product (same as operator *)
{
  return vec[0]*v[0]+vec[1]*v[1]+vec[2]*v[2];
}

template<class CoordType>
inline Vec3<CoordType> Vec3<CoordType>::operator%(const Vec3 &v) const // operator: cross product
{
  return Vec3(vec[1]*v[2]-vec[2]*v[1], vec[2]*v[0]-vec[0]*v[2], vec[0]*v[1]-vec[1]*v[0]);
}

template<class CoordType>
inline Vec3<CoordType> Vec3<CoordType>::cross(const Vec3 &v) const // cross product (same as operator %)
{
  return Vec3(vec[1]*v[2]-vec[2]*v[1], vec[2]*v[0]-vec[0]*v[2], vec[0]*v[1]-vec[1]*v[0]);
}

// returns a*this+(1-a)*v LERP=Linear intERPolation
template<class CoordType>
inline Vec3<CoordType> Vec3<CoordType>::lerp(CoordType a,const Vec3 &v) const 
{
  return Vec3(a*vec[0]+(1.0-a)*v.vec[0], a*vec[1]+(1.0-a)*v[1], a*vec[2]+(1.0-a)*v[2]);
}

// equality
template<class CoordType>
inline bool Vec3<CoordType>::operator==(const Vec3 &v) const 
{
  //return vec[0]==v[0] && vec[1]==v[1] && vec[2]==v[2];
	const CoordType epsilon = (CoordType) 0.00001;
	bool isEqual;
	isEqual = ( (CoordType) fabs(vec[0]-v[0]) < epsilon );
	isEqual &= ( (CoordType) fabs(vec[1]-v[1]) < epsilon );
	isEqual &= ( (CoordType) fabs(vec[2]-v[2]) < epsilon );
	return isEqual;
}

// inequality
template<class CoordType>
inline bool Vec3<CoordType>::operator!=(const Vec3 &v) const 
{
  return vec[0]!=v[0] || vec[1]!=v[1] || vec[2]!=v[2];
}

// if index=0 then x, index=1 then y, index=2 then z
template<class CoordType>
inline CoordType& Vec3<CoordType>::operator[](unsigned short index)
{
   assert(index<3);
   return vec[index];
}

// if index=0 then x, index=1 then y, index=2 then z
template<class CoordType>
inline const CoordType& Vec3<CoordType>::operator[](unsigned short index) const
{
   assert(index<3);
   return vec[index];
}

// create a vector that is perp to this
template<class CoordType>
inline Vec3<CoordType> Vec3<CoordType>::perpVector(void) const
{
   // taken from Hughes and Moller JGT paper
   Vec3<CoordType> v;
   CoordType x,y,z;
   x=(CoordType) fabs(vec[X]);
   y=(CoordType)fabs(vec[Y]);
   z=(CoordType)fabs(vec[Z]);
   
   if(x<y)
   {
      if(x<z) v.set(0.0, -vec[Z],vec[Y]);
      else v.set(-vec[Y],vec[X],0.0);
   }
   else
   {
      if(y<z) v.set(-vec[Z],0.0,vec[X]);
      else v.set(-vec[Y],vec[X],0.0);
   }

   v.normalize();
   return v;
}

// convert from three floats to one unsigned int: [RGBA]
template<class CoordType>
inline unsigned int  Vec3<CoordType>::toRGBA(void) const
{
   unsigned int r,g,b;
   // clamp color to [0,1]  (we do not check for <0.0 because it should not be)
   r = vec[R]>1.0 ? 255 : (unsigned int) (255.0*vec[R]);
   g = vec[G]>1.0 ? 255 : (unsigned int) (255.0*vec[G]);
   b = vec[B]>1.0 ? 255 : (unsigned int) (255.0*vec[B]);
   return (r<<24) | (g<<16) | (b<<8);  // Alpha=0
}


// print coords
template<class CoordType>
inline void Vec3<CoordType>::debugprint(void) const 
{
  fprintf( stderr, "(%2.4f, %2.4f, %2.4f)\n", (float)vec[0], (float)vec[1], (float)vec[2] );
}


///////////////////////////////////////////////////////
//                Vec4 functions                     //
///////////////////////////////////////////////////////
// not complete

template<class CoordType>
inline Vec4<CoordType>::Vec4()
{
//#if _DEBUG
/*x=y=z=w=numeric_limits<CoordType>::infinity(); */
//#endif
}

template<class CoordType>
inline Vec4<CoordType>::Vec4(CoordType xc, CoordType yc, CoordType zc, CoordType wc)
{
   vec[0]=xc;
   vec[1]=yc;
   vec[2]=zc;
   vec[3]=wc;
}

template<class CoordType>
inline Vec4<CoordType>::Vec4(const Vec4 &v)
{
   vec[0]=v[0];
   vec[1]=v[1];
   vec[2]=v[2];
   vec[3]=v[3];
}

template<class CoordType>
inline void Vec4<CoordType>::set(const Vec4 &v)
{
   vec[0]=v[0];
   vec[1]=v[1];
   vec[2]=v[2];
   vec[3]=v[3];
}

template<class CoordType>
inline void Vec4<CoordType>::set(CoordType xc, CoordType yc, CoordType zc, CoordType wc)
{
   vec[0]=xc;
   vec[1]=yc;
   vec[2]=zc;
   vec[3]=wc;   
}

template<class CoordType>
inline void Vec4<CoordType>::operator=(const Vec4 &v)
{
   vec[0]=v[0];
   vec[1]=v[1];
   vec[2]=v[2];
   vec[3]=v[3];
}

template<class CoordType>
inline CoordType& Vec4<CoordType>::operator[](unsigned short index)
{
   assert(index<4);
   return vec[index];  
}

template<class CoordType>
inline const CoordType& Vec4<CoordType>::operator[](unsigned short index) const
{
   assert(index<4);
   return vec[index];  
}

// normalize vector if length>0.0
template<class CoordType>
inline const Vec4<CoordType>& Vec4<CoordType>::normalize(void)
{
   CoordType len2=vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]+vec[3]*vec[3]; 
/*   assert(len2>numeric_limits<CoordType>::epsilon()); */
/*   if(len2>numeric_limits<CoordType>::epsilon()) */
   if(len2>0.0000001)  /* fix this */
   {
      CoordType invlen=(CoordType)(1.0/sqrt(len2));
      vec[0]*=invlen;
      vec[1]*=invlen;
      vec[2]*=invlen;
      vec[3]*=invlen;
   }
   return *this;
}

template<class CoordType>
inline void Vec4<CoordType>::debugprint(void) const
{
   fprintf( stderr, "(%2.4f, %2.4f, %2.4f, %2.4f)\n",
	  (float)vec[0], (float)vec[1], (float)vec[2],(float)vec[3] );
}


///////////////////////////////////////////////////////
//                Mtx3 functions                     //
///////////////////////////////////////////////////////

template<class CoordType>
inline Mtx3<CoordType>::Mtx3()  // if debug is on, then set to infinity!
{
#if _DEBUG
/*mtx[0][0]=mtx[1][0]=mtx[2][0]=numeric_limits<CoordType>::infinity(); */
/*mtx[0][1]=mtx[1][1]=mtx[2][1]=numeric_limits<CoordType>::infinity(); */
/*mtx[0][2]=mtx[1][2]=mtx[2][2]=numeric_limits<CoordType>::infinity(); */
#endif
}

template<class CoordType>
inline Mtx3<CoordType>::Mtx3(CoordType m00, CoordType m10, CoordType m20,
			     CoordType m01, CoordType m11, CoordType m21,
			     CoordType m02, CoordType m12, CoordType m22)
{
   mtx[0][0]=m00; mtx[1][0]=m10; mtx[2][0]=m20;
   mtx[0][1]=m01; mtx[1][1]=m11; mtx[2][1]=m21;
   mtx[0][2]=m02; mtx[1][2]=m12; mtx[2][2]=m22;
}

template<class CoordType>
inline Mtx3<CoordType>::Mtx3(const Mtx3 &m)
{
   mtx[0][0]=m.mtx[0][0]; mtx[1][0]=m.mtx[1][0]; mtx[2][0]=m.mtx[2][0];
   mtx[0][1]=m.mtx[0][1]; mtx[1][1]=m.mtx[1][1]; mtx[2][1]=m.mtx[2][1];
   mtx[0][2]=m.mtx[0][2]; mtx[1][2]=m.mtx[1][2]; mtx[2][2]=m.mtx[2][2];
}

template<class CoordType>
inline void Mtx3<CoordType>::set(const Mtx3 &m)
{
   mtx[0][0]=m.mtx[0][0]; mtx[1][0]=m.mtx[1][0]; mtx[2][0]=m.mtx[2][0];
   mtx[0][1]=m.mtx[0][1]; mtx[1][1]=m.mtx[1][1]; mtx[2][1]=m.mtx[2][1];
   mtx[0][2]=m.mtx[0][2]; mtx[1][2]=m.mtx[1][2]; mtx[2][2]=m.mtx[2][2];
}

template<class CoordType>
inline void Mtx3<CoordType>::set(CoordType m00, CoordType m10, CoordType m20,
				 CoordType m01, CoordType m11, CoordType m21,
				 CoordType m02, CoordType m12, CoordType m22)
{
   mtx[0][0]=m00; mtx[1][0]=m10; mtx[2][0]=m20;
   mtx[0][1]=m01; mtx[1][1]=m11; mtx[2][1]=m21;
   mtx[0][2]=m02; mtx[1][2]=m12; mtx[2][2]=m22;
}

template<class CoordType>
inline void Mtx3<CoordType>::set(int x,int y,CoordType m)
{
   assert(x>=0 && x<3 && y>=0 && y<3);
   mtx[x][y]=m;
}


template<class CoordType>
inline void Mtx3<CoordType>::operator=(const Mtx3 &m)  // operator: assignment
{
   mtx[0][0]=m.mtx[0][0]; mtx[1][0]=m.mtx[1][0]; mtx[2][0]=m.mtx[2][0];
   mtx[0][1]=m.mtx[0][1]; mtx[1][1]=m.mtx[1][1]; mtx[2][1]=m.mtx[2][1];
   mtx[0][2]=m.mtx[0][2]; mtx[1][2]=m.mtx[1][2]; mtx[2][2]=m.mtx[2][2];
}


// if index=0 then col vec 0, index=1 then col vec 1, if index=2 then col vec 2
template<class CoordType>
inline Vec3<CoordType> Mtx3<CoordType>::operator[](unsigned short index) const
{
   assert(index<3);
   return Vec3<CoordType>(mtx[index][0],mtx[index][1],mtx[index][2]);
}

template<class CoordType>
inline Mtx3<CoordType> Mtx3<CoordType>::operator+(const Mtx3 &m) const // matrix addition
{
   return Mtx3(mtx[0][0]+m.mtx[0][0],mtx[1][0]+m.mtx[1][0],mtx[2][0]+m.mtx[2][0],
	       mtx[0][1]+m.mtx[0][1],mtx[1][1]+m.mtx[1][1],mtx[2][1]+m.mtx[2][1],
	       mtx[0][2]+m.mtx[0][2],mtx[1][2]+m.mtx[1][2],mtx[2][2]+m.mtx[2][2]);
}

template<class CoordType>
inline Mtx3<CoordType> Mtx3<CoordType>::operator-(const Mtx3 &m) const // matrix subtraction
{
   return Mtx3(mtx[0][0]-m.mtx[0][0],mtx[1][0]-m.mtx[1][0],mtx[2][0]-m.mtx[2][0],
	       mtx[0][1]-m.mtx[0][1],mtx[1][1]-m.mtx[1][1],mtx[2][1]-m.mtx[2][1],
	       mtx[0][2]-m.mtx[0][2],mtx[1][2]-m.mtx[1][2],mtx[2][2]-m.mtx[2][2]);
}

template<class CoordType>
inline Mtx3<CoordType> Mtx3<CoordType>::operator-(void) const // unary -
{
   return Mtx3(-mtx[0][0],-mtx[1][0],-mtx[2][0],
	       -mtx[0][1],-mtx[1][1],-mtx[2][1],
	       -mtx[0][2],-mtx[1][2],-mtx[2][2]);
}

// matrix matrix multiplication
template<class CoordType>
inline Mtx3<CoordType> Mtx3<CoordType>::operator*(const Mtx3 &a) const 
{
   return Mtx3(mtx[0][0]*a.mtx[0][0]+mtx[1][0]*a.mtx[0][1]+mtx[2][0]*a.mtx[0][2],
	       mtx[0][0]*a.mtx[1][0]+mtx[1][0]*a.mtx[1][1]+mtx[2][0]*a.mtx[1][2],
	       mtx[0][0]*a.mtx[2][0]+mtx[1][0]*a.mtx[2][1]+mtx[2][0]*a.mtx[2][2],
	       
	       mtx[0][1]*a.mtx[0][0]+mtx[1][1]*a.mtx[0][1]+mtx[2][1]*a.mtx[0][2],
	       mtx[0][1]*a.mtx[1][0]+mtx[1][1]*a.mtx[1][1]+mtx[2][1]*a.mtx[1][2],
	       mtx[0][1]*a.mtx[2][0]+mtx[1][1]*a.mtx[2][1]+mtx[2][1]*a.mtx[2][2],
	       
	       mtx[0][2]*a.mtx[0][0]+mtx[1][2]*a.mtx[0][1]+mtx[2][2]*a.mtx[0][2],
	       mtx[0][2]*a.mtx[1][0]+mtx[1][2]*a.mtx[1][1]+mtx[2][2]*a.mtx[1][2],
	       mtx[0][2]*a.mtx[2][0]+mtx[1][2]*a.mtx[2][1]+mtx[2][2]*a.mtx[2][2]);
}

// operator: matrix vector mult
template<class CoordType>
inline Vec3<CoordType> Mtx3<CoordType>::operator*(const Vec3<CoordType> &v) const
{
   return Vec3<CoordType>(mtx[0][0]*v[X] + mtx[1][0]*v[Y] + mtx[2][0]*v[Z],
			  mtx[0][1]*v[X] + mtx[1][1]*v[Y] + mtx[2][1]*v[Z],
			  mtx[0][2]*v[X] + mtx[1][2]*v[Y] + mtx[2][2]*v[Z]);
}


template<class CoordType>
inline void Mtx3<CoordType>::operator+=(const Mtx3 &a) // operator +=
{
   mtx[0][0]+=a.mtx[0][0]; mtx[1][0]+=a.mtx[1][0]; mtx[2][0]+=a.mtx[2][0];
   mtx[0][1]+=a.mtx[0][1]; mtx[1][1]+=a.mtx[1][1]; mtx[2][1]+=a.mtx[2][1];
   mtx[0][2]+=a.mtx[0][2]; mtx[1][2]+=a.mtx[1][2]; mtx[2][2]+=a.mtx[2][2];
}

template<class CoordType>
inline void Mtx3<CoordType>::operator-=(const Mtx3 &a) // operator -=
{
   mtx[0][0]-=a.mtx[0][0]; mtx[1][0]-=a.mtx[1][0]; mtx[2][0]-=a.mtx[2][0];
   mtx[0][1]-=a.mtx[0][1]; mtx[1][1]-=a.mtx[1][1]; mtx[2][1]-=a.mtx[2][1];
   mtx[0][2]-=a.mtx[0][2]; mtx[1][2]-=a.mtx[1][2]; mtx[2][2]-=a.mtx[2][2];
}

template<class CoordType>
inline void Mtx3<CoordType>::operator*=(const Mtx3 &a) // operator *=
{
   CoordType x,y,z;
   x=mtx[0][0]*a.mtx[0][0]+mtx[1][0]*a.mtx[0][1]+mtx[2][0]*a.mtx[0][2];
   y=mtx[0][0]*a.mtx[1][0]+mtx[1][0]*a.mtx[1][1]+mtx[2][0]*a.mtx[1][2];
   z=mtx[0][0]*a.mtx[2][0]+mtx[1][0]*a.mtx[2][1]+mtx[2][0]*a.mtx[2][2];
   mtx[0][0]=x; mtx[1][0]=y; mtx[2][0]=z;
   
   x=mtx[0][1]*a.mtx[0][0]+mtx[1][1]*a.mtx[0][1]+mtx[2][1]*a.mtx[0][2];
   y=mtx[0][1]*a.mtx[1][0]+mtx[1][1]*a.mtx[1][1]+mtx[2][1]*a.mtx[1][2];
   z=mtx[0][1]*a.mtx[2][0]+mtx[1][1]*a.mtx[2][1]+mtx[2][1]*a.mtx[2][2];
   mtx[0][1]=x; mtx[1][1]=y; mtx[2][1]=z;
   
   x=mtx[0][2]*a.mtx[0][0]+mtx[1][2]*a.mtx[0][1]+mtx[2][2]*a.mtx[0][2];
   y=mtx[0][2]*a.mtx[1][0]+mtx[1][2]*a.mtx[1][1]+mtx[2][2]*a.mtx[1][2];
   z=mtx[0][2]*a.mtx[2][0]+mtx[1][2]*a.mtx[2][1]+mtx[2][2]*a.mtx[2][2];
   mtx[0][2]=x; mtx[1][2]=y; mtx[2][2]=z;
}

template<class CoordType>
inline bool Mtx3<CoordType>::operator==(const Mtx3 &a) const // operator ==
{
   return mtx[0][0]==a.mtx[0][0] && mtx[1][0]==a.mtx[1][0] && mtx[2][0]==a.mtx[2][0] &&
      mtx[0][1]==a.mtx[0][1] && mtx[1][1]==a.mtx[1][1] && mtx[2][1]==a.mtx[2][1] &&
      mtx[0][2]==a.mtx[0][2] && mtx[1][2]==a.mtx[1][2] && mtx[2][2]==a.mtx[2][2];
}

template<class CoordType>
inline bool Mtx3<CoordType>::operator!=(const Mtx3 &a) const // operator !=
{
   return mtx[0][0]!=a.mtx[0][0] || mtx[1][0]!=a.mtx[1][0] || mtx[2][0]!=a.mtx[2][0] ||
      mtx[0][1]!=a.mtx[0][1] || mtx[1][1]!=a.mtx[1][1] || mtx[2][1]!=a.mtx[2][1] ||
      mtx[0][2]!=a.mtx[0][2] || mtx[1][2]!=a.mtx[1][2] || mtx[2][2]!=a.mtx[2][2];
}

// load matrix with identity matrix
template<class CoordType>
inline void Mtx3<CoordType>::loadIdentity(void) 
{
   mtx[0][0]=1.0; mtx[1][0]=0.0; mtx[2][0]=0.0;
   mtx[0][1]=0.0; mtx[1][1]=1.0; mtx[2][1]=0.0;
   mtx[0][2]=0.0; mtx[1][2]=0.0; mtx[2][2]=1.0;
}

// creates a rotation matrix, "radians" around X-axis
template<class CoordType>
inline void Mtx3<CoordType>::rotX(CoordType radians) 
{
   CoordType c,s;
   c=(CoordType)cos(radians);
   s=(CoordType)sin(radians);
   mtx[0][0]=1.0; mtx[1][0]=0.0; mtx[2][0]=0.0;
   mtx[0][1]=0.0; mtx[1][1]=c;   mtx[2][1]=-s;
   mtx[0][2]=0.0; mtx[1][2]=s;   mtx[2][2]=c;
}

// creates a rotation matrix, "radians" around Y-axis
template<class CoordType>
inline void Mtx3<CoordType>::rotY(CoordType radians) 
{
   CoordType c,s;
   c=(CoordType)cos(radians);
   s=(CoordType)sin(radians);
   mtx[0][0]=c;   mtx[1][0]=0.0; mtx[2][0]=s;
   mtx[0][1]=0.0; mtx[1][1]=1.0; mtx[2][1]=0.0;
   mtx[0][2]=-s;  mtx[1][2]=0.0; mtx[2][2]=c;
}

// creates a rotation matrix, "radians" around Z-axis
template<class CoordType>
inline void Mtx3<CoordType>::rotZ(CoordType radians) 
{
   CoordType c,s;
   c=(CoordType)cos(radians);
   s=(CoordType)sin(radians);
   mtx[0][0]=c;   mtx[1][0]=-s;  mtx[2][0]=0.0;
   mtx[0][1]=s;   mtx[1][1]=c;   mtx[2][1]=0.0;
   mtx[0][2]=0.0; mtx[1][2]=0.0; mtx[2][2]=1.0;
}

// creates a scaling matrix
template<class CoordType>
inline void Mtx3<CoordType>::scale(CoordType scaleX,CoordType scaleY,CoordType scaleZ) 
{
   mtx[0][0]=scaleX; mtx[1][0]=0.0;    mtx[2][0]=0.0;
   mtx[0][1]=0.0;    mtx[1][1]=scaleY; mtx[2][1]=0.0;
   mtx[0][2]=0.0;    mtx[1][2]=0.0;    mtx[2][2]=scaleZ;
}

// creates rotation matrix, rotates "radians" around "axis", axis must be normalized
template<class CoordType>
inline void Mtx3<CoordType>::rotAxis(const Vec3<CoordType> &axis,CoordType radians)
{
   // from Ken Shoemake's gem: uses a quaternion to construct the matrix
   radians*=0.5;
   CoordType sinalpha=(CoordType)sin(radians);
   CoordType cosalpha=(CoordType)cos(radians);
   
   // create the quaternion
   CoordType q[4]={axis[X]*sinalpha, axis[Y]*sinalpha, axis[Z]*sinalpha, cosalpha};

   CoordType Nq=q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
   CoordType s=(Nq > 0.0f) ? (2.0f / Nq) : 0.0f;
   CoordType xs=q[0]*s,  ys=q[1]*s,  zs=q[2]*s;
   CoordType wx=q[3]*xs, wy=q[3]*ys, wz=q[3]*zs;
   CoordType xx=q[0]*xs, xy=q[0]*ys, xz=q[0]*zs;
   CoordType yy=q[1]*ys, yz=q[1]*zs, zz=q[2]*zs;
   
   mtx[0][0]=1.0f - (yy + zz); mtx[1][0]=xy - wz;          mtx[2][0]=xz + wy;
   mtx[0][1]=xy + wz;          mtx[1][1]=1.0f - (xx + zz); mtx[2][1]=yz - wx;
   mtx[0][2]=xz - wy;          mtx[1][2]=yz + wx;          mtx[2][2]=1.0f - (xx + yy);
}

// rotation matrix, from from-vector to to-vector (from, to must be normalized)
template<class CoordType>
inline void Mtx3<CoordType>::vecRotVec(Vec3<CoordType> &from,Vec3<CoordType> &to)
{
   // see chapter Transforms in Real-Time Rendering ;-)
   Vec3<CoordType> v;
   CoordType e,h;
   v=from%to;  // v = from x to (cross product)
   e=from*to;  // e = from . to (dot product)
/*   if(e>1.0-numeric_limits<CoordType>::epsilon()) // "from" almost or equal to "to"-vector? */
   if(e>0.99999999)
   {
      loadIdentity();  // return identity
   }
/*   else if(e<-1.0+numeric_limits<CoordType>::epsilon())  // from almost or equal to negated "to"-vector? */
   else if(e<-0.99999999)
   {
      Vec3<CoordType> up,left;
      left.set(0.0,from[Z],-from[Y]);
/*      if(left*left<numeric_limits<CoordType>::epsilon()) left.set(-from[Z],0.0,from[X]); */
      left.normalize();
      up=left%from;
      // now we have a coordinate system (from, up, left)
      // we want to rotate to: (-from, up, -left)
      CoordType fxx,fyy,fzz,fxy,fxz,fyz;
      fxx=-from[X]*from[X]; fyy=-from[Y]*from[Y]; fzz=-from[Z]*from[Z];
      fxy=-from[X]*from[Y]; fxz=-from[X]*from[Z]; fyz=-from[Y]*from[Z];
      CoordType uxx,uyy,uzz,uxy,uxz,uyz;
      uxx=up[X]*up[X]; uyy=up[Y]*up[Y]; uzz=up[Z]*up[Z];
      uxy=up[X]*up[Y]; uxz=up[X]*up[Z]; uyz=up[Y]*up[Z];
      CoordType lxx,lyy,lzz,lxy,lxz,lyz;
      lxx=-left[X]*left[X]; lyy=-left[Y]*left[Y]; lzz=-left[Z]*left[Z];
      lxy=-left[X]*left[Y]; lxz=-left[X]*left[Z]; lyz=-left[Y]*left[Z];
      // symmetric matrix 
      mtx[0][0]=fxx+uxx+lxx; mtx[1][0]=fxy+uxy+lxy; mtx[2][0]=fxz+uxz+lxz; 
      mtx[0][1]=mtx[1][0];   mtx[1][1]=fyy+uyy+lyy; mtx[2][1]=fyz+uyz+lyz; 
      mtx[0][2]=mtx[2][0];   mtx[1][2]=mtx[2][1];   mtx[2][2]=fzz+uzz+lzz;   
   }
   else
   {
      h=(1.0-e)/(v*v);
#if 1 // unoptimized version -- no performance gain, though: seems like a decent compiler
      mtx[0][0]=e+h*v[X]*v[X];   mtx[1][0]=h*v[X]*v[Y]-v[Z]; mtx[2][0]=h*v[X]*v[Z]+v[Y]; 
      mtx[0][1]=h*v[X]*v[Y]+v[Z]; mtx[1][1]=e+h*v[Y]*v[Y];   mtx[2][1]=h*v[Y]*v[Z]-v[X]; 
      mtx[0][2]=h*v[X]*v[Z]-v[Y]; mtx[1][2]=h*v[Y]*v[Z]+v[X]; mtx[2][2]=e+h*v[Z]*v[Z];   
#else // optimized version (9 mults less)
      CoordType hvx,hvz,hvxy,hvxz,hvyz;
      hvx=h*v[X];
      hvz=h*v[Z];
      hvxy=hvx*v[Y];
      hvxz=hvx*v[Z];
      hvyz=hvz*v[Y];
      mtx[0][0]=e+hvx*v[X]; mtx[1][0]=hvxy-v[Z];    mtx[2][0]=hvxz+v[Y]; 
      mtx[0][1]=hvxy+v[Z];  mtx[1][1]=e+h*v[Y]*v[Y]; mtx[2][1]=hvyz-v[X]; 
      mtx[0][2]=hvxz-v[Y];  mtx[1][2]=hvyz+v[X];    mtx[2][2]=e+hvz*v[Z];   
#endif
}
}

template<class CoordType>
inline void Mtx3<CoordType>::transpose(void)
{
   SWAP(mtx[1][0],mtx[0][1]);
   SWAP(mtx[2][0],mtx[0][2]);
   SWAP(mtx[1][2],mtx[2][1]);
}

template<class CoordType>
inline bool Mtx3<CoordType>::invert(void)
{
#define SUBDETERMINANT(row1,col1,row2,col2) \
  mtx[col1][row1]*mtx[col2][row2]-mtx[col2][row1]*mtx[col1][row2]

   CoordType det=mtx[0][0]*mtx[1][1]*mtx[2][2]+
      mtx[1][0]*mtx[2][1]*mtx[0][2]+
      mtx[2][0]*mtx[0][1]*mtx[1][2]-
      mtx[2][0]*mtx[1][1]*mtx[0][2]-
      mtx[1][0]*mtx[0][1]*mtx[2][2]-
      mtx[0][0]*mtx[2][1]*mtx[1][2];
   
/*   if(abs(det)<numeric_limits<CoordType>::epsilon()) */
   if(abs(det)<0.0000001)
      return false;
   CoordType invdet=1.0/det;
   Mtx3<CoordType> M;
   
   M.mtx[0][0]=invdet*SUBDETERMINANT(1,1,2,2);
   M.mtx[0][1]=invdet*SUBDETERMINANT(0,1,2,2);
   M.mtx[0][2]=invdet*SUBDETERMINANT(1,0,2,1);
   
   M.mtx[1][0]=invdet*SUBDETERMINANT(0,1,2,2);
   M.mtx[1][1]=invdet*SUBDETERMINANT(0,0,2,2);
   M.mtx[1][2]=invdet*SUBDETERMINANT(1,0,2,1);
   
   M.mtx[2][0]=invdet*SUBDETERMINANT(0,1,1,2);
   M.mtx[2][1]=invdet*SUBDETERMINANT(0,0,1,2);
   M.mtx[2][2]=invdet*SUBDETERMINANT(0,0,1,1);
   
   set(M);
   
   return true;
#undef SUBDETERMINANT
}

template<class CoordType>
inline void Mtx3<CoordType>::invertOrtho(void)
{
   transpose();
}

// from Dave Eberly's code
template<class CoordType>
inline void Mtx3<CoordType>::tridiagonal(CoordType afDiag[3], CoordType afSubDiag[3])
{   
   // Householder reduction T = Q^t M Q
   //   Input:   
   //     mat, symmetric 3x3 matrix M
   //   Output:  
   //     mat, orthogonal matrix Q
   //     diag, diagonal entries of T
   //     subd, subdiagonal entries of T (T is symmetric)

   CoordType fA = mtx[0][0];
   CoordType fB = mtx[0][1];
   CoordType fC = mtx[0][2];
   CoordType fD = mtx[1][1];
   CoordType fE = mtx[1][2];
   CoordType fF = mtx[2][2];
   
   afDiag[0] = fA;
   afSubDiag[2] = 0.0;
   if(fabs(fC) >= 1.0e-06)  // used to be EPSILON
   {
      CoordType fLength = sqrt(fB*fB+fC*fC);
      CoordType fInvLength = 1.0/fLength;
      fB *= fInvLength;
      fC *= fInvLength;
      CoordType fQ = 2.0*fB*fE+fC*(fF-fD);
      afDiag[1] = fD+fC*fQ;
      afDiag[2] = fF-fC*fQ;
      afSubDiag[0] = fLength;
      afSubDiag[1] = fE-fB*fQ;
      mtx[0][0] = 1.0;
      mtx[0][1] = 0.0;
      mtx[0][2] = 0.0;
      mtx[1][0] = 0.0;
      mtx[1][1] = fB;
      mtx[1][2] = fC;
      mtx[2][0] = 0.0;
      mtx[2][1] = fC;
      mtx[2][2] = -fB;
   }
   else
   {
      afDiag[1] = fD;
      afDiag[2] = fF;
      afSubDiag[0] = fB;
      afSubDiag[1] = fE;
      mtx[0][0] = 1.0;
      mtx[0][1] = 0.0;
      mtx[0][2] = 0.0;
      mtx[1][0] = 0.0;
      mtx[1][1] = 1.0;
      mtx[1][2] = 0.0;
      mtx[2][0] = 0.0;
      mtx[2][1] = 0.0;
      mtx[2][2] = 1.0;
   }
}
//----------------------------------------------------------------------------
template<class CoordType>
inline bool Mtx3<CoordType>::QLAlgorithm (CoordType afDiag[3], CoordType afSubDiag[3])
{
   // QL iteration with implicit shifting to reduce matrix from tridiagonal
   // to diagonal
   for (int i0 = 0; i0 < 3; i0++)
   {
      const int iMaxIter = 32;
      int iIter;
      for (iIter = 0; iIter < iMaxIter; iIter++)
      {
	 int i1;
	 for (i1 = i0; i1 <= 1; i1++)
	 {
	    CoordType fSum = fabs(afDiag[i1]) + fabs(afDiag[i1+1]);
	    if(fabs(afSubDiag[i1]) + fSum == fSum)
	       break;
	 }
	 if ( i1 == i0 )
	    break;
	 
	 CoordType fTmp0 = (afDiag[i0+1]-afDiag[i0])/(2.0*afSubDiag[i0]);
	 CoordType fTmp1 = sqrt(fTmp0*fTmp0+1.0);
	 if(fTmp0 < 0.0)
	    fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0-fTmp1);
	 else
	    fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0+fTmp1);
	 CoordType fSin = 1.0;
	 CoordType fCos = 1.0;
	 CoordType fTmp2 = 0.0;
	 for (int i2 = i1-1; i2 >= i0; i2--)
	 {
	    CoordType fTmp3 = fSin*afSubDiag[i2];
	    CoordType fTmp4 = fCos*afSubDiag[i2];
	    if ( fabs(fTmp3) >= fabs(fTmp0) )
	    {
	       fCos = fTmp0/fTmp3;
	       fTmp1 = sqrt(fCos*fCos+1.0);
	       afSubDiag[i2+1] = fTmp3*fTmp1;
	       fSin = 1.0/fTmp1;
	       fCos *= fSin;
	    }
	    else
	    {
	       fSin = fTmp3/fTmp0;
	       fTmp1 = sqrt(fSin*fSin+1.0);
	       afSubDiag[i2+1] = fTmp0*fTmp1;
	       fCos = 1.0/fTmp1;
	       fSin *= fCos;
	    }
	    fTmp0 = afDiag[i2+1]-fTmp2;
	    fTmp1 = (afDiag[i2]-fTmp0)*fSin+2.0*fTmp4*fCos;
	    fTmp2 = fSin*fTmp1;
	    afDiag[i2+1] = fTmp0+fTmp2;
	    fTmp0 = fCos*fTmp1-fTmp4;
	    
	    for (int iRow = 0; iRow < 3; iRow++)
	    {
	       fTmp3 = mtx[iRow][i2+1];
	       mtx[iRow][i2+1] = fSin*mtx[iRow][i2] +
		  fCos*fTmp3;
	       mtx[iRow][i2] = fCos*mtx[iRow][i2] -
		  fSin*fTmp3;
	    }
	 }
	 afDiag[i0] -= fTmp2;
	 afSubDiag[i0] = fTmp0;
	 afSubDiag[i1] = 0.0;
      }      
      if ( iIter == iMaxIter )
      {
	 fprintf(stderr,"vecmath.h: QLAlgorithm reached max num iterations.\n");
		// should not get here under normal circumstances
	 return false;
      }
   }
   
   return true;
}
//----------------------------------------------------------------------------
template<class CoordType>
inline void Mtx3<CoordType>::eigenSolveSymmetric(
   CoordType afEigenvalue[3],Vec3<CoordType> akEigenvector[3]) const
{
    Mtx3<CoordType> kMatrix = *this;
    CoordType afSubDiag[3];
    kMatrix.tridiagonal(afEigenvalue,afSubDiag);
    kMatrix.QLAlgorithm(afEigenvalue,afSubDiag);

    for (int i = 0; i < 3; i++)
    {
       akEigenvector[i][0] = kMatrix[0][i];
       akEigenvector[i][1] = kMatrix[1][i];
       akEigenvector[i][2] = kMatrix[2][i];
    }

    // make eigenvectors form a right--handed system
    Vec3<CoordType> kCross = akEigenvector[1] % akEigenvector[2];
    CoordType fDet = akEigenvector[0] * kCross;
    if(fDet < 0.0)
    {
       akEigenvector[2][0] = - akEigenvector[2][0];
       akEigenvector[2][1] = - akEigenvector[2][1];
       akEigenvector[2][2] = - akEigenvector[2][2];
    }
}


template<class CoordType>
inline void Mtx3<CoordType>::debugprint(void) const
{
   fprintf( stderr, "%2.4f %2.4f %2.4f\n",mtx[0][0],mtx[1][0],mtx[2][0]);
   fprintf( stderr, "%2.4f %2.4f %2.4f\n",mtx[0][1],mtx[1][1],mtx[2][1]);
   fprintf( stderr, "%2.4f %2.4f %2.4f\n",mtx[0][2],mtx[1][2],mtx[2][2]);
}



///////////////////////////////////////////////////////
//                Mtx4 functions                     //
///////////////////////////////////////////////////////

template<class CoordType>
inline Mtx4<CoordType>::Mtx4()  // if debug is on, then set to infinity!
{
#if _DEBUG
/*mtx[0][0]=mtx[1][0]=mtx[2][0]=mtx[3][0]=numeric_limits<CoordType>::infinity(); */
/*mtx[0][1]=mtx[1][1]=mtx[2][1]=mtx[3][1]=numeric_limits<CoordType>::infinity(); */
/*mtx[0][2]=mtx[1][2]=mtx[2][2]=mtx[3][2]=numeric_limits<CoordType>::infinity(); */
/*mtx[0][3]=mtx[1][3]=mtx[2][3]=mtx[3][3]=numeric_limits<CoordType>::infinity(); */
#endif
}

template<class CoordType>
inline Mtx4<CoordType>::Mtx4(
   CoordType m00, CoordType m10, CoordType m20, CoordType m30,
   CoordType m01, CoordType m11, CoordType m21, CoordType m31,
   CoordType m02, CoordType m12, CoordType m22, CoordType m32,
   CoordType m03, CoordType m13, CoordType m23, CoordType m33)
{
   mtx[0][0]=m00; mtx[1][0]=m10; mtx[2][0]=m20; mtx[3][0]=m30;
   mtx[0][1]=m01; mtx[1][1]=m11; mtx[2][1]=m21; mtx[3][1]=m31;
   mtx[0][2]=m02; mtx[1][2]=m12; mtx[2][2]=m22; mtx[3][2]=m32;
   mtx[0][3]=m03; mtx[1][3]=m13; mtx[2][3]=m23; mtx[3][3]=m33;
}

template<class CoordType>
inline Mtx4<CoordType>::Mtx4(const Mtx4 &m)
{
   mtx[0][0]=m.mtx[0][0]; mtx[1][0]=m.mtx[1][0]; mtx[2][0]=m.mtx[2][0]; mtx[3][0]=m.mtx[3][0];
   mtx[0][1]=m.mtx[0][1]; mtx[1][1]=m.mtx[1][1]; mtx[2][1]=m.mtx[2][1]; mtx[3][1]=m.mtx[3][1];
   mtx[0][2]=m.mtx[0][2]; mtx[1][2]=m.mtx[1][2]; mtx[2][2]=m.mtx[2][2]; mtx[3][2]=m.mtx[3][2];
   mtx[0][3]=m.mtx[0][3]; mtx[1][3]=m.mtx[1][3]; mtx[2][3]=m.mtx[2][3]; mtx[3][3]=m.mtx[3][3];
}

template<class CoordType>
inline void Mtx4<CoordType>::set(const Mtx4 &m)
{
   mtx[0][0]=m.mtx[0][0]; mtx[1][0]=m.mtx[1][0]; mtx[2][0]=m.mtx[2][0]; mtx[3][0]=m.mtx[3][0];
   mtx[0][1]=m.mtx[0][1]; mtx[1][1]=m.mtx[1][1]; mtx[2][1]=m.mtx[2][1]; mtx[3][1]=m.mtx[3][1];
   mtx[0][2]=m.mtx[0][2]; mtx[1][2]=m.mtx[1][2]; mtx[2][2]=m.mtx[2][2]; mtx[3][2]=m.mtx[3][2];
   mtx[0][3]=m.mtx[0][3]; mtx[1][3]=m.mtx[1][3]; mtx[2][3]=m.mtx[2][3]; mtx[3][3]=m.mtx[3][3];
}

template<class CoordType>
inline void Mtx4<CoordType>::set(
   CoordType m00, CoordType m10, CoordType m20, CoordType m30,
   CoordType m01, CoordType m11, CoordType m21, CoordType m31,
   CoordType m02, CoordType m12, CoordType m22, CoordType m32,
   CoordType m03, CoordType m13, CoordType m23, CoordType m33)
{
   mtx[0][0]=m00; mtx[1][0]=m10; mtx[2][0]=m20; mtx[3][0]=m30;
   mtx[0][1]=m01; mtx[1][1]=m11; mtx[2][1]=m21; mtx[3][1]=m31;
   mtx[0][2]=m02; mtx[1][2]=m12; mtx[2][2]=m22; mtx[3][2]=m32;
   mtx[0][3]=m03; mtx[1][3]=m13; mtx[2][3]=m23; mtx[3][3]=m33;
}

template<class CoordType>
inline void Mtx4<CoordType>::operator=(const Mtx4 &m)  // operator: assignment
{
   mtx[0][0]=m.mtx[0][0]; mtx[1][0]=m.mtx[1][0]; mtx[2][0]=m.mtx[2][0]; mtx[3][0]=m.mtx[3][0];
   mtx[0][1]=m.mtx[0][1]; mtx[1][1]=m.mtx[1][1]; mtx[2][1]=m.mtx[2][1]; mtx[3][1]=m.mtx[3][1];
   mtx[0][2]=m.mtx[0][2]; mtx[1][2]=m.mtx[1][2]; mtx[2][2]=m.mtx[2][2]; mtx[3][2]=m.mtx[3][2];
   mtx[0][3]=m.mtx[0][3]; mtx[1][3]=m.mtx[1][3]; mtx[2][3]=m.mtx[2][3]; mtx[3][3]=m.mtx[3][3];
}

template<class CoordType>
inline Mtx4<CoordType> Mtx4<CoordType>::operator+(const Mtx4 &m) const // matrix addition
{
   return
      Mtx4(mtx[0][0]+m.mtx[0][0],mtx[1][0]+m.mtx[1][0],mtx[2][0]+m.mtx[2][0],mtx[3][0]+m.mtx[3][0],
	   mtx[0][1]+m.mtx[0][1],mtx[1][1]+m.mtx[1][1],mtx[2][1]+m.mtx[2][1],mtx[3][1]+m.mtx[3][1],	   	   mtx[0][2]+m.mtx[0][2],mtx[1][2]+m.mtx[1][2],mtx[2][2]+m.mtx[2][2],mtx[3][2]+m.mtx[3][2],
	   mtx[0][3]+m.mtx[0][3],mtx[1][3]+m.mtx[1][3],mtx[2][3]+m.mtx[2][3],mtx[3][3]+m.mtx[3][3]);
}

template<class CoordType>
inline Mtx4<CoordType> Mtx4<CoordType>::operator-(const Mtx4 &m) const // matrix subtraction
{
   return
      Mtx4(mtx[0][0]-m.mtx[0][0],mtx[1][0]-m.mtx[1][0],mtx[2][0]-m.mtx[2][0],mtx[3][0]-m.mtx[3][0],
	   mtx[0][1]-m.mtx[0][1],mtx[1][1]-m.mtx[1][1],mtx[2][1]-m.mtx[2][1],mtx[3][1]-m.mtx[3][1],
	   mtx[0][2]-m.mtx[0][2],mtx[1][2]-m.mtx[1][2],mtx[2][2]-m.mtx[2][2],mtx[3][2]-m.mtx[3][2],
	   mtx[0][3]-m.mtx[0][3],mtx[1][3]-m.mtx[1][3],mtx[2][3]-m.mtx[2][3],mtx[3][3]-m.mtx[3][3]);
}

template<class CoordType>
inline Mtx4<CoordType> Mtx4<CoordType>::operator-(void) const // unary -
{
   return Mtx4(-mtx[0][0],-mtx[1][0],-mtx[2][0],-mtx[3][0],
	       -mtx[0][1],-mtx[1][1],-mtx[2][1],-mtx[3][1],
	       -mtx[0][2],-mtx[1][2],-mtx[2][2],-mtx[3][2],
	       -mtx[0][3],-mtx[1][3],-mtx[2][3],-mtx[3][3]);
}

template<class CoordType>
inline Mtx4<CoordType> Mtx4<CoordType>::operator*(const Mtx4 &a) const // matrix matrix multiplication
{
   return
      Mtx4(mtx[0][0]*a.mtx[0][0]+mtx[1][0]*a.mtx[0][1]+mtx[2][0]*a.mtx[0][2]+mtx[3][0]*a.mtx[0][3],
	   mtx[0][0]*a.mtx[1][0]+mtx[1][0]*a.mtx[1][1]+mtx[2][0]*a.mtx[1][2]+mtx[3][0]*a.mtx[1][3],
	   mtx[0][0]*a.mtx[2][0]+mtx[1][0]*a.mtx[2][1]+mtx[2][0]*a.mtx[2][2]+mtx[3][0]*a.mtx[2][3],
	   mtx[0][0]*a.mtx[3][0]+mtx[1][0]*a.mtx[3][1]+mtx[2][0]*a.mtx[3][2]+mtx[3][0]*a.mtx[3][3],
	   
	   mtx[0][1]*a.mtx[0][0]+mtx[1][1]*a.mtx[0][1]+mtx[2][1]*a.mtx[0][2]+mtx[3][1]*a.mtx[0][3],
	   mtx[0][1]*a.mtx[1][0]+mtx[1][1]*a.mtx[1][1]+mtx[2][1]*a.mtx[1][2]+mtx[3][1]*a.mtx[1][3],
	   mtx[0][1]*a.mtx[2][0]+mtx[1][1]*a.mtx[2][1]+mtx[2][1]*a.mtx[2][2]+mtx[3][1]*a.mtx[2][3],
	   mtx[0][1]*a.mtx[3][0]+mtx[1][1]*a.mtx[3][1]+mtx[2][1]*a.mtx[3][2]+mtx[3][1]*a.mtx[3][3],
	   
	   mtx[0][2]*a.mtx[0][0]+mtx[1][2]*a.mtx[0][1]+mtx[2][2]*a.mtx[0][2]+mtx[3][2]*a.mtx[0][3],
	   mtx[0][2]*a.mtx[1][0]+mtx[1][2]*a.mtx[1][1]+mtx[2][2]*a.mtx[1][2]+mtx[3][2]*a.mtx[1][3],
	   mtx[0][2]*a.mtx[2][0]+mtx[1][2]*a.mtx[2][1]+mtx[2][2]*a.mtx[2][2]+mtx[3][2]*a.mtx[2][3],
	   mtx[0][2]*a.mtx[3][0]+mtx[1][2]*a.mtx[3][1]+mtx[2][2]*a.mtx[3][2]+mtx[3][2]*a.mtx[3][3],

	   mtx[0][3]*a.mtx[0][0]+mtx[1][3]*a.mtx[0][1]+mtx[2][3]*a.mtx[0][2]+mtx[3][3]*a.mtx[0][3],
	   mtx[0][3]*a.mtx[1][0]+mtx[1][3]*a.mtx[1][1]+mtx[2][3]*a.mtx[1][2]+mtx[3][3]*a.mtx[1][3],
	   mtx[0][3]*a.mtx[2][0]+mtx[1][3]*a.mtx[2][1]+mtx[2][3]*a.mtx[2][2]+mtx[3][3]*a.mtx[2][3],
	   mtx[0][3]*a.mtx[3][0]+mtx[1][3]*a.mtx[3][1]+mtx[2][3]*a.mtx[3][2]+mtx[3][3]*a.mtx[3][3]
	 );
}

template<class CoordType>
inline Vec3<CoordType> Mtx4<CoordType>::multDivW(const Vec3<CoordType> &v,
CoordType vectorW) const 
// operator: full homogeneous matrix vector mult 
// use multVec if vectorW=0 (i.e., v is a vector)
// for points, use vectorW=1
{
   Vec3<CoordType> Mv(mtx[0][0]*v[X] + mtx[1][0]*v[Y] + mtx[2][0]*v[Z] + mtx[3][0]*vectorW, 
  		              mtx[0][1]*v[X] + mtx[1][1]*v[Y] + mtx[2][1]*v[Z] + mtx[3][1]*vectorW,
		              mtx[0][2]*v[X] + mtx[1][2]*v[Y] + mtx[2][2]*v[Z] + mtx[3][2]*vectorW);
   CoordType        w=mtx[0][3]*v[X] + mtx[1][3]*v[Y] + mtx[2][3]*v[Z] + mtx[3][3]*vectorW;
   // fix this....
   //assert(abs(w)>numeric_limits<CoordType>::epsilon());
   /*   if(abs(w)<numeric_limits<CoordType>::epsilon()) return Mv;// this should not happen, but could due to that the user has created a bad matrix */
   if(fabs(w)<0.000000001) return Mv;
   CoordType invw=1.0f/w;
   
   return Mv*invw;
}

template<class CoordType>
inline Vec3<CoordType> Mtx4<CoordType>::multVec(const Vec3<CoordType> &v) const 
// operator: matrix vector mult (excludes translation)
{
   return Vec3<CoordType>(mtx[0][0]*v[X] + mtx[1][0]*v[Y] + mtx[2][0]*v[Z],
			  mtx[0][1]*v[X] + mtx[1][1]*v[Y] + mtx[2][1]*v[Z],
			  mtx[0][2]*v[X] + mtx[1][2]*v[Y] + mtx[2][2]*v[Z]);
}

template<class CoordType>
inline Vec3<CoordType> Mtx4<CoordType>::multPnt(const Vec3<CoordType> &v) const 
// operator: matrix point mult (includes translation)
{
   return Vec3<CoordType>(mtx[0][0]*v[X] + mtx[1][0]*v[Y] + mtx[2][0]*v[Z] + mtx[3][0],
			  mtx[0][1]*v[X] + mtx[1][1]*v[Y] + mtx[2][1]*v[Z] + mtx[3][1],
			  mtx[0][2]*v[X] + mtx[1][2]*v[Y] + mtx[2][2]*v[Z] + mtx[3][2]);
}

template<class CoordType>
inline void Mtx4<CoordType>::operator+=(const Mtx4 &a) // operator +=
{
   mtx[0][0]+=a.mtx[0][0]; mtx[1][0]+=a.mtx[1][0]; mtx[2][0]+=a.mtx[2][0]; mtx[3][0]+=a.mtx[3][0];
   mtx[0][1]+=a.mtx[0][1]; mtx[1][1]+=a.mtx[1][1]; mtx[2][1]+=a.mtx[2][1]; mtx[3][1]+=a.mtx[3][1];
   mtx[0][2]+=a.mtx[0][2]; mtx[1][2]+=a.mtx[1][2]; mtx[2][2]+=a.mtx[2][2]; mtx[3][2]+=a.mtx[3][2];
   mtx[0][3]+=a.mtx[0][3]; mtx[1][3]+=a.mtx[1][3]; mtx[2][3]+=a.mtx[2][3]; mtx[3][3]+=a.mtx[3][3];
}

template<class CoordType>
inline void Mtx4<CoordType>::operator-=(const Mtx4 &a) // operator -=
{
   mtx[0][0]-=a.mtx[0][0]; mtx[1][0]-=a.mtx[1][0]; mtx[2][0]-=a.mtx[2][0]; mtx[3][0]-=a.mtx[3][0];
   mtx[0][1]-=a.mtx[0][1]; mtx[1][1]-=a.mtx[1][1]; mtx[2][1]-=a.mtx[2][1]; mtx[3][1]-=a.mtx[3][1];
   mtx[0][2]-=a.mtx[0][2]; mtx[1][2]-=a.mtx[1][2]; mtx[2][2]-=a.mtx[2][2]; mtx[3][2]-=a.mtx[3][2];
   mtx[0][3]-=a.mtx[0][3]; mtx[1][3]-=a.mtx[1][3]; mtx[2][3]-=a.mtx[2][3]; mtx[3][3]-=a.mtx[3][3];
}

template<class CoordType>
inline void Mtx4<CoordType>::operator*=(const Mtx4 &a) // operator *=
{
   CoordType x,y,z,w;
   x=mtx[0][0]*a.mtx[0][0]+mtx[1][0]*a.mtx[0][1]+mtx[2][0]*a.mtx[0][2] + mtx[3][0]*a.mtx[0][3];
   y=mtx[0][0]*a.mtx[1][0]+mtx[1][0]*a.mtx[1][1]+mtx[2][0]*a.mtx[1][2] + mtx[3][0]*a.mtx[1][3];
   z=mtx[0][0]*a.mtx[2][0]+mtx[1][0]*a.mtx[2][1]+mtx[2][0]*a.mtx[2][2] + mtx[3][0]*a.mtx[2][3];
   w=mtx[0][0]*a.mtx[3][0]+mtx[1][0]*a.mtx[3][1]+mtx[2][0]*a.mtx[3][2] + mtx[3][0]*a.mtx[3][3];
   mtx[0][0]=x; mtx[1][0]=y; mtx[2][0]=z; mtx[3][0]=w;

   x=mtx[0][1]*a.mtx[0][0]+mtx[1][1]*a.mtx[0][1]+mtx[2][1]*a.mtx[0][2] + mtx[3][1]*a.mtx[0][3];
   y=mtx[0][1]*a.mtx[1][0]+mtx[1][1]*a.mtx[1][1]+mtx[2][1]*a.mtx[1][2] + mtx[3][1]*a.mtx[1][3];
   z=mtx[0][1]*a.mtx[2][0]+mtx[1][1]*a.mtx[2][1]+mtx[2][1]*a.mtx[2][2] + mtx[3][1]*a.mtx[2][3];
   w=mtx[0][1]*a.mtx[3][0]+mtx[1][1]*a.mtx[3][1]+mtx[2][1]*a.mtx[3][2] + mtx[3][1]*a.mtx[3][3];
   mtx[0][1]=x; mtx[1][1]=y; mtx[2][1]=z; mtx[3][1]=w;

   x=mtx[0][2]*a.mtx[0][0]+mtx[1][2]*a.mtx[0][1]+mtx[2][2]*a.mtx[0][2] + mtx[3][2]*a.mtx[0][3];
   y=mtx[0][2]*a.mtx[1][0]+mtx[1][2]*a.mtx[1][1]+mtx[2][2]*a.mtx[1][2] + mtx[3][2]*a.mtx[1][3];
   z=mtx[0][2]*a.mtx[2][0]+mtx[1][2]*a.mtx[2][1]+mtx[2][2]*a.mtx[2][2] + mtx[3][2]*a.mtx[2][3];
   w=mtx[0][2]*a.mtx[3][0]+mtx[1][2]*a.mtx[3][1]+mtx[2][2]*a.mtx[3][2] + mtx[3][2]*a.mtx[3][3];
   mtx[0][2]=x; mtx[1][2]=y; mtx[2][2]=z; mtx[3][2]=w;
}

template<class CoordType>
inline bool Mtx4<CoordType>::operator==(const Mtx4 &a) const // operator ==
{
   return 
      mtx[0][0]==a.mtx[0][0] && mtx[1][0]==a.mtx[1][0] && mtx[2][0]==a.mtx[2][0] && 
      mtx[3][0]==a.mtx[3][0] &&
      mtx[0][1]==a.mtx[0][1] && mtx[1][1]==a.mtx[1][1] && mtx[2][1]==a.mtx[2][1] &&
      mtx[3][1]==a.mtx[3][1] &&
      mtx[0][2]==a.mtx[0][2] && mtx[1][2]==a.mtx[1][2] && mtx[2][2]==a.mtx[2][2] &&
      mtx[3][2]==a.mtx[3][2] &&
      mtx[0][3]==a.mtx[0][3] && mtx[1][3]==a.mtx[1][3] && mtx[2][3]==a.mtx[2][3] &&
      mtx[3][3]==a.mtx[3][3];
}

template<class CoordType>
inline bool Mtx4<CoordType>::operator!=(const Mtx4 &a) const // operator !=
{
   return 
      mtx[0][0]!=a.mtx[0][0] || mtx[1][0]!=a.mtx[1][0] || mtx[2][0]!=a.mtx[2][0] || 
      mtx[3][0]!=a.mtx[3][0] ||
      mtx[0][1]!=a.mtx[0][1] || mtx[1][1]!=a.mtx[1][1] || mtx[2][1]!=a.mtx[2][1] ||
      mtx[3][1]!=a.mtx[3][1] ||
      mtx[0][2]!=a.mtx[0][2] || mtx[1][2]!=a.mtx[1][2] || mtx[2][2]!=a.mtx[2][2] ||
      mtx[3][2]!=a.mtx[3][2] ||
      mtx[0][3]!=a.mtx[0][3] || mtx[1][3]!=a.mtx[1][3] || mtx[2][3]!=a.mtx[2][3] ||
      mtx[3][3]!=a.mtx[3][3];
}

template<class CoordType>
inline void Mtx4<CoordType>::loadIdentity(void) // load matrix with identity matrix
{
   mtx[0][0]=1.0; mtx[1][0]=0.0; mtx[2][0]=0.0; mtx[3][0]=0.0;
   mtx[0][1]=0.0; mtx[1][1]=1.0; mtx[2][1]=0.0; mtx[3][1]=0.0;
   mtx[0][2]=0.0; mtx[1][2]=0.0; mtx[2][2]=1.0; mtx[3][2]=0.0;
   mtx[0][3]=0.0; mtx[1][3]=0.0; mtx[2][3]=0.0; mtx[3][3]=1.0;
}

template<class CoordType>
inline void Mtx4<CoordType>::rotX(CoordType radians) // creates a rotation matrix, "radians" around X-axis
{
   CoordType c,s;
   c=(CoordType)cos(radians);
   s=(CoordType)sin(radians);
   mtx[0][0]=1.0; mtx[1][0]=0.0; mtx[2][0]=0.0; mtx[3][0]=0.0;
   mtx[0][1]=0.0; mtx[1][1]=c;   mtx[2][1]=-s;  mtx[3][1]=0.0;
   mtx[0][2]=0.0; mtx[1][2]=s;   mtx[2][2]=c; mtx[3][2]=0.0;
   mtx[0][3]=0.0; mtx[1][3]=0.0; mtx[2][3]=0.0; mtx[3][3]=1.0;
}

template<class CoordType>
inline void Mtx4<CoordType>::rotY(CoordType radians) // creates a rotation matrix, "radians" around Y-axis
{
   CoordType c,s;
   c=(CoordType)cos(radians);
   s=(CoordType)sin(radians);
   mtx[0][0]=c;   mtx[1][0]=0.0; mtx[2][0]=s;   mtx[3][0]=0.0;
   mtx[0][1]=0.0; mtx[1][1]=1.0; mtx[2][1]=0.0; mtx[3][1]=0.0;
   mtx[0][2]=-s;  mtx[1][2]=0.0; mtx[2][2]=c; mtx[3][2]=0.0;
   mtx[0][3]=0.0; mtx[1][3]=0.0; mtx[2][3]=0.0; mtx[3][3]=1.0;
}

template<class CoordType>
inline void Mtx4<CoordType>::rotZ(CoordType radians) // creates a rotation matrix, "radians" around Z-axis
{
   CoordType c,s;
   c=(CoordType)cos(radians);
   s=(CoordType)sin(radians);
   mtx[0][0]=c;   mtx[1][0]=-s;  mtx[2][0]=0.0; mtx[3][0]=0.0;
   mtx[0][1]=s;   mtx[1][1]=c;   mtx[2][1]=0.0; mtx[3][1]=0.0;
   mtx[0][2]=0.0; mtx[1][2]=0.0; mtx[2][2]=1.0; mtx[3][2]=0.0;
   mtx[0][3]=0.0; mtx[1][3]=0.0; mtx[2][3]=0.0; mtx[3][3]=1.0;
}

template<class CoordType>
inline void Mtx4<CoordType>::translate(CoordType tx,CoordType ty,CoordType tz) // creates a translation matrix
{
   mtx[0][0]=1.0;    mtx[1][0]=0.0;    mtx[2][0]=0.0;    mtx[3][0]=tx;
   mtx[0][1]=0.0;    mtx[1][1]=1.0;    mtx[2][1]=0.0;    mtx[3][1]=ty;
   mtx[0][2]=0.0;    mtx[1][2]=0.0;    mtx[2][2]=1.0;    mtx[3][2]=tz;
   mtx[0][3]=0.0;    mtx[1][3]=0.0;    mtx[2][3]=0.0;    mtx[3][3]=1.0;
}

template<class CoordType>
inline void Mtx4<CoordType>::scale(CoordType scaleX,CoordType scaleY,CoordType scaleZ) // creates a scaling matrix
{
   mtx[0][0]=scaleX; mtx[1][0]=0.0;    mtx[2][0]=0.0;    mtx[3][0]=0.0;
   mtx[0][1]=0.0;    mtx[1][1]=scaleY; mtx[2][1]=0.0;    mtx[3][1]=0.0;
   mtx[0][2]=0.0;    mtx[1][2]=0.0;    mtx[2][2]=scaleZ; mtx[3][2]=0.0;
   mtx[0][3]=0.0;    mtx[1][3]=0.0;    mtx[2][3]=0.0;    mtx[3][3]=1.0;
}

template<class CoordType>
inline void Mtx4<CoordType>::rotAxis(Vec3<CoordType> &axis,CoordType radians)
// creates rotation matrix, rotates "radians" around "axis", axis must be normalized
{
   // from Ken Shoemake's gem: uses a quaternion to construct the matrix
   radians*=0.5;
   CoordType sinalpha=(CoordType)sin(radians);
   CoordType cosalpha=(CoordType)cos(radians);

   // create the quaternion
   CoordType q[4]={axis[X]*sinalpha, axis[Y]*sinalpha, axis[Z]*sinalpha, cosalpha};

   CoordType Nq=q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
   CoordType s=(Nq > 0.0f) ? (2.0f / Nq) : 0.0f;
   CoordType xs=q[0]*s,  ys=q[1]*s,  zs=q[2]*s;
   CoordType wx=q[3]*xs, wy=q[3]*ys, wz=q[3]*zs;
   CoordType xx=q[0]*xs, xy=q[0]*ys, xz=q[0]*zs;
   CoordType yy=q[1]*ys, yz=q[1]*zs, zz=q[2]*zs;
   
   mtx[0][0]=1.0f-(yy + zz);mtx[1][0]=xy - wz;     mtx[2][0]=xz + wy;     mtx[3][0]=0.0f;
   mtx[0][1]=xy + wz;       mtx[1][1]=1.0f-(xx+zz);mtx[2][1]=yz - wx;     mtx[3][1]=0.0f;
   mtx[0][2]=xz - wy;       mtx[1][2]=yz + wx;     mtx[2][2]=1.0f-(xx+yy);mtx[3][2]=0.0f;
   mtx[0][3]=0.0f;          mtx[1][3]=0.0f;        mtx[2][3]=0.0f;        mtx[3][3]=1.0f;
}

template<class CoordType>
inline void Mtx4<CoordType>::vecRotVec(Vec3<CoordType> &from,Vec3<CoordType> &to)
// rotation matrix, from from-vector to to-vector (from, to must be normalized)
{
// see chapter Transforms in Real-Time Rendering ;-)
// shoudl be updated with Hughes and Moller's newest code.
   Vec3<CoordType> v;
   CoordType e,h;
   v=from%to;  // v = from x to (cross product)
   e=from*to;  // e = from . to (dot product)
   //if(e>1.0-numeric_limits<CoordType>::epsilon()) // "from" almost or equal to "to"-vector?
   if(e>0.9999999)
   {
      loadIdentity();  // return identity
   }
   //else if(e<-1.0+numeric_limits<CoordType>::epsilon())  // from almost or equal to negated "to"-vector?
   else if(e<-0.99999999)
   {
      Vec3<CoordType> up,left;
      left.set(0.0,from[Z],-from[Y]);
/*      if(left*left<numeric_limits<CoordType>::epsilon()) left.set(-from[Z],0.0,from[X]); */
      if(left*left<0.0000001) left.set(-from[Z],0.0,from[X]);
      left.normalize();
      up=left%from;
// now we have a coordinate system (from, up, left)
// we want to rotate to: (-from, up, -left)
      CoordType fxx,fyy,fzz,fxy,fxz,fyz;
      fxx=-from[X]*from[X]; fyy=-from[Y]*from[Y]; fzz=-from[Z]*from[Z];
      fxy=-from[X]*from[Y]; fxz=-from[X]*from[Z]; fyz=-from[Y]*from[Z];
      CoordType uxx,uyy,uzz,uxy,uxz,uyz;
      uxx=up[X]*up[X]; uyy=up[Y]*up[Y]; uzz=up[Z]*up[Z];
      uxy=up[X]*up[Y]; uxz=up[X]*up[Z]; uyz=up[Y]*up[Z];
      CoordType lxx,lyy,lzz,lxy,lxz,lyz;
      lxx=-left[X]*left[X]; lyy=-left[Y]*left[Y]; lzz=-left[Z]*left[Z];
      lxy=-left[X]*left[Y]; lxz=-left[X]*left[Z]; lyz=-left[Y]*left[Z];
// symmetric matrix 
      mtx[0][0]=fxx+uxx+lxx; mtx[1][0]=fxy+uxy+lxy; mtx[2][0]=fxz+uxz+lxz; mtx[3][0]=0.0;
      mtx[0][1]=mtx[1][0];   mtx[1][1]=fyy+uyy+lyy; mtx[2][1]=fyz+uyz+lyz; mtx[3][1]=0.0; 
      mtx[0][2]=mtx[2][0];   mtx[1][2]=mtx[2][1];   mtx[2][2]=fzz+uzz+lzz; mtx[3][2]=0.0;   
      mtx[0][3]=0.0;         mtx[1][3]=0.0;         mtx[2][3]=0.0;         mtx[3][3]=1.0;   
   }
   else
   {
      h=(1.0-e)/(v*v);
#if 0 // unoptimized version -- no performance gain, though: seems like a decent compiler
      mtx[0][0]=e+h*v[X]*v[X];   mtx[1][0]=h*v[X]*v[Y]-v[Z]; mtx[2][0]=h*v[X]*v[Z]+v[Y];
      mtx[3][0]=0.0;
      mtx[0][1]=h*v[X]*v[Y]+v[Z]; mtx[1][1]=e+h*v[Y]*v[Y];   mtx[2][1]=h*v[Y]*v[Z]-v[X];
      mtx[3][1]=0.0; 
      mtx[0][2]=h*v[X]*v[Z]-v[Y]; mtx[1][2]=h*v[Y]*v[Z]+v[X]; mtx[2][2]=e+h*v[Z]*v[Z];  
      mtx[3][2]=0.0;   
      mtx[0][3]=0.0;           mtx[1][3]=0.0;           mtx[2][3]=0.0;          
      mtx[3][3]=1.0;   
#else // optimized version (9 mults less)
      CoordType hvx,hvz,hvxy,hvxz,hvyz;
      hvx=h*v[X];
      hvz=h*v[Z];
      hvxy=hvx*v[Y];
      hvxz=hvx*v[Z];
      hvyz=hvz*v[Y];
      mtx[0][0]=e+hvx*v[X]; mtx[1][0]=hvxy-v[Z];     mtx[2][0]=hvxz+v[Y];  mtx[3][0]=0.0;
      mtx[0][1]=hvxy+v[Z];  mtx[1][1]=e+h*v[Y]*v[Y]; mtx[2][1]=hvyz-v[X];  mtx[3][1]=0.0;
      mtx[0][2]=hvxz-v[Y];  mtx[1][2]=hvyz+v[X];     mtx[2][2]=e+hvz*v[Z]; mtx[3][2]=0.0;
      mtx[0][3]=0.0;        mtx[1][3]=0.0;           mtx[2][3]=0.0;        mtx[3][3]=1.0;        
#endif
   }
}

// creates a rotation matrix from the coordsystem (fromView, fromUp, fromView x fromUp)
// to (toView, toUp, toView x toUp)
// assumes all vectors are normalized, and that View is perpendicular to Up
template<class CoordType>
inline void Mtx4<CoordType>::billboardMatrix(Vec3<CoordType> &fromView,
   Vec3<CoordType> &fromUp,Vec3<CoordType> &toView,Vec3<CoordType> toUp)
{
   Vec3<CoordType> fromRight,toRight;
   fromRight=fromView%fromUp;  // right = view x up (cross product)
   toRight=toView%toUp;

   Mtx4<CoordType> M,N;
   M.set(fromView[X], fromView[Y], fromView[Z], 0,
	 fromUp[X],   fromUp[Y],   fromUp[Z],   0,
	 fromRight[X],fromRight[Y],fromRight[Z],0,
	 0,           0,           0,           1);
   N.set(toView[X], toUp[X], toRight[X], 0,
	 toView[Y], toUp[Y], toRight[Y], 0,
	 toView[Z], toUp[Z], toRight[Z], 0,
	 0,         0,       0,          1);
   set(N*M);
}

template<class CoordType>
inline void Mtx4<CoordType>::transpose(void)
{
   SWAP(mtx[1][0],mtx[0][1]);
   SWAP(mtx[2][0],mtx[0][2]);
   SWAP(mtx[3][0],mtx[0][3]);
   SWAP(mtx[1][2],mtx[2][1]);
   SWAP(mtx[1][3],mtx[3][1]);
   SWAP(mtx[2][3],mtx[3][2]);
}

// this function was taken from somewhere...
template<class CoordType>
inline bool Mtx4<CoordType>::invert(void)
{
   Mtx4<CoordType> BB; 
   const int n=4;
   int indxc[n],indxr[n],ipiv[n];
   int i,icol=0,irow=0,j,k,l,ll;
   CoordType big,dum,pivinv;
   BB.loadIdentity();
   for(j=0;j<n;j++) ipiv[j]=0;
   for(i=0;i<n;i++)
   {
      big=0.0;
      for(j=0;j<n;j++)
      {
	 if(ipiv[j]!=1)
	 {
	    for(k=0;k<n;k++)
	    {
	       if(ipiv[k]==0)
	       {
		  if(fabs(mtx[j][k])>=big)
		  {
		     big=(CoordType)fabs(mtx[j][k]);
		     irow=j;
		     icol=k;
		  }
	       }
	       else if(ipiv[k]>1)
	       {
		  return false;  // singular matrix
	       }
	    }
	 }
      }
      ++ipiv[icol];
      if(irow!=icol)
      {
	 for(l=0;l<n;l++) SWAP(mtx[irow][l],mtx[icol][l]);
	 for(l=0;l<n;l++) SWAP(BB.mtx[irow][l],BB.mtx[icol][l]);
      }
      indxr[i]=irow;
      indxc[i]=icol;
      if(mtx[icol][icol]==0.0) return false;  // singular matrix
      pivinv=1.0f/mtx[icol][icol];
      mtx[icol][icol]=1.0;
      
      for(l=0;l<n;l++) mtx[icol][l]*=pivinv;
      for(l=0;l<n;l++) BB.mtx[icol][l]*=pivinv;
      
      for(ll=0;ll<n;ll++)
      {
	 if(ll!=icol)
	 {
	    dum=mtx[ll][icol];
	    mtx[ll][icol]=0.0;
	    for(l=0;l<n;l++) mtx[ll][l]-=mtx[icol][l]*dum;
	    for(l=0;l<n;l++) BB.mtx[ll][l]-=BB.mtx[icol][l]*dum;
	 }
      }
   }
   
   for(l=n-1;l>=0;l--)
   {
      if(indxr[l]!=indxc[l])
      {
	 for(k=0;k<n;k++)
	    SWAP(mtx[k][indxr[l]],mtx[k][indxc[l]]);
      }
   }
   return true;
}

template<class CoordType>
inline void Mtx4<CoordType>::invertOrtho(void) // assume upper left 3x3 matrix is orthogonal...
// ...the rest of the matrix is zeroes, with a one in the lower right corner
{
   SWAP(mtx[1][0],mtx[0][1]);// invert by transposing the upper left 3x3 matrix
   SWAP(mtx[2][0],mtx[0][2]);
   SWAP(mtx[1][2],mtx[2][1]);
}

template<class CoordType>
inline void Mtx4<CoordType>::invertOrthoTrans(void) // assume matrix is 3x3 ortho plus translation
{
   SWAP(mtx[1][0],mtx[0][1]);// invert by transposing the upper left 3x3 matrix
   SWAP(mtx[2][0],mtx[0][2]);
   SWAP(mtx[1][2],mtx[2][1]);
   // then take care of the translation
   CoordType tx,ty,tz;
   tx=mtx[3][0];
   ty=mtx[3][1];
   tz=mtx[3][2];
   mtx[3][0]=-(mtx[0][0]*tx+mtx[1][0]*ty+mtx[2][0]*tz);
   mtx[3][1]=-(mtx[0][1]*tx+mtx[1][1]*ty+mtx[2][1]*tz);
   mtx[3][2]=-(mtx[0][2]*tx+mtx[1][2]*ty+mtx[2][2]*tz);
   mtx[3][3]=1.0;
}


template<class CoordType>
inline void Mtx4<CoordType>::debugprint(void) const
{
	fprintf( stderr, "%2.4f %2.4f %2.4f %2.4f\n",mtx[0][0],mtx[1][0],mtx[2][0],mtx[3][0]);
	fprintf( stderr, "%2.4f %2.4f %2.4f %2.4f\n",mtx[0][1],mtx[1][1],mtx[2][1],mtx[3][1]);
	fprintf( stderr, "%2.4f %2.4f %2.4f %2.4f\n",mtx[0][2],mtx[1][2],mtx[2][2],mtx[3][2]);
	fprintf( stderr, "%2.4f %2.4f %2.4f %2.4f\n",mtx[0][3],mtx[1][3],mtx[2][3],mtx[3][3]);
}


typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int> Vec2i;

typedef Vec3<double> Vec3d;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;
//typedef Vec3<uint> Vec3ui;
typedef Vec3f Color3f;

typedef Vec4<float> Vec4f;
typedef Vec4f Color4f;  

typedef Mtx3<float> Mtx3f;
typedef Mtx3<double> Mtx3d;

typedef Mtx4<float> Mtx4f;
typedef Mtx4<double> Mtx4d;

typedef float real ;

typedef Vec2<real> Vec2r ;
typedef Vec3<real> Vec3r ;
typedef Vec4<real> Vec4r ;
typedef Mtx3<real> Mtx3r ;
typedef Mtx4<real> Mtx4r ;

typedef Vec3r Color3r ;
typedef Vec4r Color4r ;



#undef SWAP





// Bounding box classes
template<class CoordType>
class BBox2
{
public:
	Vec2<CoordType> m_min;
	Vec2<CoordType> m_max;
	BBox2(void) 
	{
		m_min = Vec2<CoordType>(FLT_MAX, FLT_MAX);
		m_max = Vec2<CoordType>(-FLT_MAX, -FLT_MAX);
	}

	BBox2(const Vec2<CoordType> &minCorner, const Vec2<CoordType> &maxCorner) : m_min(minCorner), m_max(maxCorner) {}
	BBox2 &					operator=(const BBox2 &box);
	void					setMinCorner(const Vec2<CoordType> &minCorner);
	void					setMaxCorner(const Vec2<CoordType> &maxCorner);
	Vec2<CoordType> &		getMinCorner(void);
	const Vec2<CoordType> &	getMinCorner(void) const;
	Vec2<CoordType> &		getMaxCorner(void);
	const Vec2<CoordType> &	getMaxCorner(void) const;
	Vec2<CoordType>			centerPoint(void) const;
	Vec2<CoordType>			halfVector(void) const; 
	Vec2<CoordType>			size(void) const;
	void					grow(float minSize, float percentGrow);
	bool					isInside(const Vec2<CoordType> &point) const; // Checks if this position is inside the box
	
	inline void	operator+=(const Vec2<CoordType>& p) 
	{
		for (int i=0; i<2; ++i)
		{
			if (p[i] < m_min[i])
				m_min[i] = p[i];
			if (p[i] > m_max[i])
				m_max[i] = p[i];
		}
	}

	inline void operator+=(const BBox2 &bb) 
	{
		const Vec2<CoordType>& mx = bb.getMaxCorner();
		const Vec2<CoordType>& mn = bb.getMinCorner();
		for (int i = 0; i < 2; i++) {
			if (mn[i] < m_min[i])
				m_min[i] = mn[i];
			if (mx[i] > m_max[i])
				m_max[i] = mx[i];
		}
	}

	bool intersect(BBox2& box2, Mtx4<CoordType>& mtx) //
	{
		// Transform obj2 with mtx. I.e., create a bbox around the 4 transformed corner points.
		// This can be optimized by only transforming origin + the 2 axes instead.
		// It is perhaps abit weard to allow a 2D-bounding box to be transformed by a 4x4-matrix.
		// For simplicity, we allow this and just ignore the resulting z-values.
		// The reason is to maintain similarity to the BBox3f-class and to be able to use 
		// standard 4x4-matrices to perform transformations instead of some less standard matrices

		BBox2 transformedBox2;
		Vec3<CoordType> minpt = Vec3<CoordType>(box2.m_min[0], box2.m_min[1], 0); //
		Vec2<CoordType> dim = box2.m_max - box2.m_min;
		transformedBox2 += *(Vec2<CoordType>*)&mtx.multPnt(minpt);
		transformedBox2 += *(Vec2<CoordType>*)&mtx.multPnt(minpt+Vec3<CoordType>(0,dim[1],0));
		transformedBox2 += *(Vec2<CoordType>*)&mtx.multPnt(minpt+Vec3<CoordType>(dim[0],0,0));
		transformedBox2 += *(Vec2<CoordType>*)&mtx.multPnt(minpt+Vec3<CoordType>(dim[0],dim[1],0));
		
		for(int i=0; i<2; i++)
		{
			if((transformedBox2.m_max[i] < m_min[i]) || (m_max[i] < transformedBox2.m_min[i]))
				return false;
		}
		return true;
	};
	
};

template<class CoordType>
inline BBox2<CoordType> &BBox2<CoordType>::operator=(const BBox2<CoordType> &box) {
	m_min = box.m_min;
	m_max = box.m_max;
	return *this;
}

template<class CoordType>
inline void BBox2<CoordType>::setMinCorner(const Vec2<CoordType> &minCorner) {
	m_min = minCorner;
}

template<class CoordType>
inline void BBox2<CoordType>::setMaxCorner(const Vec2<CoordType> &maxCorner) {
	m_max = maxCorner;
}

template<class CoordType>
inline Vec2<CoordType> & BBox2<CoordType>::getMinCorner(void) {
	return m_min;
}

template<class CoordType>
inline const Vec2<CoordType> &BBox2<CoordType>::getMinCorner(void) const {
	return m_min;
}

template<class CoordType>
inline Vec2<CoordType> &BBox2<CoordType>::getMaxCorner(void) {
	return m_max;
}

template<class CoordType>
inline const Vec2<CoordType> &BBox2<CoordType>::getMaxCorner(void) const {
	return m_max;
}

template<class CoordType>
inline bool BBox2<CoordType>::isInside(const Vec2<CoordType> &point) const {
	return point[0] > m_min[0] && point[0] < m_max[0] &&
		   point[1] > m_min[1] && point[1] < m_max[1]; // &&
//		   point[2] > m_min[2] && point[2] < m_max[2];
}

template<class CoordType>
inline Vec2<CoordType> BBox2<CoordType>::centerPoint(void) const {
	return (m_max + m_min)*0.5f;
}

template<class CoordType>
inline Vec2<CoordType> BBox2<CoordType>::halfVector(void) const {
	return (m_max - m_min)*0.5f;
}

template<class CoordType>
inline Vec2<CoordType> BBox2<CoordType>::size(void) const {
	return (m_max - m_min);
}

template<class CoordType>
inline void BBox2<CoordType>::grow(float minSize, float percentGrow) 
{
	Vec3f bbExpand;
	for(int i=0; i<3; i++)
		bbExpand[i] = ((getMaxCorner()[i] - getMinCorner()[i]) < minSize) ? minSize : 0.0f;

	const Vec3f cp = centerPoint();
	const Vec3f extend = halfVector() * percentGrow;

	setMaxCorner(cp + bbExpand + extend);
	setMinCorner(cp - bbExpand - extend);
}



template<class CoordType>
class BBox3
{
public:
	Vec3<CoordType> m_min;
	Vec3<CoordType> m_max;
	BBox3(void) 
	{
		m_min = Vec3<CoordType>(FLT_MAX, FLT_MAX, FLT_MAX);
		m_max = Vec3<CoordType>(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	}

	BBox3(const Vec3<CoordType> &minCorner, const Vec3<CoordType> &maxCorner) : m_min(minCorner), m_max(maxCorner) {}
	BBox3 &					operator=(const BBox3 &box);
	void					setMinCorner(const Vec3<CoordType> &minCorner);
	void					setMaxCorner(const Vec3<CoordType> &maxCorner);
	Vec3<CoordType> &		getMinCorner(void);
	const Vec3<CoordType> &	getMinCorner(void) const;
	Vec3<CoordType> &		getMaxCorner(void);
	const Vec3<CoordType> &	getMaxCorner(void) const;
	Vec3<CoordType>			centerPoint(void) const;
	Vec3<CoordType>			halfVector(void) const; 
	Vec3<CoordType>			size(void) const;
	void					grow(float minSize, float percentGrow);
	bool					isInside(const Vec3<CoordType> &point) const; // Checks if this position is inside the box
	
	inline void	operator+=(const Vec3<CoordType>& p) 
	{
		for (int i=0; i<3; ++i)
		{
			if (p[i] < m_min[i])
				m_min[i] = p[i];
			if (p[i] > m_max[i])
				m_max[i] = p[i];
		}
	}

	inline void operator+=(const BBox3 &bb) 
	{
		const Vec3<CoordType>& mx = bb.getMaxCorner();
		const Vec3<CoordType>& mn = bb.getMinCorner();
		for (int i = 0; i < 3; i++) {
			if (mn[i] < m_min[i])
				m_min[i] = mn[i];
			if (mx[i] > m_max[i])
				m_max[i] = mx[i];
		}
	}

	bool intersect(BBox3& box2, Mtx4<CoordType>& mtx) //
	{
		// Transform obj2 with mtx. I.e., create a bbox around the 8 transformed corner points.
		// This can be optimized by only transforming origin + the 3 axes instead.
		BBox3 transformedBox2;
		Vec3<CoordType> minpt = box2.m_min;
		Vec3<CoordType> dim = box2.m_max - box2.m_min;
		transformedBox2 += mtx.multPnt(minpt);
		transformedBox2 += mtx.multPnt(minpt+Vec3<CoordType>(0,0,dim[2]));
		transformedBox2 += mtx.multPnt(minpt+Vec3<CoordType>(0,dim[1],0));
		transformedBox2 += mtx.multPnt(minpt+Vec3<CoordType>(dim[0],0,0));
		transformedBox2 += mtx.multPnt(minpt+Vec3<CoordType>(dim[0],dim[1],0));
		transformedBox2 += mtx.multPnt(minpt+Vec3<CoordType>(0,dim[1],dim[2]));
		transformedBox2 += mtx.multPnt(minpt+Vec3<CoordType>(dim[0],0,dim[2]));
		transformedBox2 += mtx.multPnt(minpt+Vec3<CoordType>(dim[0],dim[1],dim[2]));
		
		for(int i=0; i<3; i++)
		{
			if((transformedBox2.m_max[i] < m_min[i]) || (m_max[i] < transformedBox2.m_min[i]))
				return false;
		}
		return true;
	};
	
};

template<class CoordType>
inline BBox3<CoordType> &BBox3<CoordType>::operator=(const BBox3<CoordType> &box) {
	m_min = box.m_min;
	m_max = box.m_max;
	return *this;
}

template<class CoordType>
inline void BBox3<CoordType>::setMinCorner(const Vec3<CoordType> &minCorner) {
	m_min = minCorner;
}

template<class CoordType>
inline void BBox3<CoordType>::setMaxCorner(const Vec3<CoordType> &maxCorner) {
	m_max = maxCorner;
}

template<class CoordType>
inline Vec3<CoordType> & BBox3<CoordType>::getMinCorner(void) {
	return m_min;
}

template<class CoordType>
inline const Vec3<CoordType> &BBox3<CoordType>::getMinCorner(void) const {
	return m_min;
}

template<class CoordType>
inline Vec3<CoordType> &BBox3<CoordType>::getMaxCorner(void) {
	return m_max;
}

template<class CoordType>
inline const Vec3<CoordType> &BBox3<CoordType>::getMaxCorner(void) const {
	return m_max;
}

template<class CoordType>
inline bool BBox3<CoordType>::isInside(const Vec3<CoordType> &point) const {
	return point[0] > m_min[0] && point[0] < m_max[0] &&
		   point[1] > m_min[1] && point[1] < m_max[1]; // &&
		   point[2] > m_min[2] && point[2] < m_max[2];
}

template<class CoordType>
inline Vec3<CoordType> BBox3<CoordType>::centerPoint(void) const {
	return (m_max + m_min)*0.5f;
}

template<class CoordType>
inline Vec3<CoordType> BBox3<CoordType>::halfVector(void) const {
	return (m_max - m_min)*0.5f;
}

template<class CoordType>
inline Vec3<CoordType> BBox3<CoordType>::size(void) const {
	return (m_max - m_min);
}

template<class CoordType>
inline void BBox3<CoordType>::grow(float minSize, float percentGrow) 
{
	Vec3<CoordType> bbExpand;
	for(int i=0; i<3; i++)
		bbExpand[i] = ((getMaxCorner()[i] - getMinCorner()[i]) < minSize) ? minSize : 0.0f;

	const Vec3<CoordType> cp = centerPoint();
	const Vec3<CoordType> extend = halfVector() * percentGrow;

	setMaxCorner(cp + bbExpand + extend);
	setMinCorner(cp - bbExpand - extend);
}


typedef BBox2<float>	BBox2f;
typedef BBox2<double>	BBox2d;
typedef BBox2<int>		BBox2i;

typedef BBox3<float>	BBox3f;
typedef BBox3<double>	BBox3d;
typedef BBox3<int>		BBox3i;



// Helper functions

// returns distance t to intersection point (or closest point) for vector p0,d0
template<class CoordType> 
CoordType computeIntersectionOfRays(const Vec3<CoordType> p0, const Vec3<CoordType> d0,
									const Vec3<CoordType> p1, const Vec3<CoordType> d1) 
{
	Vec3<CoordType> a = d0 % d1;
	CoordType t = (((p1-p0)%d1) * a) / a.length2(); 
	return t;
}


#endif
