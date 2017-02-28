//
//  main.cpp
//  Beautiful spheres.
//
//  Created by Samuel Ngwarai on 12/1/16.
//  Copyright © 2016 Samuel Ngwarai. All rights reserved.
//


//This program generates spheres using ray-casting and prints them out in a ppm file at a set resolutions.


#include "declarations.h"

                                                                //defintes the maximum ray depth
#define MAX_RAY_DEPTH 2

using namespace std;


double ipr1 = 1;
double ipg1 = 1;
double ipb1 = 1;

double iar = 0.3;
double iag = 0.5;
double iab = 0.9;



                                                                //for normalizing a vector
Vector normalize(Vector &a) {
    double xx = 0, yy = 0, zz = 0;
    xx = (a.x * a.x);
    yy = (a.y * a.y);
    zz = (a.z * a.z);
    double total = xx + yy + zz;
    double distance = sqrt(total);
    a = a * (1/distance);
    return a;
}


//---------------------------------------------------------------------------
//
//THE TRACE FUNCTION
//

Vector trace(
             Vector &viewpoint,
             Vector &direction,
             std::vector<Sphere> &spheres,
             int depth) {
    
    
    float tnear = INFINITY;
    Sphere* sphere = NULL;                                      //pointer to our chosen sphere
    
    for (int i = 0; i < spheres.size(); i++) {
        double t0 = INFINITY, t1 = INFINITY;
        
                                                                //FINDING THE CLOSEST INTERSECTION AND SPHERE RESPONIBLE
        if (spheres[i].intersect(direction, viewpoint, spheres[i].center, spheres[i].radius, t0, t1)) {
            if (t0 < 0) t0 = t1;
            if (t0 < tnear) {
                tnear = t0;
                sphere = &spheres[i];
            }
        }
    }
    
    if (!sphere) return Vector(1,1,1);                          //Setting background color
    
    Vector color(0);                                            //initializing the color vector
    
    Vector intersect;
    intersect = viewpoint + (direction * tnear);                //getting point of intersection
    
    Vector normal;
    normal = intersect - sphere->center;
    normal = normalize(normal);                                 //getting normal at that point
    
    bool inside = false;                                        //to see if we are inside a sphere
    if (direction.product(intersect) > 0) {
        inside = true;
    }
    
    

    
    
    if ((sphere->refraction > 0 || sphere->reflection > 0) && depth < MAX_RAY_DEPTH) {

        Vector refldir = direction - normal * 2 * direction.product(normal);
        refldir = normalize(refldir);
        Vector reflection(0,0,0);
        
        
        reflection = trace(intersect , refldir, spheres, depth + 1);        //CALLING TRACE ON REFLECTED RAY
        normal = normalize(normal);
        
        
        Vector refraction;
                                                                // if the sphere is also transparent compute refraction ray (transmission)
        if (sphere->refraction) {
            float ior = 1.1, eta = (inside) ? ior : 1 / ior;    // are we inside or outside the surface?
            float cosi = -normal.product(direction);
            float k = 1 - eta * eta * (1 - cosi * cosi);
            Vector refrdir = direction * eta + intersect * (eta *  cosi - sqrt(k));
            refrdir = normalize(refrdir);
            refraction = trace(intersect, refrdir, spheres, depth + 1);
        }
        
                                                                // the result is a mix of reflection and refraction (if the sphere is transparent)
        color = (
                 reflection * sphere->reflection +
                 refraction * sphere->refraction +
                 sphere->ambience * (1 - sphere->reflection - sphere->refraction));
    }
    
    
    //--------------------------------
    //
    //IF NOT REFLECTING OR REFRACTING
    
    else {
        Vector lightsource(-6.5, 0, 12);                         //the poisition of the lightsource
        Vector halfway;
        Vector nlight;
        Vector lightdirection;
        
                                                                //calculating the attenuation
        double att1;
        double d_att1;
        d_att1 = lightsource.distance(intersect);
        att1 = 1.0/(0.2 + (0.8 * d_att1) + (d_att1 * d_att1 * 0.5));
        att1 = min(1.0,att1);
        
        
        nlight = lightsource - intersect;
        lightdirection = normalize(nlight);
        
        Vector view;
        view = intersect - viewpoint;
        view = normalize(view);
        
        halfway = lightdirection + view;
        
        Vector nhalfway;
        
        nhalfway = normalize(halfway);                          //normalizing the halfway vector
        
                                                                //calculating the max(n*l, 0) * kd
        double n_l;
        n_l = normal.product(lightdirection);
        double mid1r = sphere->diffuse.x * max(n_l, 0.0);
        double mid1g = sphere->diffuse.y * max(n_l, 0.0);
        double mid1b = sphere->diffuse.z * max(n_l, 0.0);
        
                                                                    //calculating the max(h*n, 0) * ks
        double n_h;
        n_h = normal.product(nhalfway);
        double mid2r = sphere->specular.x * pow(max(n_h, 0.0),sphere->shine);
        double mid2g = sphere->specular.y * pow(max(n_h, 0.0),sphere->shine);
        double mid2b = sphere->specular.z * pow(max(n_h, 0.0),sphere->shine);
        
                                                                //adding the max(h*n, 0) * ks and max(n*l, 0) * kd
        double mid3r = mid2r + mid1r;
        double mid3g = mid2g + mid1g;
        double mid3b = mid2b + mid1b;
        
                                                                //calculating att* Ip values
        double mid4r = ipr1 * att1;
        double mid4g = ipg1 * att1;
        double mid4b = ipb1 * att1;
        
                                                                //adding all it all up
        double intensity_r = (iar * sphere->ambience.x) + mid4r * mid3r;
        double intensity_g = (iag * sphere->ambience.x) + mid4g * mid3g;
        double intensity_b = (iab * sphere->ambience.x) + mid4b * mid3b;
        
        double total_r = 0, total_g = 0, total_b = 0;
        total_r += intensity_r;
        total_g += intensity_g;
        total_b += intensity_b;
        Vector temp(total_r, total_g, total_b);
        
        color = color + temp;
        
                                                                //Vector color(total_r, total_g, total_b);
    }
    return color;
}

void render  (std::vector<Sphere> &spheres) {
    
    cout << "Creating screen and viewpoint" << endl;
                                                                //declaring the viewpoint
    Vector viewpoint(0, 0, -2);
    
                                                                //declaring the screen coordinates
    Vector topleft(-1, 1, 0);
    Vector topright(1, 1, 0);
    Vector bottomleft(-1, -1, 0);
    Vector bottomright(1, -1, 0);
    
                                                                //the resolution of our screen
    double x = 1500, y = 1500;
    
    double unitx = topright.distance(topleft) / x;
    double unity = topright.distance(bottomright) / y;
    
                                                                //opening the bitmap file we will use to store our data
    std::ofstream ofs("coolspheres.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << x << " " << y << "\n255\n";
    
    cout << "Printing your new image, this will take a few seconds" << endl;
    for (double i = 0; i < 2; i += unitx) {
        for (double j = 0; j < 2; j += unity) {
                                                                //The point at that pixel;
            Vector pixel ((-1 + i),( -1 + j), 0);
            
                                                                //The direction from the viewpoint to that point. It will look like a point
            Vector direction;
            direction = pixel - viewpoint;
            
            Vector color;
            int depth;
            color = trace(viewpoint, direction, spheres, depth);
            char r = 255 * color.x;
            char g = 255 * color.y;
            char b = 255 * color.z;
            ofs << r << g << b;
            
        }
    }
    std::cout << "Finished iterating. Look for your new image at 'coolspheres.ppm'" << std::endl;
    ofs.close();
    
    
}



int main(int argc, const char * argv[]) {
    
    //declaring the spheres
    std::vector<Sphere> spheres;
    
    //                       center             radius      ambience              specular              diffuse   shine     reflection refration
    spheres.push_back(Sphere(Vector (0,4,12),        3,     Vector (0.4,0.5,0.9),    Vector (1,1,1), Vector (1,1,1), 530,       1,        0));
    spheres.push_back(Sphere(Vector (3,-2,10),       2,     Vector (0.4,0.4,0.9),    Vector (1,1,1), Vector (1,1,1), 10,        0,        0.7));
    spheres.push_back(Sphere(Vector (-3,0,10),       2,     Vector (0.4,0.4,0.9),    Vector (1,1,1), Vector (1,1,1), 10,        0,        0));
    spheres.push_back(Sphere(Vector( 0, -105, 15), 100,     Vector (0,0,0),        Vector (1,1,1), Vector (1,1,1), 10,        0.7,      0));
    
    render(spheres);
    
    return 0;
}
