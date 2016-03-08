#include "TerrainRenderer.h"

#include <GL/glew.h>
#include <cmath>
#include <stdexcept>

TerrainRenderer::TerrainRenderer(
	int width, 
	int height, 
	int chunkWidth, 
	int chunkHeight,
	int* chunkSubdivision,
	int chunkSubdivisionLength
)
:width(width)
,height(height)
,chunkWidth(chunkWidth)
,chunkHeight(chunkHeight)
,chunkSubdivision(chunkSubdivision)
,chunkSubdivisionLength(chunkSubdivisionLength)
,chunkCenterX(0)
,chunkCenterY(0)
,heightMap(width, height, -1.f)
,chunkMap(width / chunkWidth, height / chunkHeight, -1)
{
  // GET GPU BUFFERS
  int y_start = -getChunkDrawDistance();
  int y_end = getChunkDrawDistance();
  int x_start = -getChunkDrawDistance();
  int x_end = getChunkDrawDistance();
  
  for(int i = y_start; i <= y_end; ++i){
    for(int j = x_start; j <= x_end; ++j){
      int vertex_count = getChunkSize2D(getChunkSubdivision(j, i));
      
      int abs_x = j + (getChunkDrawDistance() - 1);
      int abs_y = j + (getChunkDrawDistance() - 1);
      
      GLuint buffer_id;
      glGenBuffers(1, &buffer_id);
      glBufferData(buffer_id, sizeof(float) * 3 * vertex_count, NULL, GL_DYNAMIC_DRAW);
      
      chunkMap(abs_x, abs_y) = buffer_id;
    }
  }
}

TerrainRenderer::~TerrainRenderer(){
  
}

int TerrainRenderer::getChunkMapWidth(){
	return (chunkSubdivisionLength * 2) - 1;
}

int TerrainRenderer::getChunkMapHeight(){
	return (chunkSubdivisionLength * 2) - 1;
}

int TerrainRenderer::getChunkMapSize(){
	return getChunkMapWidth() * getChunkMapHeight();
}

int TerrainRenderer::getChunkSubdivision(int x, int y){
  int dist = std::max(abs(x), abs(y));
  if(dist >= chunkSubdivisionLength)
    throw std::runtime_error("Invalid chunk!");
  
  return chunkSubdivision[dist];
}

int TerrainRenderer::getChunkDrawDistance(){
  return chunkSubdivisionLength;
}

int TerrainRenderer::setChunkCenter(int x, int y){
  chunkCenterX = x;
  chunkCenterY = y;
  
  // TODO: Shift chunks relative to old location and invalidate
  // chunks that need to be sent at a different resolution 
   
}

int TerrainRenderer::getChunkSize1D(int subdivisions){
  return (pow(2, subdivisions) + 1);
}

int TerrainRenderer::getChunkSize2D(int subdivisions){
  int chunk_size_1d = getChunkSize1D(subdivisions);
  return chunk_size_1d * chunk_size_1d;
}

int reloadChunk(int x, int y){
  
}

