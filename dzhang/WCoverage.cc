#include <WCoverage.h>

WCoverage::WCoverage(const vector<SensorInfo> &sensors,
		     const vector<vector<double> > &dem, int size_of_tin, int width){
  NO_TIN_D = size_of_tin;
  width_of_terrain = width;
  Sensors = sensors;
  DEM = dem;
  int tin_size = 2 * NO_TIN_D * NO_TIN_D;
  coverageMatrix = vector<int> (tin_size,0);
  Tin_Matrix.clear();
  updateTinMatrix();
  updateCoverageMatrix();
  testingFun();
  // coveringMappingMatrix = vector<(Sensors.size(), vector<int> ());
  
}

double WCoverage:: evaluateSingleCoverRedun(const int chs_id){
  double coverageRedun = 0.0;
  int ele = chs_id;
      if(ele < coveringMappingMatrix.size()){
	auto coveredTinSet = coveringMappingMatrix[ele];
	if(coveredTinSet.size() > 0 ){
	  double denominator = 0;
	  double numerator  = 0;
	  for( auto & coveredTin:  coveredTinSet ) {
	    numerator ++;
	     int coveredTimes  = coverageMatrix[coveredTin];
	     if(coveredTimes < 1 ){
	       throw "Calculaton error, at least covered by self";
	     } else {
	       denominator += 1.0 / (double)coveredTimes ;
	     }
	   }
	  coverageRedun += numerator/ denominator;
	}else{
	  //does nothing,  no covertin, no coverage redundancy;
	}
      }else {
	throw "EA setting problem: sensor id exceed coveragemappingmatrix length!";
      }
      //assert( coverageRedun != 0.0);
      return coverageRedun;
}
double WCoverage:: evaluateCoverageRedun(const vector<int> &chs_id)
{
  vector<int> clusterHeads = chs_id;
  int totalCoverage = 0;
  int sizeOfCH  = clusterHeads.size();
  
  // for(const auto & ele : coverageMatrix){
  //   if (ele)   totalCoverage++;
  // }
  // double totalCoverageRatio =(double) totalCoverage  / (double)coverageMatrix.size();

  
  double coverageRedun=0.0 ;
  if(sizeOfCH != 0){

    for(const auto &ele : clusterHeads){
      if(ele < coveringMappingMatrix.size()){
	auto coveredTinSet = coveringMappingMatrix[ele];
	if(coveredTinSet.size() > 0 ){
	  double denominator = 0;
	  double numerator  = 0;
	  for( auto & coveredTin:  coveredTinSet ) {
	    numerator ++;
	     int coveredTimes  = coverageMatrix[coveredTin];
	     if(coveredTimes < 1 ){
	       throw "Calculaton error, at least covered by self";
	     } else {
	       denominator += 1.0 / (double)coveredTimes ;
	     }
	   }
	  coverageRedun += (double) numerator/ denominator;
	}else{
	  //does nothing,  no covertin, no coverage redundancy;
	}
      }else {
	throw "EA setting problem: sensor id exceed coveragemappingmatrix length!";
      }
    }
    assert( coverageRedun != 0.0);
  } else{
    //everyone is CH
    for(int idx_ch =0;idx_ch < coverageMatrix.size();idx_ch ++){
    auto coveredTinSet = coveringMappingMatrix[idx_ch];
	if(coveredTinSet.size() > 0 ){
	  double denominator = 0.0;
	  double numerator  = 0.0;
	  for( auto & coveredTin:  coveredTinSet ) {
	    numerator ++;
	     int coveredTimes  = coverageMatrix[coveredTin];
	     if(coveredTimes < 1 ){
	       throw "Calculaton error, at least covered by self";
	     } else {
	       denominator += 1.0 /(double) coveredTimes ;
	     }
	   }
	  coverageRedun += numerator/ denominator;
	}else{
	  //does nothing,  no covertin, no coverage redundancy;
	}
    }

  }
  int outsideNumerator = (sizeOfCH ==0)? coverageMatrix.size() : sizeOfCH;
  return (double) coverageRedun ;


};
void WCoverage:: updateTinMatrix()
{
    // int noOfTilehori = 0, noOfTileVerti =0;
  int width = width_of_terrain;
  static int tin_id = 0;
  double TIN_D = (double) width / (double) NO_TIN_D;

       for(int i =0 ;i < NO_TIN_D; i++){

	for( int j=0;j < NO_TIN_D; j++){
	  coor3d upperleft = givingElev({j*TIN_D, i*TIN_D, 0});
	  coor3d lowerleft = givingElev({j*TIN_D, (i+1)*TIN_D, 0});
	  coor3d upperright = givingElev({(j+1)*TIN_D, i*TIN_D, 0});
	  coor3d lowerright = givingElev({(j+1)*TIN_D, (i+1)*TIN_D, 0});
	  //the vertices in tin has (x,y)
	  Tin_Matrix.push_back( Tin(tin_id++, upperleft, lowerleft, lowerright));
	  //	  Tin temp = Tin_Matrix.back();
	  //	  trace() << "tin-" << temp.id << " : "<< temp.first.x << " - "<< temp.first.y <<"\n";
	  Tin_Matrix.push_back( Tin(tin_id++, upperleft, upperright, lowerright));
	}
      }

}
 void WCoverage :: updateCoverageMatrix()
{
  coveringMappingMatrix = vector<vector<int>> (Sensors.size(), vector<int>(0));
  for (int s_idx =Sensors.size()-1;s_idx>=0 ;s_idx--){
  //0 no checked, 1 : checked and valid , 2 : checked and invalid
    vector<vector<int> > sqMatrix (NO_TIN_D, vector<int> (NO_TIN_D, 0));
    // ostringstream os;
    // os<<"x: "<< Sensors[s_idx].x <<"y: "<<Sensors[s_idx].y<<"senrange: "<< Sensors[s_idx].sensorRadius;
    // trace()<< os.str();
  
    updateSingleCoverage(sqMatrix, Sensors[s_idx] );

    for(int i=0;i< sqMatrix.size();i++){
      for(int j = 0;j<sqMatrix[0].size();j++){
	//sqmatrix is row col
	if(sqMatrix[i][j]== 1){
	  auto p = findTinIdFromSqtCoor(j,i);
	  for(auto p_e : p){
	    //Tin is pure (x, y), sensor is pure (x,y)
	    if(coveringTin(Tin_Matrix[p_e], Sensors[s_idx])){
	      coverageMatrix[p_e] += 1;
	      coveringMappingMatrix[s_idx].push_back(p_e);
	      
	    }
  
	  }
	}
	
      }
    }
  }
}


void  WCoverage ::  updateSingleCoverage(vector<vector<int>> &board, const SensorInfo sen) 
{
  int width = width_of_terrain;
  //  vector<pair<int, int>> rst;
  double r = sen.sensorRadius;
  double s_x = sen.x;
  double s_y = sen.y;
  double s_z = sen.z;
  double cellwidth =(double) width / (double) NO_TIN_D; 
  int tem_j = int(s_x / cellwidth);
  int tem_i = int(s_y /cellwidth);
  bfs(board, tem_i, tem_j, s_x, s_y, sen.sensorRadius); 
   // for(auto row : board){
   //    trace() << "\n";
   //    ostringstream os;
   //    for(auto ele : row){
   // 	//	trace()<<" "<< ele <<" ";
	
   // 	os<< ele <<" ";
   //    }
   //    trace()<< os.str();
   // }

}

void WCoverage :: bfs (vector<vector<int>> &board, int row, int col, double coorx, double coory, double sen_range) {
  //row col
  typedef pair<int, int> state;

  queue<state> q;
  const int m =  board.size();
  const int n = board[0].size();
  int width = width_of_terrain;
  int cellWidth = 5;
  auto state_is_valid = [&](const state &s) {
    // row and col
    const int x = s.first;
    const int y = s.second;
    if( x < 0 || x >= m || y< 0 || y>= n|| board[x][y] != 0){
      return false;
    }
    return true;
  };
  auto out_of_radius = [&](const state &s  ) {
    
    const double cellwidth =(double) width / (double) NO_TIN_D; 
    const double x_min_distance =   std::min(std::abs((s.second) * cellwidth - coorx) 
					     , std::abs((s.second+1) * cellwidth - coorx));
    const double y_min_distance =   std::min(std::abs((s.first) * cellwidth - coory) 
					     , std::abs((s.first+1) * cellwidth - coory));
    return (x_min_distance >= sen_range)||(y_min_distance >= sen_range);
  };


  auto state_extend = [&](const state &s ){
    vector<state> ext_result;
    ext_result.clear();
    const int x = s.first;
    const int y = s.second;
    const state new_states[4] = {
      {x-1, y}, {x+1, y}, {x, y-1}, {x, y+1}};
    for(int k=0;k<4;++k){
      if(state_is_valid(new_states[k])){
  //0: un-checked, 1 : checked and valid , 2 : checked and invalid
	if(!out_of_radius(new_states[k])){
	  board[new_states[k].first][new_states[k].second] = 1;
	  ext_result.push_back(new_states[k]);
	} else{
	  board[new_states[k].first][new_states[k].second] = 2;
	}
      } 
	
	
    }
    return ext_result;
   
  };

  state start ={row, col};
  if(state_is_valid (start)){
    board[row][col] = 1;
    q.push(start);
  }
  while(!q.empty()){
    auto cur = q.front();
    q.pop();
    auto new_states = state_extend(cur);
    for(auto s: new_states) q.push(s);
  }
}


