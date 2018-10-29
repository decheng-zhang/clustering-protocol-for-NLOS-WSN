//  ProblemFactory.cpp
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


#include <ProblemFactory.h>

/**
 * Problem factory
 * @param name : Name of the problem
 */
//Problem * ProblemFactory::getProblem(char * name) 
//{
  //return getProblem(name, 0, NULL);
//}

Problem * ProblemFactory::getProblem(char * name,int networkSize_,vector< vector<double>> adjacencyM) 
{
      return new MOPSOProblem(adjacencyM); 
 }


Problem * ProblemFactory::getProblem(char * name, int argc, char ** argv)
{

  
}


