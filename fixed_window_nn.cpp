#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <armadillo>

#define DEBUG

typedef std::tuple<int, int, int> Triple;
typedef std::multimap<Triple, int> Map;

#ifdef DEBUG
using ms = std::chrono::milliseconds;
using get_time = std::chrono::steady_clock ;
#endif

int main(int argc, char* argv[])
{
#ifdef DEBUG
  auto start = get_time::now();
#endif

  Map mapData;

  if(argc != 6)
  {
    std::cout <<
      "Modo de uso: " << std::endl << argv[0] << " [archivo ubicaciones data 3D]" <<
      " [archivo ubicaciones a consultar 3D]" << " [ventana de busqueda en X Y Z]" << std::endl;
    return 0;
  }

  arma::mat data;
  arma::mat locations;
  arma::vec search_window;

  data.load(argv[1], arma::raw_ascii);
  locations.load(argv[2], arma::raw_ascii);
  search_window << std::stod(argv[3]) << std::stod(argv[4]) << std::stod(argv[5]);

  double minDataX = data.col(0).min();
  double minDataY = data.col(1).min();
  double minDataZ = data.col(2).min();
  double maxDataX = data.col(0).max();
  double maxDataY = data.col(1).max();
  double maxDataZ = data.col(2).max();

  for(int i = 0; i < data.n_rows; i++)
  {
    Triple dataLocation(
      ceil((abs(data(i,0) - minDataX)) / search_window(0)),
      ceil((abs(data(i,1) - minDataY)) / search_window(1)),
      ceil((abs(data(i,2) - minDataZ)) / search_window(2))
    );
    mapData.insert({dataLocation,i});
  }
  for(int i = locations.n_rows-1; i >= 0 ; i--)
  {
    Triple location(
      ceil((abs(locations(i,0) - minDataX)) / search_window(0)),
      ceil((abs(locations(i,1) - minDataY)) / search_window(1)),
      ceil((abs(locations(i,2) - minDataZ)) / search_window(2))
    );
    std::pair <Map::const_iterator, Map::const_iterator> ret;
    ret = mapData.equal_range(location);
    if(ret.first==ret.second) continue;
    for (Map::const_iterator itRet=ret.first; itRet!=ret.second; ++itRet)
      if(
        abs(locations(i,0) - data(itRet->second,0)) <= search_window(0) &&
        abs(locations(i,1) - data(itRet->second,1)) <= search_window(1) &&
        abs(locations(i,2) - data(itRet->second,2)) <= search_window(2)
      )
      {
          /* aca esta la data del mismo bloque donde esta la ubicacion
          data(itRet->second,0)
          data(itRet->second,1)
          data(itRet->second,2)
          */
      }
    for(int z = -1; z <= 1; z++)
      for(int y = -1; y <= 1; y++)
        for(int x = -1; x <= 1; x++)
        {
          Triple currentLocation(
            std::get<0>(location)+x, std::get<1>(location)+y, std::get<2>(location)+z
          );
          std::pair <Map::const_iterator, Map::const_iterator> retNeighbors;
          retNeighbors = mapData.equal_range(currentLocation);
          if(retNeighbors.first==retNeighbors.second) continue;
          for (Map::const_iterator itRet=retNeighbors.first; itRet!=retNeighbors.second; ++itRet)
          if(
             abs(locations(i,0) - data(itRet->second,0)) <= search_window(0) &&
             abs(locations(i,1) - data(itRet->second,1)) <= search_window(1) &&
             abs(locations(i,2) - data(itRet->second,2)) <= search_window(2)
          )
          {
            /* aca estan los datos correspondientes de los bloque colindantes a donde esta la ubicacion
            data(itRet->second,0)
            data(itRet->second,1)
            data(itRet->second,2)
            */
          }
        }
  }
#ifdef DEBUG
std::cout << "tama~no data: " << data.n_rows << "x" << data.n_cols << std::endl;
std::cout << "tama~no ubicaciones: " << locations.n_rows << "x" << locations.n_cols << std::endl;
std::cout << "tama~no de ventana de busqueda en XYZ respectivamente: " << search_window;
std::cout << "data min x: " << minDataX << " y: " << minDataY << " z: " << minDataZ << std::endl;
std::cout << "data max x: " << maxDataX << " y: " << maxDataY << " z: " << maxDataZ << std::endl;
std::cout << ceil((maxDataX - minDataX) / search_window(0)) << " bloques en eje X" << std::endl;
std::cout << ceil((maxDataY - minDataY) / search_window(1)) << " bloques en eje Y" << std::endl;
std::cout << ceil((maxDataZ - minDataZ) / search_window(2)) << " bloques en eje Z" << std::endl;
auto end = get_time::now();
auto diff = end - start;
std::cout<<"Elapsed time is :  "<< std::chrono::duration_cast<ms>(diff).count()<<" ms "<<std::endl;
#endif

  return 0;
}

