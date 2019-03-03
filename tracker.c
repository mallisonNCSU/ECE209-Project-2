/*
Author: Michelle Allison
Description: This program, given a time, an x coordinate, and a y coordinate per each movement, tracks the movement of a point along 2 roads on a coordinate plane, similar to the way a GPS would. It keeps track of when the point has changed direction, when it turns, what its peak speed is, what the total distance travelled is, and what road the point started on and ended on. 

I prefer the Virtual Box for grading.
*/

#include <stdio.h>
#include <math.h>

#ifndef SECRET_TESTS
double M1=-1.0, B1=0; // M1 * x + B1 = y -- road 1
double M2=1.0, B2=0; // M2 * x + B2 = y -- road 2
#else
double M1=SECRET_M1, B1=SECRET_B1;
double M2=SECRET_M2, B2=SECRET_B2;
#endif

// global variables, most are what are shown at the end of the program, self-explanatory:
int Road=1, First_road=1;
int Changed_direction=0;
int Changed_road=0;
double Total_distance=0;
double Peak_speed=0.0;
double prior_delta = 0.0; // prior difference between x and prior x, for seeing if point changed direction
// Function declarations, self explanatory (more explained at functions' definitions):
void track(double x, double y, double time);
double get_y(double x, int road);
double intercept(void);
double distance(double x1, double y1, double x2, double y2);
double nearest_point_on_road(double x, double y, int road);
int closest_road(double x, double y, int road);
int first_closest_road(double x, double y);

int main () {

	double time, x, y;
	int ret;

	do {
	// get time,x,y coordinate from stdin
	ret = scanf("%lf%lf%lf",&time,&x,&y);

		if (ret==3)
		{
			// call into the tracking algorithm
			track(x,y,time);
		}

		// if we didn't get three doubles, time to quit
	} while(ret==3);

	// print out final results
	printf("Total distance = %.2lf\n",Total_distance);
	printf("Peak speed = %.2lf\n",Peak_speed);
	printf("# of changes in direction = %d\n",Changed_direction);
	printf("# of changes in road = %d\n",Changed_road);
	printf("First Road = %d, Last Road = %d\n",First_road,Road);

	return 0;
}

void track(double x, double y, double time) { // track keeps track of all the variables presented at the end of the program
	static int count = 0; // how many times track has been called
	static double prior_x; // prior x, used for prior_delta and seeing if point changed direction
	double current_speed = 0; //used for comparing to Peak_speed
	double current_delta = 0; //used for comparing with prior_delta
	double current_distance = 0; //no need to copy and paste long distance() calls
	count++;
	if (count == 1) { //if first time track is called, can't compute distance/direction/speed/etc.
		First_road = first_closest_road(x,y);
		Road = First_road;
		prior_x = nearest_point_on_road(x,y,Road);	
	} else { //keeping track of all the global variables printed at the end of the program
		if (closest_road(x,y,Road) == Road) { //if the point didn't turn, do this
			current_distance = distance(prior_x, get_y(prior_x, Road), nearest_point_on_road(x,y,Road), get_y(nearest_point_on_road(x,y,Road),Road));
			current_delta = prior_x - x;
			if ((prior_delta/current_delta) < 0) { //if the point reversed, do this
				Changed_direction++;
			}
			prior_delta = current_delta;
		} else { // if the point did turn, do this
			Changed_direction++;
			Changed_road++;
			current_distance = distance(prior_x,get_y(prior_x,Road),intercept(),get_y(intercept(),Road)) + distance(intercept(),get_y(intercept(),Road), nearest_point_on_road(x,y,closest_road(x,y,Road)), get_y(nearest_point_on_road(x,y,closest_road(x,y,Road)),closest_road(x,y,Road)));
			Road = closest_road(x,y,Road);
		} //track needs to do this whether the point turned or not
			prior_x = nearest_point_on_road(x,y,Road);
			current_speed = current_distance / time;
			Total_distance += current_distance;
			if (current_speed > Peak_speed) {
				Peak_speed = current_speed;
			}
	}
}

double distance(double x1, double y1, double x2, double y2) { //computes distance between 2 points
return sqrt(pow(x1-x2,2.0) + pow(y1-y2,2.0));
}

double get_y(double x, int road) { //gets the y coordinate given x and the road (line)
	if (road==1){
		return ((M1*x)+B1);
	}
	if (road==2){
		return ((M2*x)+B2);
	} else {
		printf("An error occured in a get_y function call.\n");
		return 0.0;
	}
}

double intercept(void) { //gets the x coordinate of the intercept of the two roads
	return ((B2-B1)/(M1-M2));
}

double nearest_point_on_road(double x, double y, int road){ //gets nearest point to a given road given an x and y coordinate
	if (road==1) {
		return ((y+(x*(1/M1))-B1)/(M1+(1/M1)));
	}
	if (road==2) {
		return ((y+(x*(1/M2))-B2)/(M2+(1/M2)));
	} else {
		printf("An error occured in a nearest_point_on_road function call.\n");
		return 0.0;
	}
}

int closest_road(double x, double y, int road) { //computes the road that is closest to a given x and y coordinate, though comparing it with the road the point is currently on.
	int other = 0; // getting x and y coordinates of closest points on roads 1 and 2
	double x_other = 0;
	double dist_other = 0;
	double x_current = nearest_point_on_road(x,y,road);
	double dist_current = 0;
	if (road == 1){
		other = 2;
		x_other = nearest_point_on_road(x,y,other);
	}
	if (road == 2){
		other = 1;
		x_other = nearest_point_on_road(x,y,other);
	}

	dist_current = distance(x,y,x_current,get_y(x_current,road));
	dist_other = distance(x,y,x_other,get_y(x_other,other));

	if (dist_current > dist_other) { // seeing if the road is different or not
		if (dist_current < 0.0001) { // if the distance is 0.0001 close enough that just stay on same road
			return road;
		} else {
			return other;
		}
	}
	if (dist_current < dist_other) {
		return road;
	}
	if (dist_current == dist_other) { //if the distances are the same, just stay on same road
		return road;
	}
	printf("An error occured in a closest_road function call.\n");
	return 0;
}

int first_closest_road(double x, double y) { //basically closest_road but isn't given a current road; this function is only used at the first call of track
	double x_1 = nearest_point_on_road(x,y,1); // getting x and y coordinates of closest points on roads 1 and 2
	double x_2 = nearest_point_on_road(x,y,2);
	double dist_1 = distance(x,y,x_1,get_y(x_1,1));
	double dist_2 = distance(x,y,x_2,get_y(x_2,2));

	if (dist_1 > dist_2) {
		return 2;
	}
	if (dist_1 < dist_2) {
		return 1;
	}
	if (dist_1 == dist_2) {
		return 1;
	}
	printf("An error occured in a first_closest_road function call.\n");
	return 0;
}









