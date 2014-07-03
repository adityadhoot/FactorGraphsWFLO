#ifndef __WINDFARM_H_INCLUDED__
#define __WINDFARM_H_INCLUDED__

typedef struct {
	std::string site_description;
	double x,y,z,h,wa,wk,pd;
	int sectors;
	double f_wa_wk [12][3];
} WindResourceData;

typedef struct {
	arma::vec windDirection; //wind directions
	arma::vec windSpeed; //wind speeds
	arma::vec windProb; //wind probabilities
} windata;

class windfarm {

public:

	//Constructors
	windfarm (const char *WRF_Path); //For use with WRF File
	windfarm (const char *WFL_Path, 
							const char *TCoors_Path, 
							const char *WRF_Path, 
							std::string fcoordinates,
							std::string fwindDirections, 
							std::string fwindSpeeds, 
							std::string fwindProb,
							std::string fZ,
							std::string fCt, 
							std::string fDRot, 
							std::string fZ0, 
							std::string fpowerTable);
	

	//SET Functions
	void setWindFarmLayoutFile (const char *WFL_Path);
	void setWindFarmTurbineCoordinatesFiles (const char *TCoors_Path); 
	void setWindResourceFile (const char *WRF_Path);
		
	//READ Functions
	void readWindFarmLayoutFile ();
	void readTurbineCoordinatesFile ();
	void readWindResourceFile ();

	//GET Functions
	const char *getWindFarmLayoutFile ();
	const char *getWindTurbineCoordinatesFile ();
	const char *getWindResourceFile ();
	arma::mat getLayoutMatrix ();
	arma::vec getTurbineCoordinates ();
	double getTotalFarmPower ();
	

	//Processing
	double calculateFarmPower (arma::vec someTurbineCoordinates);
	void validate ();






	//DRomero variables for calculating Energy/Power/Farm Efficiency
	arma::vec coordinates;
	arma::mat Z; //turbine heights
	arma::mat powerTable; //turbine data
	arma::mat Ct; //turbine data
	arma::mat DRot; //rotor diameters
	arma::vec windDirections; //wind directions 
	arma::mat windSpeeds; //wind speeds
	arma::mat windProb; //wind probabilities
	arma::mat Z0; //terrain data

	arma::vec turbinePowers; //Powers generated by each turbine
	arma::vec turbineEffectiveWindSpeeds;
	double farmEfficiency;
	int numtT; //number of turbines
	double Proximity;
	double AEP;
	double SPL;

	//Processing
	void convertWindDataToContinuous ();
	
	//GET
	windData (double x, double y);
	


};

#endif