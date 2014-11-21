/*ckwg +5
 * Copyright 2013 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

// .NAME vtkVgHull2D - produce a polygonal n-sided convex hull
// .SECTION Description
// vtkVgHull2D is a filter which will produce an n-sided convex hull given a
// set of n lines. (The convex hull bounds the input polygonal data.)
// The hull is generated by squeezing the lines towards the input
// vtkPolyData, until the lines just touch the vtkPolyData. Then,
// the resulting lines are used to generate a polygon (i.e., hull)
// that is represented by a set of lines.  The depth component of the input
// data will be ignored.
//
// The n lines can be defined in a number of ways including 1) manually
// specifying each line; 2) choosing the four edge lines of the input's
// bounding box; 3) choosing the four vertex lines of the input's
// bounding box; 4) using a recursively subdivided circle.
// Note that when specifying lines, the line "normals" should point
// outside of the convex region.
//
// The output of this filter can be used in combination with vtkLODActor
// to represent a levels-of-detail in the LOD hierarchy. Another use of
// this class is to manually specify the lines, and then generate the
// polygon from the lines (without squeezing the lines towards the
// input). The method GenerateHull() is used to do this.
//
// Note: The output of this algorithm is a set of adjacent line segments,
// in the same way that the output of vtkHull is a set of adjacent polygons.
// In order to compute a closed polygon from this data you will need to do
// further processing.

#ifndef __vtkVgHull2D_h
#define __vtkVgHull2D_h

#include "vtkPolyDataAlgorithm.h"

#include <vgExport.h>

class vtkCellArray;
class vtkPoints;
class vtkPolyData;

class VTKVG_CORE_EXPORT vtkVgHull2D : public vtkPolyDataAlgorithm
{
public:
  static vtkVgHull2D* New();
  vtkTypeMacro(vtkVgHull2D, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Remove all lines from the current set of lines.
  void RemoveAllLines(void);

  // Description:
  // Add a line to the current set of lines. It will be added at the
  // end of the list, and an index that can later be used to set this
  // line's normal will be returned. The values A, B, C are from the
  // line equation Ax + By + C = 0. This vector does not have to
  // have unit length (but it must have a non-zero length!). If a value
  // 0> i>= -NumberOfLines is returned, then the line is parallel
  // with a previously inserted line, and |-i-1| is the index of the
  // line that was previously inserted. If a value i <-NumberOfLines
  // is returned, then the line normal is zero length.
  int  AddLine(double A, double B);
  int  AddLine(double line[2]);

  // Description:
  // Set the normal values for line i. This is a line that was already
  // added to the current set of lines with AddLine(), and is now being
  // modified. The values A, B, and C are from the line equation
  // Ax + By + C = 0. This vector does not have to have unit length.
  // Note that C is set to zero, except in the case of the method taking
  // a vtkLines* argument, where it is set to the C value defined there.
  void SetLine(int i, double A, double B);
  void SetLine(int i, double line[2]);

  // Description:
  // Variations of AddLine()/SetLine() that allow C to be set. These
  // methods are used when GenerateHull() is used.
  int AddLine(double A, double B, double C);
  int AddLine(double line[2], double D);
  void SetLine(int i, double A, double B, double C);
  void SetLine(int i, double line[2], double C);

  // Description:
  // Get the number of lines in the current set of lines.
  vtkGetMacro(NumberOfLines, int);

  // Description:
  // Add the 4 lines that represent the vertices of a square - the combination
  // of the 2 edge lines connecting to a vertex - (1,1),(1,-1),(-1,1),(-1,-1).
  void AddSquareVertexLines();

  // Description:
  // Add the 4 lines that represent the edges of a square -
  // (1,0), (-1,0), (0,1), (0,-1)
  void AddSquareEdgeLines();

  // Description:
  // Add the lines that represent the normals of the vertices of a
  // circle formed by recursively subdividing the edges in a
  // square.  The level indicates how many subdivisions to do with a
  // level of 0 used to add the 4 lines from the original square, level 1
  // will add 8 lines, and so on.
  void AddRecursiveCircleLines(int level);

  // Description:
  // A special method that is used to generate a polygon directly
  // from a set of n lines. The lines that are supplied by the user
  // are not squeezed towards the input data (in fact the user need
  // not specify an input). To use this method, you must provide an
  // instance of vtkPolyData into which the points and cells defining
  // the polygon are placed. You must also provide a bounding box
  // where you expect the resulting polygon to lie. This can be
  // a very generous fit, it's only used to create the initial polygons
  // that are eventually clipped.
  void GenerateHull(vtkPolyData* pd, double* bounds);
  void GenerateHull(vtkPolyData* pd, double xmin, double xmax,
                    double ymin, double ymax, double zmin, double zmax);

protected:
  vtkVgHull2D();
  ~vtkVgHull2D();

  // The lines - 4 doubles per line for A, B, C, D
  double*     Lines;

  // This indicates the current size (in lines - 4*sizeof(double)) of
  // the this->Lines array. Lines are allocated in chunks so that the
  // array does not need to be reallocated every time a new line is added
  int       LinesStorageSize;

  // The number of lines that have been added
  int       NumberOfLines;

  // Internal method used to find the position of each line
  void      ComputeLineDistances(vtkPolyData* input);

  // Internal method used to create the actual polygons from the set
  // of lines
  void      ClipSegmentsFromLines(vtkPoints* points, vtkCellArray* polys,
                                  double* bounds);

  // Internal method used to create the initial "big" polygon from the
  // line equation. This polygon is clipped by all other lines to form
  // the final polygon (or it may be clipped entirely)
  void      CreateInitialSegment(double*, int, double*);

  // The method that does it all...
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*);
private:
  vtkVgHull2D(const vtkVgHull2D&);  // Not implemented.
  void operator=(const vtkVgHull2D&);  // Not implemented.
};

#endif