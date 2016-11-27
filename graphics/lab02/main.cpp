
 #include "objparser.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <assert.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "text.h"
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "stb_write_image.h"
#include <stdio.h>
#include <stdlib.h> // some memory management is done

/* using the FreeMono font from the GNU fonts collection. this is free and has a
 "copy-left" licence. see package for details */
#define FONT_FILE_NAME "FreeMono.ttf"
#define PNG_OUTPUT_IMAGE "atlas.png"
#define ATLAS_META_FILE "atlas.meta"


using namespace glm;
// dimensions of the window drawing surface
int gl_width = 800;
int gl_height = 800;

float backx = 0.0;
float backy = 0.0;
float backz = 0.0;
float backw = 0.0;
float upperlimit = 0.70;
float lowerlimit = 0.30;
float colourchange = 0;
bool xzero = true, zzero = true, yzero = true;
GLuint shader_programme;

GLuint points_vbo;
GLuint vao;
GLuint normals_vbo;
GLuint texturebuffer;


//
// copy a shader from a plain text file into a character array
bool parse_file_into_str (
                          const char* file_name, char* shader_str, int max_len
                          ) {
    FILE* file = fopen (file_name , "r");
    int current_len = 0;
    char line[2048];
    
    shader_str[0] = '\0'; /* reset string */
    if (!file) {
        fprintf (stderr, "This ERROR: opening file for reading: %s\n", file_name);
        return false;
    }
    strcpy (line, ""); /* remember to clean up before using for first time! */
    while (!feof (file)) {
        if (NULL != fgets (line, 2048, file)) {
            current_len += strlen (line); /* +1 for \n at end */
            if (current_len >= max_len) {
                fprintf (stderr,
                         "ERROR: shader length is longer than string buffer length %i\n",
                         max_len
                         );
            }
            strcat (shader_str, line);
        }
    }
    if (EOF == fclose (file)) { /* probably unnecesssary validation */
        fprintf (stderr, "ERROR: closing file from reading %s\n", file_name);
        return false;
    }
    return true;
}
//load texture to texture buffer from file
void ltexture(const char* filename){
    int x,y,n;
    unsigned char* texture = stbi_load (filename, &x, &y, &n, STBI_rgb_alpha);
    if(texture == NULL){
        
    }
    //GLuint texture_buffer;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texturebuffer);
    glActiveTexture(GL_TEXTURE1 );
    glBindTexture(GL_TEXTURE_2D, texturebuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y,0.75, GL_RGBA, GL_UNSIGNED_BYTE, texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(texture);

}



//load mesh, return point count.
int loadmesh (char* filename)
{
    
    int point_count = 0;
    GLfloat* vp = NULL; // array of vertex points
    GLfloat* vn = NULL; // array of vertex normals (not needed for assignment)
    GLfloat* vt = NULL; // array of texture coordinates
    if (!load_obj_file (filename, vp, vt, vn, point_count)) {
        fprintf (stderr, "ERROR: could not find mesh file...\n");
        // do something
    }
    // create a vertex points buffer
  //  GLuint points_vbo;
    glGenBuffers (1, &points_vbo);
    glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
    // note: this is dynamically allocated memory, so we can’t say "sizeof (vp)"
    glBufferData (GL_ARRAY_BUFFER, sizeof (float) * 3 * point_count, vp,
                  GL_STATIC_DRAW);
    
    // -----==create a texture coordinates vertex buffer object here==-----
    // free allocated memory
    glGenBuffers(1, &normals_vbo);
    glBindBuffer(GL_ARRAY_BUFFER ,normals_vbo);
    
    //glBufferData(GL_ARRAY_BUFFER, point_count, vt, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER,  sizeof(float) * 3 * point_count, vn, GL_STATIC_DRAW);
    
    glGenBuffers(1, &texturebuffer);
    glBindBuffer(GL_ARRAY_BUFFER ,texturebuffer);
    glBufferData(GL_ARRAY_BUFFER,  sizeof(float) * 2 * point_count, vt, GL_STATIC_DRAW);
    
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    
    
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    glBindBuffer (GL_ARRAY_BUFFER, normals_vbo);
    glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 0, NULL);
  
    glEnableVertexAttribArray(2);   //very important line do not delete
    glBindBuffer (GL_ARRAY_BUFFER, texturebuffer);
    glVertexAttribPointer(2,2,GL_FLOAT, GL_FALSE, 0, NULL);
    
    
    free (vp);
    free (vn);
    free (vt);
    
    return point_count;
}

//method for updating background colours based on left turn
void updatecoloursleft(){
    if(backx >upperlimit)
    {
        xzero = true;
    }
    if(backx < lowerlimit)
    {
        xzero = false;
    }
    
    if(xzero == true){
        backx-=colourchange;
    }
    else{
        backx+=colourchange;
    }
    
    if(backz > upperlimit){
        zzero = true;
    }
    if(backz < lowerlimit)
    {
        zzero = false;
    }

    if(zzero == true){
        backz-=colourchange;
    }
    else{
        backz+=colourchange;
    }
}
//method for updating background colours based on right turn
void updatecoloursright(){
    if(backy > upperlimit){
        yzero = true;
    }
    if(backy < lowerlimit)
    {
        yzero = false;
    }
    
    if(yzero == true){
        backy-=colourchange;
    }
    else{
        backy+=colourchange;
    }

}

vec3 updateposition(vec3 position, vec3 direction, float deltaTime, float speed ){

return position += direction * deltaTime * speed;

}

glm::vec3 moveplanet(glm::vec3 position){
    vec3 newposition;
    float x = position[0]+=0.5;
    float y = position[1]-=1;
    newposition = vec3(x,y, position[2]);
    return newposition;
}

int main () {
    GLFWwindow* window = NULL;
    const GLubyte* renderer;
    const GLubyte* version;
    
    //
    // Start OpenGL using helper libraries
    // --------------------------------------------------------------------------
    if (!glfwInit ()) {
        fprintf (stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }
    
    //* change to 3.2 if on Apple OS X
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3.2);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //*/
    
    window = glfwCreateWindow (gl_width, gl_height, "Hello Triangle", NULL, NULL);
    if (!window) {
        fprintf (stderr, "ERROR: opening OS window\n");
        return 1;
    }
    glfwMakeContextCurrent (window);
    
    glewExperimental = GL_TRUE;
    glewInit ();
    
    
    
    /* get version info */
    renderer = glGetString (GL_RENDERER); /* get renderer string */
    version = glGetString (GL_VERSION); /* version as a string */
    
    // --------------------------------------------------------------------------
    {
            
        char* vertex_shader_str;
        char* fragment_shader_str;
        
        // allocate some memory to store shader strings
        vertex_shader_str = (char*)malloc (81920);
        fragment_shader_str = (char*)malloc (81920);
        // load shader strings from text files
        assert (parse_file_into_str ("teapot.vert", vertex_shader_str, 81920));
        assert (parse_file_into_str ("teapot.frag", fragment_shader_str, 81920));
        //assert (parse_file_into_str ("vs.glsl", vertex_shader_str, 81920));
        //assert (parse_file_into_str ("fs.glsl", fragment_shader_str, 81920));
        
        GLuint vs, fs;
        vs = glCreateShader (GL_VERTEX_SHADER);
        fs = glCreateShader (GL_FRAGMENT_SHADER);
        glShaderSource (vs, 1, (const char**)&vertex_shader_str, NULL);
        glShaderSource (fs, 1, (const char**)&fragment_shader_str, NULL);
        // free memory
        free (vertex_shader_str);
        free (fragment_shader_str);
        glCompileShader (vs);
        glCompileShader (fs);
        shader_programme = glCreateProgram ();
        glAttachShader (shader_programme, fs);
        glAttachShader (shader_programme, vs);
        glLinkProgram (shader_programme);
        // TODO NOTE: you should check for errors and print logs after compiling and also linking shaders
        
    }
    
    
    
    // Start rendering
    // --------------------------------------------------------------------------
    // tell GL to only draw onto a pixel if the fragment is closer to the viewer
    
    
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable (GL_CULL_FACE); // cull face
    glCullFace (GL_BACK); // cull back face
    double lastTime = glfwGetTime();
    float xgate = 2.0f;
    float ygate = -2.0f;
    glm::vec3 position2 = glm::vec3(xgate, 1, 25 ); //set base
   // glm::vec3 boxpos [3] = {glm::vec3(0, 2, 25 ), glm::vec3(0, 0, 25 ), glm::vec3(0, -2, 25 )};
    glm::vec3 position3 = glm::vec3(ygate, 1, 25);  //room
    glm::vec3 position4 = glm::vec3(2, -5, 150);  //room

    // horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
    // vertical angle : 0, look at the horizon
    float verticalAngle = 0.0f;
    // Initial Field of View
    float initialFoV = 45.0f;
    float speed = 12.0f; // 3 units / second
    //float mouseSpeed = 0.05f;
    //float theta = 0.0f;     //rotation angle
    
    
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix, ViewMatrix2, ViewMatrix3, ViewMatrix4;
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    
    char simpboard[9] = "ship.obj";
    char box[14] = "dodgebox1.obj";
    char myroomtex[20] = "opengl_textures.jpg";
    char stars[14] = "boxtex2.png";
    char saturn[12] = "Saturno.obj";
    char satcol[11] = "saturn.png";
    init_text_rendering ("freemono.png", "freemono.meta", gl_width, gl_height);
   
    // x and y are -1 to 1
    // size_px is the maximum glyph size in pixels (try 100.0f)
    // r,g,b,a are red,blue,green,opacity values between 0.0 and 1.0
    // if you want to change the text later you will use the returned integer as a parameter
    float x = -0.65;
    float y = 0.5;
    float size_px = 200.0f;
    float r = 1.0 , g= 1.0, b= 1.0, a= 1.0;
    bool gameactive = true;
    
    char*  gameover = "GAME OVER";
    char*  startover = "Start again?(y/n)";
    char*  really = "Really??";
    char*  nice = "NICE!";
    bool passed = false;
    int nicecount = 0;
    bool leftpush = false;
    bool rightpush = false;
    int hello_id2 = add_text (really, x, y, size_px, r, g, b, a);
    int score = 0 ;     //initialise game score
    bool lplus = true;
    bool rplus = true;
    float zz = 0;
    int scount;
     glm::vec3 position = glm::vec3( 0, 0.75, 2 );  //lego man
    float th2 = 90.0f;
    vec3 brotation = vec3(0.0, 0.5, 0.0);
    mat4 ModelMatrixR2;
    float reduction = 10;
    ProjectionMatrix = glm::perspective(80.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 PlanetProjectionMatrix = glm::perspective(15.0f, 1.0f, 0.001f, 5000.0f);
    glm::mat4 ShipProjectionMatrix = glm::perspective(80.0f, 1.0f, 0.01f, 100.0f);

    GLuint texid;
   
    while (!glfwWindowShouldClose (window)) {
       
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor (backx, backy ,backz, 1.0);
        
        // just the default viewport, covering the whole render area
        glUseProgram (shader_programme);
        glBindVertexArray (vao);
        if(lplus == true && rplus == true){
            
            position = glm::vec3( 0, 0.75, 2 );  //lego man
        }
        else{
            position = glm::vec3(0, 0.75, 2);
            zz++;
        }
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        
        
        if (glfwGetKey(window, GLFW_KEY_R ) == GLFW_PRESS){
            
            position = glm::vec3( 0, 0.5, 6 );
            // horizontal angle : toward -Z
            horizontalAngle = 3.14f;
            // vertical angle : 0, look at the horizon
            verticalAngle = 0.0f;
            // Initial Field of View
            initialFoV = 45.0f;
            
            
            
            
        }
        
        // Direction : Spherical coordinates to Cartesian coordinates conversion
        glm::vec3 direction(cos(verticalAngle) * sin(horizontalAngle),sin(verticalAngle),cos(verticalAngle) * cos(horizontalAngle));
        
        // Right vector
        glm::vec3 right = glm::vec3(sin(horizontalAngle - 3.14f/2.0f),0, cos(horizontalAngle - 3.14f/2.0f));
        
        // Up vector : perpendicular to both direction and right
        glm::vec3 up = glm::cross( right, direction );
        
        float drift = 1.0f;
       // automatically move with extra drift based on most recent turn
        if(leftpush == true && gameactive == true){
            position2 -= right/reduction;
            position3 -= right/reduction;
            position4 -= right/(reduction*2);
         //   updatecoloursleft();
            
        }
        if(rightpush == true && gameactive == true){
            
            position2 += right/reduction;
            position3 += right/reduction;
            position4 += right/(reduction*2);
          //  updatecoloursright();
        }
        
        position2 = updateposition(position2, direction, deltaTime, speed);
        position3 = updateposition(position3, direction, deltaTime, speed);
               // Move backward
        if (glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS){
           // position -= direction * deltaTime * speed;
            position2 -= direction * deltaTime * speed;
            position3 -= direction * deltaTime * speed;
            
        }
        // Move right
        if (glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS){
            //position += right * deltaTime * speed;
            position2 += right * deltaTime * (speed/2.0f);
            position3 += right * deltaTime * speed/2.0f;
            rightpush = true;
            leftpush = false;
            brotation = vec3(0.0, 0.5, 0.05);

        }
        // Move left
        if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS){
            //position -= right * deltaTime * speed;
            position2 -= right * deltaTime * speed/2.0f;
            position3 -= right * deltaTime * speed/2.0f;
            leftpush = true;
            rightpush = false;
            brotation = vec3(0.05, 0.5, 0.0);

        }
        
        
       
        ViewMatrix       = glm::lookAt(
                                       position,           // Camera is here
                                       position+direction, // and looks here : at the same position, plus "direction"
                                       up                  // Head is up (set to 0,-1,0 to look upside-down)
                                       );
        
        
        
        
        
        ViewMatrix2       = glm::lookAt(
                                        position2,           // Camera is here
                                        position2+direction, // and looks here : at the same position, plus "direction"
                                        up                  // Head is up (set to 0,-1,0 to look upside-down)
                                        );
        
        
        ViewMatrix3       = glm::lookAt(
                                        position3,           // Camera is here
                                        position3+direction, // and looks here : at the same position, plus "direction"
                                        up                  // Head is up (set to 0,-1,0 to look upside-down)
                                        );
    
        ViewMatrix4       = glm::lookAt(
                                        position4,           // Camera is here
                                        position4+direction, // and looks here : at the same position, plus "direction"
                                        up                  // Head is up (set to 0,-1,0 to look upside-down)
                                        );
        
        
        if((int)position[2] >= (int)position2[2] && position[0] < position2[0] && position[0] >position3[0]){
            printf("%s\n", "Gate passed!!!");
            score++;
            //draw new gates
            if(position2[0] < 0){
                xgate = rand() %  8;
            }
            else{
                xgate = rand() % -8;
            }
             ygate = xgate - 4;
            float dist = rand() % 5 + 12;
            
            position2 = vec3(xgate,1, position[2]*dist);
            position3 = vec3(ygate ,1, position[2]*dist );
            //update saturn
            position4[2]-=2.5;
            position4[1]+=0.05;
            if(score % 3 == 0){
                printf("%s\n" ,"boost");
                //speed+=0.5;
                //update_text(hello_id2, nice);
                //draw_texts();
            }
         //  draw_texts();
            
        }
        if((int)position[2] >= (int)position2[2] && position[0] > position2[0] && position[0] >position3[0]){
            printf("%s\n", "Gate missed left!!!");
            rplus = false; //rotate out

        }
        
        if((int)position[2] >= (int)position2[2] && position[0] < position2[0] && position[0] < position3[0]){
            printf("%s\n", "Gate missed right!!!");
            lplus = false; //rotate out
        }
        //board
        if(rplus == false || lplus == false)
        {
            //initiate spin
            brotation = vec3(2.0, 3.0, 0);
            th2+=20;
            gameactive = false;
            update_text(hello_id2, gameover);
            //switch to texture for text.
            position4 = moveplanet(position4);
            draw_texts();
        }
        
        
        glUseProgram (shader_programme);
        int point_count = loadmesh(simpboard);
        
        int M_loc2 = glGetUniformLocation (shader_programme, "M");
        ModelMatrixR2 = rotate(ModelMatrix, th2 ,brotation);
      //  ModelMatrixR2 = glm::scale(ModelMatrixR2, vec3(0.05, 0.05, 0.05));
        
        glUniformMatrix4fv (M_loc2, 1, GL_FALSE, glm::value_ptr(ModelMatrixR2));
        int utrans2 = glGetUniformLocation(shader_programme, "P");
        glUniformMatrix4fv(utrans2, 1, GL_FALSE, glm::value_ptr(ShipProjectionMatrix));
        int V_loc2 = glGetUniformLocation(shader_programme, "V");
        glUniformMatrix4fv(V_loc2, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
        
        ltexture(myroomtex);
        GLuint textureLoc = glGetUniformLocation(shader_programme, "btex");
        glUniform1i(textureLoc, 1);
       
        GLuint light = glGetUniformLocation(shader_programme, "light_position_world");
        vec3 newlight = vec3(position[0], position[1], position[2]);//vec3(0,5, position[2]);
        glUniformMatrix4fv(light, 1, GL_FALSE, glm::value_ptr(newlight));
        
        glDrawArrays( GL_TRIANGLES, 0, point_count );
        
        //surface
        
        int rcount = loadmesh(box);
        
        
        M_loc2 = glGetUniformLocation (shader_programme, "M");
        glUniformMatrix4fv (M_loc2, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
         utrans2 = glGetUniformLocation(shader_programme, "P");
        glUniformMatrix4fv(utrans2, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
        
        V_loc2 = glGetUniformLocation(shader_programme, "V");
        glUniformMatrix4fv(V_loc2, 1, GL_FALSE, glm::value_ptr(ViewMatrix2));
        light = glGetUniformLocation(shader_programme, "light_position_world");
         newlight = position;//vec3(0,5, position[2]);
        glUniformMatrix4fv(light, 1, GL_FALSE, glm::value_ptr(newlight));

        
     
        ltexture(stars);
         textureLoc = glGetUniformLocation(shader_programme, "btex");
        glUniform1i(textureLoc, 1);
        glDrawArrays( GL_TRIANGLES, 0, rcount );
        
        
        
       V_loc2 = glGetUniformLocation(shader_programme, "V");
        glUniformMatrix4fv(V_loc2, 1, GL_FALSE, glm::value_ptr(ViewMatrix3));
        glDrawArrays (GL_TRIANGLES, 0, rcount);
//        glBindVertexArray(0);

        
        utrans2 = glGetUniformLocation(shader_programme, "P");
        glUniformMatrix4fv(utrans2, 1, GL_FALSE, glm::value_ptr(PlanetProjectionMatrix));
        
        glUniformMatrix4fv(V_loc2, 1, GL_FALSE, glm::value_ptr(ViewMatrix4));
        
        light = glGetUniformLocation(shader_programme, "light_position_world");
         newlight = position;//vec3(0,5, position[2]);
        glUniformMatrix4fv(light, 1, GL_FALSE, glm::value_ptr(newlight));

        scount = loadmesh(saturn);
        
        ltexture(satcol);
        textureLoc = glGetUniformLocation(shader_programme, "btex");
        glUniform1i(textureLoc, 1);
        glDrawArrays( GL_TRIANGLES, 0, scount );
        
        
        lastTime = currentTime;
        /* this just updates window events and keyboard input events (not used yet) */
        glfwPollEvents ();
        glfwSwapBuffers (window);
        
    }
    
    
    return 0;
}
