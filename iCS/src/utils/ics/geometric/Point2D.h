/****************************************************************************/
/// @file    Point2D.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Jan 4, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

/*! \file point2D.h
 \brief Data Structure of the 2D point for the iTetris DigitalMap and Location Referencing blocks.
*/

#ifndef POINT2D_H_
#define POINT2D_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <cmath>

#define POSITION_EPS 0.1		/**< Maximum difference value */

namespace ics_types {

/*! \class Point2D
 \brief Position class adapted from SUMO.

 This class contains the coordinates of a Cartesian point and some useful functions.
*/
class Point2D {

public:
    Point2D() : 				/**< Void constructor. */
            myX(0.0), myY(0.0) {}

    Point2D(float x, float y):	/**< Alternative constructor. */
            myX(x), myY(y) {}

    ~Point2D() {}				/**< Destructor. */

    /*! \fn float x() const
     \brief returns the value of myX.
    */
    float x() const {
        return myX;
    }

    /*! \fn float y() const
     \brief returns the value of myY.
    */
    float y() const {
        return myY;
    }

    /*! \fn void set(float x, float y)
     \brief Set the values x and y of the point.
     \param[in] x X coordinate of the point.
     \param[in] y X coordinate of the point.
    */
    void set(float x, float y) {
        myX = x;
        myY = y;
    }

    /*! \fn void setx(float x)
     \brief Set only the x value of the point.
     \param[in] x X coordinate of the point.
    */
    void setx(float x) {
        myX = x;
    }

    /*! \fn void sety(float y)
     \brief Set only the y value of the point.
     \param[in] y Y coordinate of the point.
    */
    void sety(float y) {
        myY = y;
    }

    /*! \fn void set(const Point2D &pos)
     \brief Set the values of the point given the address of another point.
     \param[in] &pos address of the input point.
    */
    void set(const Point2D &pos) {
        myX = pos.myX;
        myY = pos.myY;
    }

    /*! \fn void mul(float val)
     \brief Multiply the X and Y coordinates of the point by a val value.
     \param[in] val multiplier.
    */
    void mul(float val) {
        myX *= val;
        myY *= val;
    }

    /*! \fn void mul(float mx, float my)
     \brief Multiply the X and Y coordinates of the point.
     \param[in] mx multiplier for X.
     \param[in] my multiplier for Y.
    */
    void mul(float mx, float my) {
        myX *= mx;
        myY *= my;
    }

    /*! \fn void add(const Point2D &pos)
     \brief Add the coordinates of a point to the current point.
     \param[in] &pos Address of the input point to be added.
    */
    void add(const Point2D &pos) {
        myX += pos.myX;
        myY += pos.myY;
    }

    /*! \fn void add(float dx, float dy)
     \brief Add two values to the current X and Y.
     \param[in] dx value to be added to X.
     \param[in] dy value to be added to Y.
    */
    void add(float dx, float dy) {
        myX += dx;
        myY += dy;
    }

    /*! \fn void sub(const Point2D &pos)
     \brief Subtract the coordinates of a point to the current point.
     \param[in] &pos Address of the input point to be subtracted.
    */
    void sub(const Point2D &pos) {
        myX -= pos.myX;
        myY -= pos.myY;
    }

    /*! \fn void sub(float dx, float dy)
     \brief Subtract two values to the current X and Y.
     \param[in] dx value to be added to X.
     \param[in] dy value to be added to Y.
    */
    void sub(float dx, float dy) {
        myX -= dx;
        myY -= dy;
    }

    /*! \fn void norm()
    	 \brief Normalize the coordinates of the point to be between 0 and 1.
    */
    void norm() {
        float val = sqrt(myX*myX + myY*myY);
        myX = myX / val;
        myY = myY / val;
    }

    /*! \fn void reshiftRotate(float xoff, float yoff, float rot)
     \brief Shift and rotate the point (useful for vectors).
     \param[in] xoff offset on the X component.
     \param[in] yoff offset on the Y component.
     \param[in] rot rotation component in radians.
    */
    void reshiftRotate(float xoff, float yoff, float rot) {
        float x = myX * cos(rot) - myY * sin(rot) + xoff;
        float y = myX * sin(rot) + yoff + myY * cos(rot);
        myX = x;
        myY = y;
    }

    /*! \fn friend std::ostream &operator<<(std::ostream &os, const Point2D &p)
     \brief Print the coordinates of the point.
    */
    friend std::ostream &operator<<(std::ostream &os, const Point2D &p) {
        os << p.x() << "," << p.y();
        return os;
    }

    /*! \fn bool operator==(const Point2D &p2) const
     \brief Overloading of the 'equal to' operator for the class Point2D.
     \return Output of the operator 'equal to'.
    */
    bool operator==(const Point2D &p2) const {
        return myX==p2.myX && myY==p2.myY;
    }

    /*! \fn bool operator!=(const Point2D &p2) const
     \brief Overloading of the 'not equal to' operator for the class Point2D.
     \return Output of the operator 'not equal to'.
    */
    bool operator!=(const Point2D &p2) const {
        return myX!=p2.myX || myY!=p2.myY;
    }

    /*! \fn bool almostSame(const Point2D &p2, float maxDiv=POSITION_EPS) const
     \brief Compare the coordinates of the point with the one given as input.
     \param[in] &p2 Address of the reference point.
     \param[in] maxDiv Maximum difference value.
     \return true if the points are the (almost) same, false otherwise.
    */
    bool almostSame(const Point2D &p2, float maxDiv=POSITION_EPS) const {
        return fabs(myX-p2.myX)<maxDiv && fabs(myY-p2.myY)<maxDiv;
    }

    /*! \fn inline float distanceTo(const Point2D &p2) const
     \brief Compute the distance between the point and a reference one.
     \param[in] &p2 Address of the reference point.
     \return Distance.
    */
    inline float distanceTo(const Point2D &p2) const {
        return sqrt(distanceSquaredTo(p2));
    }

    /*! \fn inline float distanceSquaredTo(const Point2D &p2) const
     \brief Compute the quadratic distance between the point and a reference one.
     \param[in] &p2 Address of the reference point.
     \return Quadratic distance.
    */
    inline float distanceSquaredTo(const Point2D &p2) const {
        return (myX-p2.myX)*(myX-p2.myX) + (myY-p2.myY)*(myY-p2.myY);
    }


    /*! \fn inline float atan() const
     \brief Compute the arc tangent of the point with respect to the origin.
     \return Arc tangent in radians.
    */
    inline float originAtan(void) const {
        return atan(myY/myX);
    }

    /*! \fn inline float shiftedAtan(const Point2D &p2) const
     \brief Compute the arc tangent of the point with respect to a point (considered as reference).
     \param[in] &p2 Address of the reference point.
     \return Referred arc tangent in radians.
    */
    inline float shiftedAtan(const Point2D &p2) const {
        return atan((myY-p2.myY)/(myX-p2.myX));
    }

private:
    float myX;			/**< X coordinate */
    float myY;			/**< Y coordinate */

};


}



#endif /* POINT2D_H_ */
