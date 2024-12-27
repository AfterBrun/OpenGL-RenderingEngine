## OpenGL_Rendering_Engine


### 1. 프로젝트 소개
* 본 프로젝트는 OpenGL을 이용하여 작성된 Renderer 입니다.

### 2. 적용 기술
1. Blinn Phong Shading
2. Normal Map
3. Shadow Map [기술 설명](https://github.com/AfterBrun/OpenGL-RenderingEngine/blob/master/HomemadeEngine/shader/shadowMapping.vs)
4. Height Map을 이용한 지형 렌더링
5. Assimp를 활용한 모델 불러오기

### 3. 사용 라이브러리
* OpenGL 3.3: 3D 렌더링
* GLFW 3.x: 윈도우 생성 및 이벤트 처리
* GLAD: OpenGL 함수 로딩 라이브러리
* Assimp: obj, fbx등 모델 파일 로딩
* stb_image: image파일 로딩 / MIT license
* spdlog: 콘솔 log출력 / MIT license
* GLM: 선형수학 라이브러리 
* ImGui: Ui 렌더링 / MIT license

### 4. 개발 환경
* Visual Studio 2019
* C++20

### 5. 사용법
* 카메라 이동:
  * 마우스 오른쪽을 누른상태로 W, A, S, D: 이동
  * 마우스 오른쪽을 누른상태로 R: 위로, F: 아래로

### 6. Future Study List
* Bloom
* PBR(Physics based rendering)
* Cascaded Shadow Mapping
* Geo Mipmaping(QuadTree, Tessellation shader)
* Displacement mapping
* Ray Marching, Ray Tracing
* Texture noise based procedurally generated terrain
    * perlin noise
    * Hydraulic erosion algorithm
* Volumetric cloud
