//
//  main.cpp
//  USCongressionalDistricts
//
//  Created by Jesus Orduna on 3/10/15.
//  Copyright (c) 2015 Brown University. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <ctime>
#include <math.h> /* round, tan */

int main(int argc, const char * argv[]) {
    int pictureWidth, pictureHeight;
    double minLongitude, maxLongitude;
    double minLatitude, maxLatitude;
    // Central parallel:
    double alpha_0 = 36.0;
    // Radius, 1.0 shoud be ok...
    double r_0 = 1.0;
    // Central meridian:
    double gamma_0 = -96.0;
    std::ifstream inputParameters("parameters.txt");
    inputParameters >> pictureWidth >> pictureHeight;
    inputParameters >> minLatitude >> maxLatitude;
    inputParameters >> minLongitude >> maxLongitude;
    inputParameters.close();
    
    // This will be used later
    double beta_0 = cos(alpha_0*M_PI/180.0)/tan(alpha_0*M_PI/180.0);
    double rho_0, theta_0;
    
    std::map<std::string, std::string> theCoveredDistricts;
    std::string theDistrict;
    std::string theStatus;
    std::ifstream districtsCovered("covered.txt");
    while (districtsCovered >> theDistrict >> theStatus) {
        theCoveredDistricts[ theDistrict ] = theStatus;
    }
    districtsCovered.close();
    
    double longitudeWindow = maxLongitude - minLongitude;
    double latitudeWindow = maxLatitude - minLatitude;
    double theLongitude, theLatitude;
    bool firstPoint;
    
    // xPx : [ 0, pictureWidth ], left to right
    // yPx : [ 0, pictureHeight ], top to bottom
    double xPx, yPx;
    double previousX = 0.0, previousY = 0.0;
    double deltaX, deltaY;
    
    std::ofstream theSVGFile("theMap.svg");
    theSVGFile << "<?xml version=\"1.0\" standalone=\"no\"?>" << std::endl;
    theSVGFile << "<svg width=\"" << pictureWidth << "px\" height=\"" << pictureHeight << "px\" " ;
    theSVGFile << "viewBox=\"0 0 " << pictureWidth << ' ' << pictureHeight << "\" " ;
    theSVGFile << "xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << std::endl;
    std::time_t result = std::time(nullptr);
    theSVGFile << "<title>" << std::endl;
    theSVGFile << "DC trip 2015 - " << std::asctime(std::localtime(&result)) ;
    theSVGFile << "</title>" << std::endl;
    theSVGFile << "<desc>" << std::endl;
    theSVGFile << "Creator: Jesus Orduna" << std::endl;
    theSVGFile << "CreationDate: " << std::asctime(std::localtime(&result)) ;
    theSVGFile << "</desc>" << std::endl;
    theSVGFile << "<defs></defs>" << std::endl;
    theSVGFile << "<rect id=\"Sea\" x=\"0\" y=\"0\" fill=\"#C8EBFF\" width=\"" << pictureWidth << "\" height=\"" << pictureHeight << "\"/>" << std::endl;
    
    std::ifstream includeFiles("include.txt");
    std::string theFile;
    int scheduled = 0, pending = 0, denied = 0;
    while (includeFiles >> theFile) {
        std::ifstream theDistrictCoordinates(theFile.c_str());
        firstPoint = true;
        theSVGFile << "<a xlink:title=\"" << theFile.substr(4, 5) << "\">" << std::endl;
        theSVGFile << " <path id=\"" << theFile << "\" fill=\"" ;
        if ( theCoveredDistricts.count( theFile ) ) {
            if (theCoveredDistricts[ theFile ] == "Yes" ) {
                scheduled++;
                theSVGFile << "green\" opacity=\"0.5" ;
            } else if (theCoveredDistricts[ theFile ] == "Pending" ) {
                pending++;
                theSVGFile << "yellow\" opacity=\"0.5" ;
            } else {
                denied++;
                theSVGFile << "red\" opacity=\"0.5" ;
            }
        } else {
            theSVGFile << "#FDFCEA" ;
        }
        theSVGFile <<"\" stroke=\"#0978AB\" stroke-width=\"0.25\" d=\"" ;
        double xPxMin = 1.e100;
        while (theDistrictCoordinates >> theLongitude >> theLatitude) {
            rho_0 = r_0*(1.0/tan(alpha_0*M_PI/180.0)-tan((theLatitude-alpha_0)*M_PI/180.0));
            theta_0 = beta_0*(theLongitude - gamma_0)/360.0-M_PI_2;
            xPx = (rho_0*cos(theta_0)-minLongitude)*pictureWidth/longitudeWindow;
            yPx = pictureHeight*(1.0-(rho_0*sin(theta_0)-minLatitude)/latitudeWindow);
            
            if (xPx > 0 && xPx < pictureWidth) {
                if (xPx < xPxMin) { xPxMin = xPx; }
            }
            if (firstPoint) {
                theSVGFile << "M" << xPx << "," << yPx ;
                firstPoint = false;
            } else {
                deltaX = round(1000.*(xPx - previousX))/1000.;
                deltaY = round(1000.*(yPx - previousY))/1000.;
                if( deltaX == 0 && deltaY == 0 ) {
                } else {
                    if (deltaX == 0) {
                        theSVGFile << "v" << deltaY ;
                    } else {
                        if (deltaY == 0) {
                            theSVGFile << "h" << deltaX ;
                        } else {
                            theSVGFile << "l" << deltaX << "," << deltaY ;
                        }
                    }
                }
            }
            previousX = xPx;
            previousY = yPx;
        }
        theSVGFile << "z\"/>" << std::endl;
        theSVGFile << "</a>" << std::endl;
        theDistrictCoordinates.close();
    }
    includeFiles.close();
    
    std::ifstream theInstitutions("listOfInstitutions.txt");
    std::string theInstitution;
    while (!theInstitutions.eof()) {
        theInstitutions >> theLongitude >> theLatitude ;
        getline(theInstitutions, theInstitution);
        rho_0 = r_0*(1.0/tan(alpha_0*M_PI/180.0)-tan((theLatitude-alpha_0)*M_PI/180.0));
        theta_0 = beta_0*(theLongitude - gamma_0)/360.0-M_PI_2;
        xPx = (rho_0*cos(theta_0)-minLongitude)*pictureWidth/longitudeWindow;
        yPx = pictureHeight*(1.0-(rho_0*sin(theta_0)-minLatitude)/latitudeWindow);
        theSVGFile << "<a xlink:href=\"http://maps.google.com/?q=" << theLatitude << ',' << theLongitude << "\" ";
        theSVGFile << "xlink:title=\"" << theInstitution << "\">" << std::endl;
        theSVGFile << " <circle cx=\"" << xPx << "\" cy=\"" << yPx << "\" r=\"2\" fill=\"yellow\" stroke=\"red\" stroke-width=\".5\"/>" << std::endl;
        theSVGFile << "</a>" << std::endl;
    }
    theInstitutions.close();
    theSVGFile << "</svg>" << std::endl;
    theSVGFile.close();
    
    std::cout << " - - Summary - -" << std::endl;
    std::cout << "Confirmed meetings: " << scheduled << std::endl;
    std::cout << "Pending meetings: " << pending << std::endl;
    std::cout << "Denied meetings: " << denied << std::endl;
    return 0;
}
