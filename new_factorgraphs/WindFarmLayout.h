#ifndef __WINDFARMLAYOUT_H_INCLUDED__
#define __WINDFARMLAYOUT_H_INCLUDED__

#include <iostream> 
#include <fstream>
#include <string>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include "armadillo"

#define PI 3.1415926535897932 // less expensive than acos(-1)

using namespace std;
using namespace arma;

class WindFarmLayout {

	public:
		const char *layout_file;
		const char *turbine_coordinates_file;
		const char *wind_resource_file_path;
		arma::mat Layout_Matrix; 
		arma::mat turbine_coordinates; 
		double total_farm_power;
		int num_coordinates;
		arma::mat constraints_coordinates; 

		//DRomero variables
		arma::vec coordinates;
		arma::mat Z; //turbine heights
		arma::mat powerTable; //turbine data
		arma::mat Ct; //turbine data
		arma::mat DRot; //rotor diameters
		arma::vec windDirections; //wind directions 
		arma::mat windSpeeds; //wind speeds
		arma::mat windProb; //wind probabilities
		arma::mat Z0; //terrain data

		arma::vec turbinePowers;
		arma::vec turbineEffectiveWindSpeeds;
		double farmEfficiency;
		int numT;
		double Proximity;
		double AEP;
		double SPL;
		

	WindFarmLayout(const char *wind_farm_layout_file_path, const char *turbine_coordinates_file_path, const char *wind_resource_file_path, string fcoordinates,
											string fwindDirections, string fwindSpeeds, string fwindProb, string fZ,
											string fCt, string fDRot, string fZ0, string fpowerTable);

	//SET Functions
	void setWindFarmLayoutFile (const char *wind_farm_layout_file_path);
	void setWindFarmTurbineCoordinatesFiles (const char *turbine_coordinates_file_path); 
	void setWindResourceFile (const char *wind_resource_file_path);

	//READ Functions
	void readWindFarmLayoutFile ();
	void readTurbineCoordinatesFile ();

	//GET Functions
	const char *getWindFarmLayoutFile ();
	const char *getWindTurbineCoordinatesFile ();
	const char *getWindResourceFile ();
	arma::mat getLayoutMatrix ();
	arma::vec getTurbineCoordinates ();
	double getTotalFarmPower ();
	arma::mat getConstraintsCoordinates();	
	double getPower(double v);
	double getCt(double v);

	//Calculate Function
	double calculateFarmPower (arma::vec someTurbineCoordinates);	
	void validate();

};

#endif
