LIB_CODE_BASE=../external/support

HW2_CXX_SRC=main.cpp video_recorder.cpp light.cpp scene.cpp scene_object.cpp object.cpp mesh.cpp camera.cpp glut_program.cpp sample_program.cpp basic_obj_library.cpp utilities.cpp
HW2_HEADER=video_recorder.h light.h camera.h glut_program.h sample_program.h mesh.h scene_object.h object.h scene.h basic_obj_library.h utilities.h
HW2_OBJ=$(notdir $(patsubst %.cpp,%.o,$(HW2_CXX_SRC)))

LIB_CODE_CXX_SRC=$(wildcard $(LIB_CODE_BASE)/*.cpp)
LIB_CODE_HEADER=$(wildcard $(LIB_CODE_BASE)/*.h)
LIB_CODE_OBJ=$(notdir $(patsubst %.cpp,%.o,$(LIB_CODE_CXX_SRC)))

IMAGE_LIB_SRC=$(wildcard ../external/imageIO/*.cpp)
IMAGE_LIB_HEADER=$(wildcard ../external/imageIO/*.h)
IMAGE_LIB_OBJ=$(notdir $(patsubst %.cpp,%.o,$(IMAGE_LIB_SRC)))

HEADER=$(HW2_HEADER) $(LIB_CODE_HEADER) $(IMAGE_LIB_HEADER)
CXX_OBJ=$(HW2_OBJ) $(LIB_CODE_OBJ) $(IMAGE_LIB_OBJ)

CXX=g++
TARGET=sample
CXXFLAGS=-DGLM_FORCE_RADIANS -std=c++11
OPT=-O3

UNAME_S=$(shell uname -s)

ifeq ($(UNAME_S),Linux)
  PLATFORM=Linux
  INCLUDE=-I../external/glm/ -I$(LIB_CODE_BASE) #-I../external/imageIO -I../external/VegaFEM-v2.2/
  LIB=-lGLEW -lGL -lglut -ljpeg #-../external/VegaFEM-v2.2/minivector/libminivector.a
  LDFLAGS=
else
  PLATFORM=Mac OS
  INCLUDE=-I../external/glm/ -I$(LIB_CODE_BASE) -I../external/imageIO -I../external/jpeg-9a-mac/include #-I../external/VegaFEM-v2.2/minivector -I../external/VegaFEM-v2.2/objMesh
  LIB=-framework OpenGL -framework GLUT ../external/jpeg-9a-mac/lib/libjpeg.a #../external/VegaFEM-v2.2/minivector/libminivector.a ../external/VegaFEM-v2.2/objMesh/libobjMesh.a
  CXXFLAGS+= -Wno-deprecated-declarations
  LDFLAGS=-Wl,-w
endif

all: $(TARGET)

$(TARGET): $(CXX_OBJ)
	$(CXX) $(LDFLAGS) $^ $(OPT) $(LIB) -o $@

$(HW2_OBJ):%.o: %.cpp $(HEADER)
	$(CXX) -c $(CXXFLAGS) $(OPT) $(INCLUDE) $< -o $@

$(LIB_CODE_OBJ):%.o: $(LIB_CODE_BASE)/%.cpp $(LIB_CODE_HEADER)
	$(CXX) -c $(CXXFLAGS) $(OPT) $(INCLUDE) $< -o $@

$(IMAGE_LIB_OBJ):%.o: ../external/imageIO/%.cpp $(IMAGE_LIB_HEADER)
	$(CXX) -c $(CXXFLAGS) $(OPT) $(INCLUDE) $< -o $@

clean: 
	rm -rf *.o $(TARGET)