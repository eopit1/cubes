//Emmanuel Opit

enum eVertexArrayObject {
    VAOVerticesData,
    VAOSphereData,      
    VAOCount
};

enum eBufferObject {
    VBOVerticesData,
    VBOSphereData,      
    BOCount
};

enum eProgram {
    QuadScreenProgram,
    ProgramCount
};

enum eTexture {
    BrickTexture,
    TextureCount
};

#include "common.cpp"

GLchar windowTitle[] = "Lighting and Z-buffer";

GLuint cameraPositionLoc;
GLuint lightPositionLoc;
GLuint inverseTransposeMatrixLoc;

vec3 cameraPosition = vec3(0.0f, 1.5f, 5.0f);
vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
vec3 cameraUpVector = vec3(0.0f, 1.0f, 0.0f);

vec3 lightPosition;

mat3 inverseTransposeMatrix;

float radius = 5.0f;
float cameraAngle = 0.0f;
float height = 1.5f;

float lightRadius = 4.0f;
float lightSpeed = 1.0f;

bool lightEnabled = true;
bool useMagenta = false;
bool sunVisible = true;      

GLfloat verticesData[] = {
  

    
    -0.5f,-0.5f,-0.5f, 0,0,-1,
     0.5f,-0.5f,-0.5f, 0,0,-1,
     0.5f, 0.5f,-0.5f, 0,0,-1,
     0.5f, 0.5f,-0.5f, 0,0,-1,
    -0.5f, 0.5f,-0.5f, 0,0,-1,
    -0.5f,-0.5f,-0.5f, 0,0,-1,

   
    -0.5f,-0.5f,0.5f, 0,0,1,
     0.5f,-0.5f,0.5f, 0,0,1,
     0.5f, 0.5f,0.5f, 0,0,1,
     0.5f, 0.5f,0.5f, 0,0,1,
    -0.5f, 0.5f,0.5f, 0,0,1,
    -0.5f,-0.5f,0.5f, 0,0,1,

  
    -0.5f, 0.5f, 0.5f,-1,0,0,
    -0.5f, 0.5f,-0.5f,-1,0,0,
    -0.5f,-0.5f,-0.5f,-1,0,0,
    -0.5f,-0.5f,-0.5f,-1,0,0,
    -0.5f,-0.5f, 0.5f,-1,0,0,
    -0.5f, 0.5f, 0.5f,-1,0,0,

    
     0.5f, 0.5f, 0.5f,1,0,0,
     0.5f, 0.5f,-0.5f,1,0,0,
     0.5f,-0.5f,-0.5f,1,0,0,
     0.5f,-0.5f,-0.5f,1,0,0,
     0.5f,-0.5f, 0.5f,1,0,0,
     0.5f, 0.5f, 0.5f,1,0,0,

   
     -0.5f,-0.5f,-0.5f,0,-1,0,
      0.5f,-0.5f,-0.5f,0,-1,0,
      0.5f,-0.5f, 0.5f,0,-1,0,
      0.5f,-0.5f, 0.5f,0,-1,0,
     -0.5f,-0.5f, 0.5f,0,-1,0,
     -0.5f,-0.5f,-0.5f,0,-1,0,

    
     -0.5f,0.5f,-0.5f,0,1,0,
      0.5f,0.5f,-0.5f,0,1,0,
      0.5f,0.5f, 0.5f,0,1,0,
      0.5f,0.5f, 0.5f,0,1,0,
     -0.5f,0.5f, 0.5f,0,1,0,
     -0.5f,0.5f,-0.5f,0,1,0
};

std::vector<GLfloat> sphereVertices;
int sphereVertexCount = 0;

void buildSphere(float r, int latSteps, int lonSteps)
{
    sphereVertices.clear();
    const float PI = 3.14159265358979f;
    for (int i = 0; i <= latSteps; i++) {
        float theta = (float)i / latSteps * PI;
        for (int j = 0; j <= lonSteps; j++) {
            float phi = (float)j / lonSteps * 2.0f * PI;
            float x = sinf(theta) * cosf(phi);
            float y = cosf(theta);
            float z = sinf(theta) * sinf(phi);
            float u = (float)j / lonSteps;
            float v = (float)i / latSteps;
           
            sphereVertices.insert(sphereVertices.end(), {
                r * x, r * y, r * z,   
                x,   y,   z,     
                u,   v            
                });
        }
    }

   
    std::vector<GLfloat> expanded;
    auto vert = [&](int i, int j) {
        int idx = (i * (lonSteps + 1) + j) * 8;
        for (int k = 0; k < 8; k++) expanded.push_back(sphereVertices[idx + k]);
        };
    for (int i = 0; i < latSteps; i++) {
        for (int j = 0; j < lonSteps; j++) {
            vert(i, j);   vert(i + 1, j);   vert(i, j + 1);
            vert(i + 1, j + 1); vert(i, j + 1); vert(i + 1, j);
        }
    }
    sphereVertices = expanded;
    sphereVertexCount = (int)sphereVertices.size() / 8;
}

double deltaTime;
bool mousePressed = false;
double lastX = 0.0, lastY = 0.0;
bool firstMouse = true;
GLuint lightDirectionLoc;
GLuint lightColorLoc;
GLuint lightEnabledLoc;
GLuint objectColorLoc;
GLuint isSunLoc;            
GLuint sunTextureLoc;       

GLuint loadTexture(const GLchar* texturePath) {
    GLuint textureID = SOIL_load_OGL_texture(
        texturePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if (textureID == 0) {
        cout << "Error: could not find texture image: " << texturePath << endl;
        cleanUpScene(EXIT_FAILURE);
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    return textureID;
}

void initShaderProgram()
{
    buildSphere(0.3f, 24, 24);

    ShaderInfo shader_info[ProgramCount][3] = {
        {
            { GL_VERTEX_SHADER,   "./vertexShader.glsl"   },
            { GL_FRAGMENT_SHADER, "./fragmentShader.glsl" },
            { GL_NONE, nullptr }
        }
    };

    for (int i = 0; i < ProgramCount; i++)
        program[i] = LoadShaders(shader_info[i]);

    glUseProgram(program[0]);

    locationMatModel = glGetUniformLocation(program[0], "matModel");
    locationMatView = glGetUniformLocation(program[0], "matView");
    locationMatProjection = glGetUniformLocation(program[0], "matProjection");
    inverseTransposeMatrixLoc = glGetUniformLocation(program[0], "inverseTransposeMatrix");
    lightPositionLoc = glGetUniformLocation(program[0], "lightPosition");
    cameraPositionLoc = glGetUniformLocation(program[0], "cameraPosition");
    lightDirectionLoc = glGetUniformLocation(program[0], "lightDirection");
    lightColorLoc = glGetUniformLocation(program[0], "lightColor");
    lightEnabledLoc = glGetUniformLocation(program[0], "lightEnabled");
    objectColorLoc = glGetUniformLocation(program[0], "objectColor");
    isSunLoc = glGetUniformLocation(program[0], "isSun");      // NEW
    sunTextureLoc = glGetUniformLocation(program[0], "sunTexture"); // NEW

    glBindVertexArray(VAO[VAOVerticesData]);
    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOVerticesData]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesData), verticesData, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDisableVertexAttribArray(2);

    glBindVertexArray(VAO[VAOSphereData]);
    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOSphereData]);
    glBufferData(GL_ARRAY_BUFFER,
        sphereVertices.size() * sizeof(GLfloat),
        sphereVertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);

    texture[BrickTexture] = loadTexture("sun.jpg");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[BrickTexture]);
}

void computeCameraMatrix()
{
    matView = lookAt(cameraPosition, cameraTarget, cameraUpVector);
    glUniformMatrix4fv(locationMatView, 1, GL_FALSE, value_ptr(matView));
}

void drawCube(vec3 pos)
{
    glBindVertexArray(VAO[VAOVerticesData]);

    matModel = translate(mat4(1.0f), pos);
    glUniformMatrix4fv(locationMatModel, 1, GL_FALSE, value_ptr(matModel));

    inverseTransposeMatrix = mat3(inverseTranspose(matModel));
    glUniformMatrix3fv(inverseTransposeMatrixLoc, 1, GL_FALSE, value_ptr(inverseTransposeMatrix));

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawSunSphere(vec3 pos)
{
    glBindVertexArray(VAO[VAOSphereData]);

    matModel = translate(mat4(1.0f), pos);
    glUniformMatrix4fv(locationMatModel, 1, GL_FALSE, value_ptr(matModel));

    inverseTransposeMatrix = mat3(inverseTranspose(matModel));
    glUniformMatrix3fv(inverseTransposeMatrixLoc, 1, GL_FALSE, value_ptr(inverseTransposeMatrix));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[BrickTexture]);
    glUniform1i(sunTextureLoc, 0);

    glDrawArrays(GL_TRIANGLES, 0, sphereVertexCount);
}

void display(GLFWwindow* window, double currentTime)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static double lastFrame = 0.0;
    deltaTime = currentTime - lastFrame;
    lastFrame = currentTime;

    float speed = 2.0f * (float)deltaTime;

    if (keyboard[GLFW_KEY_W] || keyboard[GLFW_KEY_UP])    height += speed;
    if (keyboard[GLFW_KEY_S] || keyboard[GLFW_KEY_DOWN])  height -= speed;
    if (keyboard[GLFW_KEY_A] || keyboard[GLFW_KEY_LEFT])  cameraAngle -= speed;
    if (keyboard[GLFW_KEY_D] || keyboard[GLFW_KEY_RIGHT]) cameraAngle += speed;

    cameraPosition.x = radius * cos(cameraAngle);
    cameraPosition.z = radius * sin(cameraAngle);
    cameraPosition.y = height;

    computeCameraMatrix();

    float t = (float)currentTime * lightSpeed;
    lightPosition.x = lightRadius * cos(t);
    lightPosition.z = lightRadius * sin(t);
    lightPosition.y = 2.0f;

    vec3 lightDirection = normalize(-lightPosition);

    glUniform3fv(lightPositionLoc, 1, value_ptr(lightPosition));
    glUniform3fv(lightDirectionLoc, 1, value_ptr(lightDirection));
    glUniform3fv(cameraPositionLoc, 1, value_ptr(cameraPosition));
    glUniform3fv(lightColorLoc, 1, value_ptr(vec3(1.0f, 1.0f, 0.0f))); 
    glUniform1i(lightEnabledLoc, lightEnabled ? 1 : 0);

    vec3 activeObjectColor = useMagenta ? vec3(1.0f, 0.0f, 1.0f) : vec3(1.0f, 1.0f, 1.0f);
    glUniform3fv(objectColorLoc, 1, value_ptr(activeObjectColor));
    glUniform1i(isSunLoc, 0);  
    drawCube(vec3(-2.0f, 0.0f, 0.0f));
    drawCube(vec3(0.0f, 0.0f, 0.0f));
    drawCube(vec3(2.0f, 0.0f, 0.0f));
    if (sunVisible) {
        glUniform1i(isSunLoc, 1);
        drawSunSphere(lightPosition);
        glUniform1i(isSunLoc, 0);
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    windowWidth = glm::max(width, 1);
    windowHeight = glm::max(height, 1);

    glViewport(0, 0, windowWidth, windowHeight);

    matProjection = perspective(
        radians(45.0f),
        (float)windowWidth / (float)windowHeight,
        0.1f, 100.0f
    );

    glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)   keyboard[key] = GL_TRUE;
    else if (action == GLFW_RELEASE) keyboard[key] = GL_FALSE;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        lightEnabled = !lightEnabled;

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        useMagenta = !useMagenta;

}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (!mousePressed) return;

    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    double xOffset = xPos - lastX;
    double yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    cameraAngle += (float)xOffset * 0.005f;
    height += (float)yOffset * 0.005f;
    height = glm::clamp(height, -5.0f, 5.0f);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mousePressed = true;
            firstMouse = true;
        }
        else if (action == GLFW_RELEASE) {
            mousePressed = false;
        }
    }
}

int main()
{
    init(3, 3, GLFW_OPENGL_COMPAT_PROFILE);

    initShaderProgram();
    framebufferSizeCallback(window, windowWidth, windowHeight);

    while (!glfwWindowShouldClose(window))
    {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanUpScene(EXIT_SUCCESS);
    return 0;
}