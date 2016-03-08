#ifndef EROSION_SIMULATOR_H
#define EROSION_SIMULATOR_H

class erosion_simulator{
  private:
    int width;
    int height;
    float* rock_height;
    float* soil_height;
    float* water_height;
    float* soil_pipes;
    float* water_pipes;

  public:
    static int pipe_count(int w, int h);
    int up_pipe_address(int x, int y);
    int down_pipe_address(int x, int y);
    int left_pipe_address(int x, int y);
    int right_pipe_address(int x, int y);

  public:
    erosion_simulator(int width, int height);
    ~erosion_simulator();

};

#endif //EROSION_SIMULATOR_H
