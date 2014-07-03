#include "WindFarmLayout.h"
using namespace std;
using namespace arma; 

WindFarmLayout::WindFarmLayout(const char *wind_farm_layout_file_path, const char *turbine_coordinates_file_path, const char *wind_resource_file_path, string fcoordinates,
																	string fwindDirections, string fwindSpeeds, string fwindProb, string fZ,
																	string fCt, string fDRot, string fZ0, string fpowerTable){

	layout_file = wind_farm_layout_file_path;
	turbine_coordinates_file = turbine_coordinates_file_path;
	this->wind_resource_file_path = wind_resource_file_path;


	arma::vec coordinates;
	arma::mat Z; //turbine heights
	arma::mat powerTable; //turbine data
	arma::mat Ct; //turbine data
	arma::mat DRot; //rotor diameters
	arma::vec windDirections; //wind directions 
	arma::mat windSpeeds; //wind speeds
	arma::mat windProb; //wind probabilities
	arma::mat Z0; //terrain data
	
	coordinates.load (fcoordinates);
	Z.load (fZ);
	powerTable.load (fpowerTable);
	Ct.load (fCt);
	DRot.load (fDRot);
	windDirections.load (fwindDirections);
	windSpeeds.load (fwindSpeeds);
	windProb.load (fwindProb);
	Z0.load (fZ0);

	this->coordinates = coordinates;
	this->Z = Z;
	this->powerTable = powerTable;
	this->Ct = Ct;
	this->DRot = DRot;
	this->windDirections = windDirections;
	this->windSpeeds = windSpeeds;
	this->windProb = windProb;
	this->Z0 = Z0; 

	//Other Calculations
	this->turbinePowers = arma::zeros<vec>(coordinates.n_elem/2);
	this->turbineEffectiveWindSpeeds = arma::zeros<vec>(coordinates.n_elem/2);
	this->farmEfficiency = 0.0;
	this->numT = this->coordinates.n_elem /2;
	this->Proximity = 5 * DRot(0,0);

	this->AEP = 0.0;
	this->SPL = 0.0;

	validate();

}

/*	const char *layout_file;
	const char *turbine_coordinates_file;
	const char *wind_resource_file_path;
	arma::mat Layout_Matrix;
	int num_coordinates;
	arma::mat turbine_coordinates; 
	double total_farm_power;
*/


//SET Functions
void WindFarmLayout::setWindFarmLayoutFile (const char *wind_farm_layout_file_path){
	layout_file = wind_farm_layout_file_path;
}

void WindFarmLayout::setWindFarmTurbineCoordinatesFiles (const char *turbine_coordinates_file_path){
	turbine_coordinates_file = turbine_coordinates_file_path;
}

void WindFarmLayout::setWindResourceFile (const char *wind_resource_file_path){
	this->wind_resource_file_path = wind_resource_file_path;
}

//READ Functions
void WindFarmLayout::readWindFarmLayoutFile (){
	std::ifstream f_in;
	f_in.open(layout_file);
	std::string line;
	int count = 10;
	Layout_Matrix.set_size (count,count);
	constraints_coordinates.set_size (10, 2); 
	int constraints = 0;

	for (int i=0;i<count;i++){
		getline (f_in,line);
		for (int j=0;j<line.length();j++){
			Layout_Matrix (i,j) = (int)(line[j]) - 48;
			if (((int)(line[j]) - 48)==1){
				constraints_coordinates (constraints,0)= i;
				constraints_coordinates (constraints,1) = j;
				constraints = constraints + 1;
			}
		}
	}
}

void WindFarmLayout::readTurbineCoordinatesFile (){
	std::ifstream f_in;
	f_in.open(turbine_coordinates_file);
	std::string line;
	int count = 0;

	while (getline(f_in, line)){
		count = count + 1;
	}

	num_coordinates = count;
	f_in.close ();
	turbine_coordinates.set_size (count, 2);
	f_in.open(turbine_coordinates_file);

	for (int i=0;i<count;i++){
		getline (f_in, line);
		int pos = line.find(',');
		std::string xc, yc;
		xc = line.substr (0,pos);
		yc = line.substr (pos+1, line.length()-1);
		int x_c, y_c;
		x_c = atoi(xc.c_str());
		y_c = atoi(yc.c_str());
		turbine_coordinates (i,0) = x_c;
		turbine_coordinates (i,1) = y_c;
	}

	f_in.close ();
}

//GET Functions
const char* WindFarmLayout::getWindFarmLayoutFile (){
	return layout_file;
}

const char* WindFarmLayout::getWindTurbineCoordinatesFile (){
	return turbine_coordinates_file;
}

const char* WindFarmLayout::getWindResourceFile (){
	return wind_resource_file_path;
}

arma::mat WindFarmLayout::getLayoutMatrix (){
	return Layout_Matrix;
}

arma::vec WindFarmLayout::getTurbineCoordinates (){
	return coordinates;
}

double WindFarmLayout::getTotalFarmPower (){
	return total_farm_power;
}

arma::mat WindFarmLayout::getConstraintsCoordinates(){
	return  constraints_coordinates;
}	

//Calculate Function
double WindFarmLayout::calculateFarmPower (arma::vec someTurbineCoordinates){
//initialization
	unsigned numT = someTurbineCoordinates.n_elem / 2;
	double totalPower = 0.0;
	double upperbound = 0.0;
	//TODO: Make this use a different height for each turbine
	//double alpha = 0.5 / log(z/z0); // assume turbine height is constant for now

	//TODO: Check, once again, that all wake calculations are correct.


	// dromero
	// For calculation of this->turbineEffectiveWindSpeeds
	mat tEffSpeed = zeros<mat>(numT, this->windDirections.n_elem);
	mat effPower = zeros<mat>(numT, this->windDirections.n_elem);

	for (unsigned w = 0; w < windDirections.n_elem; w++) // each wind direction has one ROW in windSpeeds and windProb
			{
		double theta = windDirections(w);

		cube downstream = ones(numT, numT, windSpeeds.n_cols) * -1; //1 if i is in downstream of j, 0 otherwise, -1 if not initialized

		cube overlap = zeros(numT, numT, windSpeeds.n_cols); // 1 if i is downstream and completely in the wake of j; 0 if i is not in the wake of j

		for (unsigned i = 0; i < numT; i++) {
			// dromero
			// use a turbine-specific rotor diameter
			double r0i = this->DRot(i) / 2;

			for (unsigned j = 0; j < numT; j++) {
				// dromero
				// calculates wake expansion angle based on this turbine's height
				double alpha = 0.5 / log(this->Z(j) / this->Z0(j));
				// use a turbine-specific rotor diameter
				double r0j = this->DRot(j) / 2;

				for (unsigned l = 0; l < windSpeeds.n_cols; l++) {
					if (downstream(i, j, l) == -1) //not already initialized
							{
						/*
						 \      /
						 \  * /<-- centre of turbine i
						 \__/ <-- turbine j
						 \/
						 */
						downstream(i, j, l) = 0;

						if (i != j) {
							double al = atan(alpha); //half angle of the wake spread
							double a = 1 - sqrt(1 - getCt(windSpeeds(w, l)));

							//TODO: Use Ct-dependent wake radius
							// If considering Ct-dependent wake radius immediately behind turbine, use the first method; otherwise the second one is closer with OpenWind results
							double r;
#ifdef CT_DEPENDENT
								r = r0j * (1 - 0.5 * a) / (1 - a); // radius of wake immediately behind turbine
#else
								r = r0j;
#endif

							vec pt1(2); // coordinates of the "left" end of initial wake cone
							vec pt2(2); // coordinates of the "right" end of initial wake cone
							pt1(0) = someTurbineCoordinates(j)
									+ r * cos(theta + PI / 2);
							pt2(0) = someTurbineCoordinates(j)
									+ r * cos(theta - PI / 2);
							pt1(1) = someTurbineCoordinates(j + numT)
									+ r * sin(theta + PI / 2);
							pt2(1) = someTurbineCoordinates(j + numT)
									+ r * sin(theta - PI / 2);

							vec pt3(2); // coordinates of the "left" end of turbine i
							vec pt4(2); // coordinates of the "right" end of turbine i
							pt3(0) = someTurbineCoordinates(i)
									+ r0i * cos(theta + PI / 2);
							pt4(0) = someTurbineCoordinates(i)
									+ r0i * cos(theta - PI / 2);
							pt3(1) = someTurbineCoordinates(i + numT)
									+ r0i * sin(theta + PI / 2);
							pt4(1) = someTurbineCoordinates(i + numT)
									+ r0i * sin(theta - PI / 2);

							int num_dtPoints = 10;
							vec dtPointsInWake = zeros(num_dtPoints);
							for (int s = 0; s < num_dtPoints; s++) {
								vec dtPoint(2); // one point on the rotor of downstream turbine i
								dtPoint(0) = (pt3(0) - pt4(0)) * s
										/ num_dtPoints + pt4(0);
								dtPoint(1) = (pt3(1) - pt4(1)) * s
										/ num_dtPoints + pt4(1);
								vec r1(2); //vector pointing from pt1 to the point on turbine i
								vec r2(2);
								r1(0) = dtPoint(0) - pt1(0);
								r1(1) = dtPoint(1) - pt1(1);
								r2(0) = dtPoint(0) - pt2(0);
								r2(1) = dtPoint(1) - pt2(1);

								vec r1_hat = r1 / norm(r1, 2); // normalized r1
								vec r2_hat = r2 / norm(r2, 2);

								double halfangle = (al + PI / 2) / 2;

								vec pt1_c(2); // centre line at pt1 between wake left boundary and turbine
								vec pt2_c(2);
								pt1_c(0) = cos(theta - PI / 2 + halfangle);
								pt1_c(1) = sin(theta - PI / 2 + halfangle);
								pt2_c(0) = cos(theta + PI / 2 - halfangle);
								pt2_c(1) = sin(theta + PI / 2 - halfangle);

								if (pt1_c(0) * r1_hat(0) + pt1_c(1) * r1_hat(1)
										>= cos(halfangle)
										&& pt2_c(0) * r2_hat(0)
												+ pt2_c(1) * r2_hat(1)
												>= cos(halfangle))
												//if (dot(pt1_c, r1_hat) >= cos(halfangle) && dot(pt2_c, r2_hat) >= cos(halfangle))
														{
									downstream(i, j, l) = 1;
									dtPointsInWake(s) = 1;
								}
							}
							overlap(i, j, l) = (double) sum(dtPointsInWake)
									/ num_dtPoints;
						}
					}
				}
			}
		}

		mat effSpeed(numT, windSpeeds.n_cols); //stores final effective wind speed at i
		mat defSumSqr = zeros(numT, windSpeeds.n_cols); // sum of the sqr of deficits (0~1) from multiple wakes

		// Calculate the total power produced by turbine i
		// First, calculate the total wake effects on turbine i
		// Second, calculate the resulting wind speed and power output
		for (unsigned i = 0; i < numT; i++) {

			for (unsigned j = 0; j < numT; j++) {
				// dromero
				// use a turbine-specific rotor diameter
				double r0j = this->DRot(j) / 2;
				// calculates wake expansion angle based on this turbine's height
				double alpha = 0.5 / log(this->Z(j) / this->Z0(j));

				for (unsigned l = 0; l < windSpeeds.n_cols; l++) {
					if (j != i && downstream(i, j, l) == 1) // if downstream and different turbines
							{
						double dd = (someTurbineCoordinates(i)
								- someTurbineCoordinates(j)) * cos(theta)
								+ (someTurbineCoordinates(i + numT)
										- someTurbineCoordinates(j + numT))
										* sin(theta);
						double a = 1 - sqrt(1 - getCt(windSpeeds(w, l)));

						// If considering Ct-dependent wake radius immediately behind turbine, use the first method; otherwise the second one is closer with OpenWind results
						double r;
#ifdef CT_DEPENDENT
							r = r0j * (1 - 0.5 * a) / (1 - a); // radius of wake immediately behind turbine
#else
							r = r0j;
#endif

						double deficit = (1 - sqrt(1 - getCt(windSpeeds(w, l))))
								* pow(r / (r + alpha * dd), 2.0)
								* overlap(i, j, l); // the Modified Park way
						defSumSqr(i, l) += pow(deficit, 2);
					}
				}
			}
		}

		for (unsigned l = 0; l < windSpeeds.n_cols; l++) {
			for (unsigned i = 0; i < numT; i++) {
				effSpeed(i, l) = windSpeeds(w, l) * (1 - sqrt(defSumSqr(i, l)));
				totalPower += getPower(effSpeed(i, l)) * windProb(w, l);
				upperbound += getPower(windSpeeds(w, l)) * windProb(w, l);
				effPower(i, w) += getPower(effSpeed(i, l)) * windProb(w, l);
				tEffSpeed(i, w) += effSpeed(i, l) * windProb(w, l);
			}

		}

	} // END of for over wind directions

	// dromero
	// Calculates the effective wind speed at each turbine for each wind direction w, as an expected value using windProb
	// Save Effective wind speeds for each turbine, so that we can use them for noise calculations.
	// Note that there is a probability for each wind speed and direction. To calculate this, we are using the marginal probability for wind direction, i.e., we sum up the probabilityes across the wind speeds.
	this->turbineEffectiveWindSpeeds = sum(tEffSpeed, 1);
	this->turbinePowers = sum(effPower, 1) * 8766 / 1000000;
	this->AEP = sum(this->turbinePowers);

	// Stores the calculated efficiency in this instance of the object
	this->farmEfficiency = totalPower / upperbound;

	//cout << "Total power: " << totalPower << " (" << farmEfficiency * 100 << "%)" << endl;
	//cout << "Effective Wind Speeds at each Turbine" << endl << turbineEffectiveWindSpeeds;

	if (farmEfficiency == 1) {
	//	cout << "Total power: " << totalPower << " (" << farmEfficiency * 100 << "%)" << endl;
	}	
	return ((this->farmEfficiency)*100);
	

}

void WindFarmLayout::validate(){

	unsigned numT = this->numT;

	if (this->Z.n_elem == 1) {
		arma::vec t = arma::ones<vec>(numT);
		t.fill(this->Z(0));
		this->Z = t;
	}

	if (this->DRot.n_elem == 1) {
		arma::vec t = arma::ones<vec>(numT);
		t.fill(this->DRot(0));
		this->DRot = t;
	}

	if (this->Z0.n_elem == 1) {
		arma::vec t = arma::ones<vec>(numT);
		t.fill(this->Z0(0));
		this->Z0 = t;
	}
}


double WindFarmLayout::getPower(double v){
	
	for (unsigned i = 0; i < powerTable.n_rows - 1; i++)
		if (powerTable(i, 0) <= v && v <= powerTable(i + 1, 0)) {
			return powerTable(i, 1)
					+ (powerTable(i + 1, 1) - powerTable(i, 1))
							* (v - powerTable(i, 0))
							/ (powerTable(i + 1, 0) - powerTable(i, 0)); // linear interpolation
		}
	
	return 0;
}

double WindFarmLayout::getCt(double v) {
	for (unsigned i = 0; i < Ct.n_rows - 1; i++)
		if (Ct(i, 0) <= v && v <= Ct(i + 1, 0))
			return Ct(i, 1)
					+ (Ct(i + 1, 1) - Ct(i, 1)) * (v - Ct(i, 0))
							/ (Ct(i + 1, 0) - Ct(i, 0)); // linear interpolation

	return 1;
}


