#include "erosion_simulator.h"

erosion_simulator::erosion_simulator(int width, int height)
  :width(width)
  ,height(height)
  ,rock_height(new float[width * height])
  ,soil_height(new float[width * height])
  ,water_height(new float[width * height])
  ,soil_pipes(new float[erosion_simulator::pipe_count(width, height)])
  ,water_pipes(new float[erosion_simulator::pipe_count(width, height)])
{}

erosion_simulator::~erosion_simulator(){
  delete [] rock_height;
  delete [] soil_height;
  delete [] water_height;
  delete [] soil_pipes;
  delete [] water_pipes;
}

/**
 * Calculates the amount of pipes required to simulate the map
 */
int erosion_simulator::pipe_count(int width, int height){
  return 4 * width * height + 2 * width + 2 * height;
}

/**
 * <direcion>_pipe_address returns the address of the outbound
 * pipe in the specified direction at the speicfied tile
 */
int erosion_simulator::up_pipe_address(int x, int y){
  return y * (2 * width + 2 * (width + 1)) + (2 * x);
}

int erosion_simulator::down_pipe_address(int x, int y){
  return (y + 1) * (2 * width + 2 * (width + 1)) + (2 * x) + 1;
}

int erosion_simulator::left_pipe_address(int x, int y){
  return (2 * width) + y * (2 * (width + 1) + 2 * width) + (2 * x);
}

int erosion_simulator::right_pipe_address(int x, int y){
  return (2 * width) + y * (2 * (width + 1) + 2 * width) + (2 * (x + 1)) + 1;
}
