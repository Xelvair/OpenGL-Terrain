#ifndef TERRAIN_RENDERER_H
#define TERRAIN_RENDERER_H

#include <GL/glew.h>
#include <GL/gl.h>

#include "Array2D.h"

class TerrainRenderer{
  private:
    struct chunk_info{
      GLuint buffer_id;
      bool valid;
    };
  
    int     chunkSize;
    int*    chunkSubdivision;
    int     chunkSubdivisionLength;
    int     chunkCenterX;
    int     chunkCenterY;
    float   terrainHeight;

    Array2D<float>    heightMap;
    Array2D<chunk_info>   chunkMap;
	
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
  
  /**
   * @brief Reloads the chunk at the specified index
   * @param x
   * @param y
   */
  void loadChunk(int x, int y);
  
  /**
   * @brief Converts absolute position in chunkMap to center-relative
   * Because chunks are rectangular, we don't need to distinguish between
   * X and Y
   * @param v the X or Y coord to be converted to relative
   * @return 
   */
  int toRelative(int v);
  
  /**
   * @brief Converts center-relative position in chunkMap to absolute
   * Because chunks are rectangular, we don't need to distinguish between
   * X and Y
   * @param v the X or Y coord to be converted to absolute
   * @return 
   */
  int toAbsolute(int v);
  
  public:
    TerrainRenderer(
      int width, 
      int height, 
      float terrainHeight,
      int chunkSize, 
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
    
    /**
     * @brief Renders the terrain
     */
    void render(GLuint modelMatrixUniform);
    
    /**
     * @brief Loads/reloads all visible chunks
     */
    void loadAllChunks();
    
    /**
     * @brief Copies heightmap
     * @param heightmap heightmap to be copied
     */
    void setHeightmap(const Array2D<float>& heightmap);
    
    /**
     * @brief Moves heightmap into renderer
     * @param heightmap heightmap to be moved
     */
    void setHeightmap(Array2D<float>&& heightmap);
    
    /**
     * @brief Sets the position of the LOD center
     */
    void setObserverPosition(float x, float y);
};

#endif //TERRAIN_RENDERER_H
