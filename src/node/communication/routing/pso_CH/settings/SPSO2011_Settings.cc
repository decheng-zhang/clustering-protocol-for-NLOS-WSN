//  PSO_Settings.cpp
//
//  Author:
//       Esteban López-Camacho <esteban@lcc.uma.es>
//
//  Copyright (c) 2011 Antonio J. Nebro, Juan J. Durillo
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include <SPSO2011_Settings.h>


/**
 * Default constructor
 */
SPSO2011_Settings::SPSO2011_Settings () : Settings() 
{
		

} // PSO_Settings


/**
 * Destructor
 */
SPSO2011_Settings::~SPSO2011_Settings () {
  delete algorithm ;

} // ~PSO_Settings


/**
 * Constructor
 */
SPSO2011_Settings::SPSO2011_Settings(string problemName, int networkSize_ ,vector< vector<double>> adjacencyM) 
{

    problemName_ = problemName ;

    problem_ = ProblemFactory::getProblem((char *) problemName_.c_str(),networkSize_,adjacencyM);

    int swarmSize_ = 50;
    int maxIterations_ = 500;
    int numberOfParticlesToInform_ = 50;
  
} // PSO_Settings


/*** Configure method */
Algorithm * SPSO2011_Settings::configure() 
{
	  algorithm = new StandardPSO2011(problem_);

  	  algorithm->setInputParameter("swarmSize",&swarmSize_);
  	  algorithm->setInputParameter("maxIterations",&maxIterations_);
          algorithm->setInputParameter("numberOfParticlesToInform", &numberOfParticlesToInform_);

	  return algorithm ;

} // configure

