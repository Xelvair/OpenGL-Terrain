##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=OpenGL-Terrain
ConfigurationName      :=Debug
WorkspacePath          := "/home/xel"
ProjectPath            := "/home/xel/OpenGL-Terrain1"
IntermediateDirectory  :=Debug/
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=xel
Date                   :=08/03/16
CodeLitePath           :="/home/xel/.codelite"
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="OpenGL-Terrain.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)glfw3 $(LibrarySwitch)easybmp $(LibrarySwitch)GLEW $(LibrarySwitch)GLU $(LibrarySwitch)GL $(LibrarySwitch)X11 $(LibrarySwitch)Xrandr $(LibrarySwitch)Xxf86vm $(LibrarySwitch)pthread $(LibrarySwitch)dl $(LibrarySwitch)Xinerama $(LibrarySwitch)Xcursor $(LibrarySwitch)Xi 
ArLibs                 :=  "glfw3" "easybmp" "GLEW" "GLU" "GL" "X11" "Xrandr" "Xxf86vm" "pthread" "dl" "Xinerama" "Xcursor" "Xi" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := g++
CC       := gcc
CXXFLAGS :=  -g -O0 -Wall  -std=c++11 $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/async_noise.cpp$(ObjectSuffix) $(IntermediateDirectory)/erosion_simulator.cpp$(ObjectSuffix) $(IntermediateDirectory)/noise.cpp$(ObjectSuffix) $(IntermediateDirectory)/TerrainRenderer.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d Debug/ || $(MakeDirCommand) Debug/

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/xel/OpenGL-Terrain1/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM "main.cpp"

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) "main.cpp"

$(IntermediateDirectory)/async_noise.cpp$(ObjectSuffix): async_noise.cpp $(IntermediateDirectory)/async_noise.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/xel/OpenGL-Terrain1/async_noise.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/async_noise.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/async_noise.cpp$(DependSuffix): async_noise.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/async_noise.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/async_noise.cpp$(DependSuffix) -MM "async_noise.cpp"

$(IntermediateDirectory)/async_noise.cpp$(PreprocessSuffix): async_noise.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/async_noise.cpp$(PreprocessSuffix) "async_noise.cpp"

$(IntermediateDirectory)/erosion_simulator.cpp$(ObjectSuffix): erosion_simulator.cpp $(IntermediateDirectory)/erosion_simulator.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/xel/OpenGL-Terrain1/erosion_simulator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/erosion_simulator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/erosion_simulator.cpp$(DependSuffix): erosion_simulator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/erosion_simulator.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/erosion_simulator.cpp$(DependSuffix) -MM "erosion_simulator.cpp"

$(IntermediateDirectory)/erosion_simulator.cpp$(PreprocessSuffix): erosion_simulator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/erosion_simulator.cpp$(PreprocessSuffix) "erosion_simulator.cpp"

$(IntermediateDirectory)/noise.cpp$(ObjectSuffix): noise.cpp $(IntermediateDirectory)/noise.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/xel/OpenGL-Terrain1/noise.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/noise.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/noise.cpp$(DependSuffix): noise.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/noise.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/noise.cpp$(DependSuffix) -MM "noise.cpp"

$(IntermediateDirectory)/noise.cpp$(PreprocessSuffix): noise.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/noise.cpp$(PreprocessSuffix) "noise.cpp"

$(IntermediateDirectory)/TerrainRenderer.cpp$(ObjectSuffix): TerrainRenderer.cpp $(IntermediateDirectory)/TerrainRenderer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/xel/OpenGL-Terrain1/TerrainRenderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/TerrainRenderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/TerrainRenderer.cpp$(DependSuffix): TerrainRenderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/TerrainRenderer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/TerrainRenderer.cpp$(DependSuffix) -MM "TerrainRenderer.cpp"

$(IntermediateDirectory)/TerrainRenderer.cpp$(PreprocessSuffix): TerrainRenderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/TerrainRenderer.cpp$(PreprocessSuffix) "TerrainRenderer.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r Debug/


