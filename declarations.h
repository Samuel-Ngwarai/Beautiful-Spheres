//
//  declarations.h
//  Beautiful spheres
//
//  Created by Samuel Ngwarai on 12/1/16.
//  Copyright © 2016 Samuel Ngwarai. All rights reserved.
//

//this file serves as the declaration of the Vectors and Spheres that will be used in the main function.

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>

using namespace std;
#ifndef declarations_h
#define declarations_h

//A Vector class to handle all the Vectors and points
class Vector {
public:
    double x;
    double y;
    double z;
    
    //different ways to declare the vector variables
    Vector () {};
    ~Vector() {};
    Vector (double a, double b, double c){x = a; y = b; z = c;}
    Vector (double a){x = a; y = a; z = a;}
    
    //to print the position of the point/vector
    void print(){cout << "x: " << x << " y: " << y << " z: " << z << endl;}
    
    //to return the z co-ordinate
    double getz() {return z;}
    
    //overwritting the - operator
    Vector operator- (const Vector &a) {
        Vector newp;
        newp.x = this->x - a.x;
        newp.y = this->y - a.y;
        newp.z = this->z - a.z;
        return newp;
    }
    //overwritting the + operator
    Vector operator+ (const Vector &a) {
        Vector newp;
        newp.x = this->x + a.x;
        newp.y = this->y + a.y;
        newp.z = this->z + a.z;
        return newp;
    }
    
    //overwritting the = operator
    Vector& operator = (const Vector &a) {
        x = a.x;
        y = a.y;
        z = a.z;
        return *this;
    }
    
    //overwriting the * operator for multiplication
    Vector operator* (double a) {
        Vector newp;
        newp.x = this->x * a;
        newp.y = this->y * a;
        newp.z = this->z * a;
        return newp;
    }
    
    //overwritting the << operator
    friend ostream& operator<<(ostream& out, const Vector &z) {
        out << "x: " << z.x << " y: " << z.y << " z: " << z.z << endl;
        return out;
    }
    
    //getting the distance between two points in space
    double distance(Vector &a) {
        double diffx = a.x - this->x;
        double diffy = a.y - this->y;
        double diffz = a.z - this->z;
        return sqrt((diffx * diffx) + (diffy * diffy) + (diffz * diffz));
    }
    
    //getting the dot product of two points
    double product(Vector &a) {
        return ((this->x * a.x) + (this->y * a.y) + (this->z * a.z));
    }
    
};


//Sphere class to handle all our spheres.
class Sphere {
public:
    Vector center;
    double radius;
    
    //light color
    Vector ambience;
    Vector specular;
    Vector diffuse;
    double shine;
    
    double reflection;
    double refraction;
    
    //to allow for spheres in arrays for easier implementation with many spheres
    Sphere () {};
    ~Sphere() {};
    
    //declaring spheres
    Sphere (Vector c, double r, Vector am, Vector sp, Vector di, double shi, double refl, double refr) {
        center = c;
        radius = r;
        ambience = am;
        specular = sp;
        diffuse = di;
        shine = shi;
        reflection = refl;
        refraction = refr;
    }
    
    //The sphere intersect function
    bool intersect (Vector &direction, Vector &viewpoint, Vector &center, double &radius, double &x0, double &x1)
    {
        double a, b, c;
        
        Vector for_b;
        double for_c = 0;
        
        a = direction.product(direction);
        for_b = viewpoint - center;
        b = 2 * (direction.product(for_b));
        for_c = radius * radius;
        c = for_b.product(for_b) - for_c;
        
        float discr = b * b - 4 * a * c;
        if (discr < 0) {
            return false;
        }
        else if (discr == 0) {
            x0 = x1 = - 0.5 * b / a;
        }
        else {
            float q = (b > 0) ?
            -0.5 * (b + sqrt(discr)) :
            -0.5 * (b - sqrt(discr));
            x0 = q / a;
            x1 = c / q;
        }
        return true;
    }
    
};

#endif /* declarations_h */
