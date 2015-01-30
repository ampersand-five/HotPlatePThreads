//#include <vector>
#include <array>
#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <math.h>
#include <sys/resource.h>
#include <omp.h>

#define PLATE_SIZE 4096
#define NEUTRAL_TEMP 50

using std::array;


int main()
{

	//vectors one initialized to having all 50's
	//vector<vector<double>> previous_plate (PLATE_SIZE, vector<double>(PLATE_SIZE, NEUTRAL_TEMP));
	double** previous_plate = new double* [PLATE_SIZE];
	double** current_plate = new double* [PLATE_SIZE];
	auto *prev = previous_plate;
	auto *curr = current_plate;

	for(int r = 0; r < PLATE_SIZE; ++r)
	{
		previous_plate[r] = new double [PLATE_SIZE];
		current_plate[r] = new double [PLATE_SIZE];
	}

	std::string file = "hotPlateFinished.txt";
	std::ofstream file_out(file, std::ofstream::out);
	//std::ofstream png_out("hotPlateFinished.png", std::ofstream::out);
	//png_out << "137 80 78 71 13 10 26 10 ";

	//auto initalize_start_time = std::chrono::high_resolution_clock::now();
	//initialize vector to have correct starting temperatures
	//Make them all have 50
	for (int i = 0; i < PLATE_SIZE; ++i)
	{
		for (int j = 0; j < PLATE_SIZE; ++j)
		{
			previous_plate[i][j] = NEUTRAL_TEMP;
		}
	}
	//bottom row get's 100 degrees
	for (int i = 0; i < PLATE_SIZE; ++i)
	{
		previous_plate[PLATE_SIZE - 1][i] = 100;
	}
	//mid section (400,0-330) gets 100 degrees
	for (int i = 0; i < 331; ++i)
	{
		previous_plate[400][i] = 100;
	}
	//single point gets 100 degrees
	previous_plate[200][500] = 100;
	//Top & side bars get 0 degrees
	for (int i = 0; i < PLATE_SIZE; ++i)
	{
		previous_plate[0][i] = 0; //top bar
		previous_plate[i][0] = 0; //left bar
		previous_plate[i][PLATE_SIZE - 1] = 0; //right bar
	}

	//Set current_plate to also have the same setup
	for (int i = 0; i < PLATE_SIZE; ++i)
	{
		for (int j = 0; j < PLATE_SIZE; ++j)
		{
			current_plate[i][j] = previous_plate[i][j];
		}
	}

	//auto initalize_end_time = std::chrono::high_resolution_clock::now();

	//auto init_time = std::chrono::duration_cast<std::chrono::duration<double>>(initalize_end_time - initalize_start_time);
	//file_out << "Initialize time: " << init_time.count() << " seconds\n";

	//Start iterating
	//Start time
	auto start_time = std::chrono::high_resolution_clock::now();

	//Run
	int converged = 0;
	int cycles = 0;
	do
	{
		converged = 0;
		//Determine new plate
		#pragma omp parallel for
		for (int i = 1; i < PLATE_SIZE - 1; ++i)
		{
			for (int j = 1; j < PLATE_SIZE - 1; ++j)
			{
				//new temperature
				//Skip fixed points
				if ((i == 400 && j < 331) || (i == 200 && j == 500))
				{
					//file_out << previous_plate[i][j] << " ";
					//png_out << (int)previous_plate[i][j] << " ";
				}
				else
				{
					//Debug purposes
					//double new_val = (previous_plate[i + 1][j] + previous_plate[i - 1][j] +
					//	previous_plate[i][j + 1] + previous_plate[i][j - 1] + 4 * previous_plate[i][j]) / 8;
					//current_plate[i][j] = new_val;

					//file_out << new_val << " ";
					//png_out << (int)new_val << " ";


					curr[i][j] = (prev[i + 1][j] + prev[i - 1][j] +
						prev[i][j + 1] + prev[i][j - 1] + 4 * prev[i][j]) / 8;
					//steady state check
					if (fabs(prev[i][j] - (prev[i + 1][j] + prev[i - 1][j] +
						prev[i][j + 1] + prev[i][j - 1]) / 4) > 0.1)
					{
						++converged;
					}
				}

			}
			//file_out << "\n"; //debug
		}

		//Swap
		auto *swap = prev;
		prev = curr;
		curr = swap;

		++cycles;
		//file_out << "\n"; //debug
		//file_out << "\n Cycle: " << cycles << " Converged: " << converged << "\n";
		//std::cout << "\n Cycle: " << cycles << " Converged: " << converged << "\n";

	} while (converged > 0);


	//End time
	auto end_time = std::chrono::high_resolution_clock::now();

	auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
	file_out << "Total running time: " << time_span.count() << " seconds\n";
	file_out << "Iterations: " << cycles;

/*	for(int r = 0, r < PLATE_SIZE; ++r)
	{
		for(int c = 0; c < PLATE_SIZE; ++c)
		{
			delete previous_plate[r][c];
		}
		delete[] previous_plate[r];
		delete previous_plate[i];
	}
	delete[] previous_plate;
	delete[] current_plate;
	*/
}
