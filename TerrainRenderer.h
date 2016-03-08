#ifndef TERRAIN_RENDERER_H
#define TERRAIN_RENDERER_H

#include <GL/glew.h>
#include <GL/gl.h>

#include "Array2D.h"

class TerrainRenderer{
  private:
    int     width;
    int     height;
    int     chunkWidth;
    int     chunkHeight;
    int*    chunkSubdivision;
    int     chunkSubdivisionLength;
    int     chunkCenterX;
    int     chunkCenterY;

    Array2D<float>    heightMap;
    Array2D<GLuint>   chunkMap;

	/**
	 * @brief Returns the width of the chunk map
	 * @return 
	 */
	int getChunkMapWidth();
	
	/**
	 * @brief Returns the height of the chunk map
	 * @return 
	 */
	int getChunkMapHeight();
	
	/**
	 * @brief Returns the area of the chunk map
	 * @return 
	 */
	int getChunkMapSize();
	
	/**
	 * @brief Calculates the subdivisions for the given chunk
	 * @param x relative X coord of the chunk
	 * @param y relative Y coord of the chunk
	 * @return integer representing the subdivision count of the chunk
	 */
	int getChunkSubdivision(int x, int y);

	/**
	 * @brief Returns the draw distance in chunks
	 * @return 
	 */
	int getChunkDrawDistance();
  
  /**
   * @brief Calculates the one dimensional size of the chunk
   * which would be the amount of vertices along the x or y direction
   * @param subdivisions chunk subdivision count
   * @return 
   */
  int getChunkSize1D(int subdivisions);
  
  /**
   * @brief Calculates the two dimensional size (area) of the chunk
   * which would be the amount of vertices in the chunk
   * @param subdivisions
   * @return 
   */
  int getChunkSize2D(int subdivisions);

  public:
    TerrainRenderer(
      int width, 
      int height, 
      int chunkWidth, 
      int chunkHeight, 
      int* chunkSubdivision, 
      int chunkSubdivisionLength
    );
    
    ~TerrainRenderer();
  
    /**
     * @brief Sets the center chunk of the terrain renderer
     * @param x center X coord
     * @param y center Y coord
     * @return 
     */
    int setChunkCenter(int x, int y);
};

#endif //TERRAIN_RENDERER_H
