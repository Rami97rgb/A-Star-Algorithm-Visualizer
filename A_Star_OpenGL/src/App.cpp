#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <list>
#include <shader.h>
#include <list>
#include <iostream>

//glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window);

//get mouse cursor position
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

//get mouse boutton presses
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

//calculate distance between two nodes
float distance(float x1, float y1, float x2, float y2);

//window dimensions
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;



int main()
{

    //previous time variable
    double previousTime = glfwGetTime();

    //fps variable
    int frameCount = 0;

    //node structure
    struct node
    {
        float x;
        float y;
        bool isvisited;
        bool isobstacle;
        bool ispath;
        node* parent;
        float global_v;
        float loacal_v;
        std::vector<node*> neighbors;

    };

    //define grid as 20 * 20 nodes
    node* nodes = nullptr;
    int gridwidth = 20;
    int gridheight = 20;

    //initialize grid
    nodes = new node[gridwidth * gridheight];
    for (int i=0; i < gridwidth; i++)
        for (int j = 0; j < gridheight; j++)
        {
            nodes[j * gridheight + i].x = i;
            nodes[j * gridheight + i].y = j;
            nodes[j * gridheight + i].isobstacle = false;
            nodes[j * gridheight + i].isvisited = false;
            nodes[j * gridheight + i].ispath = false;
            nodes[j * gridheight + i].parent = nullptr;
        }

    //get nodes neighbors
    for (int i = 0; i < gridwidth; i++)
        for (int j = 0; j < gridheight; j++)
        {
            if (j > 0)
                nodes[j * gridheight + i].neighbors.push_back(&nodes[(j - 1) * gridheight + i]);
            if (i > 0)
                nodes[j * gridheight + i].neighbors.push_back(&nodes[j * gridheight + (i - 1)]);
            if (j < (gridheight - 1))
                nodes[j * gridheight + i].neighbors.push_back(&nodes[(j + 1) * gridheight + i]);
            if (i < (gridwidth - 1))
                nodes[j * gridheight + i].neighbors.push_back(&nodes[j * gridheight + (i + 1)]);

        }
    
    //initialize start and end nodes
    node* startnode = &nodes[gridheight * (gridheight / 4) + gridwidth / 4];
    node* endnode = &nodes[3 * (gridheight * (gridheight / 4) + gridwidth / 4)];



    //initialize and configure glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//assure compatibility with apple devices 
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    //glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //build and compile the shader program
    Shader ourShader("3.3.shader.vs", "3.3.shader.fs"); // you can name your shader files however you like

    //set up vertex data for first node represented as a rectangle
    float vertices[] = {
        // positions         // colors
        -0.995f,  0.995f, 0.0f,  1.0f, 0.0f, 0.0f,  
        -0.905f,  0.995f, 0.0f,  1.0f, 0.0f, 0.0f,  
        -0.995f,  0.905f, 0.0f,  1.0f, 0.0f, 0.0f,
        -0.905f,  0.905f, 0.0f,  1.0f, 0.0f, 0.0f
    };

    //set up indices for first rectangle (two triagnles combined)
    unsigned int indices[] = {  
       0, 1, 2,  // first triangle
       1, 2, 3,  //second triangle
    };

    

    //cursor coordinates
    double xpos, ypos;

    //two state variables
    int state1;
    int state2;

    // vertex & index pointers
    float* vertex = nullptr;
    unsigned int* index = nullptr;

    //vertex buffer object, vetex array object and element buffer obeject
    unsigned int VBO, VAO, EBO;

    //current time variable
    double currentTime;

    //render loop
    while (!glfwWindowShouldClose(window))
    {
        
        //get cursor positions
        glfwGetCursorPos(window, &xpos, &ypos);

        //get mouse button presse
        state1 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

        //set start node position if 's' is pressed
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && state1 == GLFW_PRESS && state2 == GLFW_RELEASE && xpos < SCR_WIDTH && xpos > 0 && ypos < SCR_HEIGHT && ypos > 0 && !nodes[(int(ypos) / 30) * gridheight + (int(xpos) / 30)].isobstacle && endnode != &nodes[(int(ypos) / 30) * gridheight + (int(xpos) / 30)])
            startnode = &nodes[(int(ypos) / 30) * gridheight + (int(xpos) / 30)];

        //set end node position if 'e' is pressed
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && state1 == GLFW_PRESS && state2 == GLFW_RELEASE && xpos < SCR_WIDTH && xpos > 0 && ypos < SCR_HEIGHT && ypos > 0 && !nodes[(int(ypos) / 30) * gridheight + (int(xpos) / 30)].isobstacle && startnode != &nodes[(int(ypos) / 30) * gridheight + (int(xpos) / 30)])
            endnode = &nodes[(int(ypos) / 30) * gridheight + (int(xpos) / 30)];

        //set obstacles
        if (state1 == GLFW_PRESS && state2 == GLFW_RELEASE && xpos < SCR_WIDTH && xpos > 0 && ypos < SCR_HEIGHT && ypos > 0 && &nodes[(int(ypos) / 30) * gridheight + (int(xpos) / 30)] != startnode && &nodes[(int(ypos) / 30) * gridheight + (int(xpos) / 30)] != endnode)
        {
            nodes[(int(ypos) / 30) * gridheight + (int(xpos) / 30)].isobstacle = !nodes[(int(ypos) / 30) * gridheight + (int(xpos) / 30)].isobstacle; 
        }

        //get mouse button press
        state2 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        
        //initialize node variables
        for (int i = 0; i < gridwidth; i++)
            for (int j = 0; j < gridheight; j++)
            {
                nodes[j * gridheight + i].global_v = INFINITY;
                nodes[j * gridheight + i].loacal_v = INFINITY;
                nodes[j * gridheight + i].isvisited = false;
                nodes[j * gridheight + i].ispath = false;
                nodes[j * gridheight + i].parent = nullptr;
            }
        
        //initialize start and end nodes variables
        std::list<node*> listofnodes;
        node* currentnode = startnode;
        startnode->loacal_v = 0;
        startnode->global_v = distance(startnode->x, startnode->y, endnode->x, endnode->y);
        listofnodes.push_back(startnode);

        //start algorithm
        if (glfwGetKey(window, GLFW_KEY_SPACE))
        {

            //algorithm loop
            while (!listofnodes.empty() && currentnode != endnode)
            {
                
                glfwWaitEventsTimeout(0.05);

                //for each iteration sort list of nodes and pop front node if it is already visited until front node is not visited
                listofnodes.sort([](const node* a, const node* b) { return a->global_v < b->global_v; });
                while (!listofnodes.empty() && listofnodes.front()->isvisited)
                    listofnodes.pop_front();
                
                //terminate if list of nodes is empty
                if (listofnodes.empty())
                    break;
                
                //set current node to front node and mark it as visited
                currentnode = listofnodes.front();
                currentnode->isvisited = true;

                //loop over neighbors
                for (auto neinode : currentnode->neighbors)
                {
                    //if node is not visited and not an obstacle to push it to list of nodes
                    if (!neinode->isvisited && neinode->isobstacle == 0)
                    {
                        listofnodes.push_back(neinode);
                    }

                    //set parent, global value and local value of the neighbor
                    if ((currentnode->loacal_v + 1) < neinode->loacal_v)
                    {
                        neinode->parent = currentnode;
                        neinode->loacal_v = currentnode->loacal_v + 1;
                        neinode->global_v = neinode->loacal_v + distance(neinode->x, neinode->y, endnode->x, endnode->y);
                    }

                }

                //set vertex values for each rectangle
                vertex = new float[gridwidth * gridheight * 24];
                for (int i = 0; i < gridwidth; i++)
                    for (int j = 0; j < gridheight; j++)
                    {
                        vertex[24 * (j * gridheight + i) + 0] = vertices[0] + i * 0.1f;
                        vertex[24 * (j * gridheight + i) + 1] = vertices[1] - j * 0.1f;
                        vertex[24 * (j * gridheight + i) + 2] = vertices[2];
                        vertex[24 * (j * gridheight + i) + 3] = !nodes[(j * gridheight + i)].isobstacle;
                        vertex[24 * (j * gridheight + i) + 4] = (&nodes[(j * gridheight + i)] == startnode) + 0.5f * nodes[(j * gridheight + i)].isvisited + (&nodes[(j * gridheight + i)] == endnode);
                        vertex[24 * (j * gridheight + i) + 5] = (&nodes[(j * gridheight + i)] == endnode) + 0.5f * nodes[(j * gridheight + i)].ispath;
                        vertex[24 * (j * gridheight + i) + 6] = vertices[6] + i * 0.1f;
                        vertex[24 * (j * gridheight + i) + 7] = vertices[7] - j * 0.1f;
                        vertex[24 * (j * gridheight + i) + 8] = vertices[8];
                        vertex[24 * (j * gridheight + i) + 9] = !nodes[(j * gridheight + i)].isobstacle;
                        vertex[24 * (j * gridheight + i) + 10] = (&nodes[(j * gridheight + i)] == startnode) + 0.5f * nodes[(j * gridheight + i)].isvisited + (&nodes[(j * gridheight + i)] == endnode);
                        vertex[24 * (j * gridheight + i) + 11] = (&nodes[(j * gridheight + i)] == endnode) + 0.5f * nodes[(j * gridheight + i)].ispath;
                        vertex[24 * (j * gridheight + i) + 12] = vertices[12] + i * 0.1f;
                        vertex[24 * (j * gridheight + i) + 13] = vertices[13] - j * 0.1f;
                        vertex[24 * (j * gridheight + i) + 14] = vertices[14];
                        vertex[24 * (j * gridheight + i) + 15] = !nodes[(j * gridheight + i)].isobstacle;
                        vertex[24 * (j * gridheight + i) + 16] = (&nodes[(j * gridheight + i)] == startnode) + 0.5f * nodes[(j * gridheight + i)].isvisited + (&nodes[(j * gridheight + i)] == endnode);
                        vertex[24 * (j * gridheight + i) + 17] = (&nodes[(j * gridheight + i)] == endnode) + 0.5f * nodes[(j * gridheight + i)].ispath;
                        vertex[24 * (j * gridheight + i) + 18] = vertices[18] + i * 0.1f;
                        vertex[24 * (j * gridheight + i) + 19] = vertices[19] - j * 0.1f;
                        vertex[24 * (j * gridheight + i) + 20] = vertices[20];
                        vertex[24 * (j * gridheight + i) + 21] = !nodes[(j * gridheight + i)].isobstacle;
                        vertex[24 * (j * gridheight + i) + 22] = (&nodes[(j * gridheight + i)] == startnode) + 0.5f * nodes[(j * gridheight + i)].isvisited + (&nodes[(j * gridheight + i)] == endnode);
                        vertex[24 * (j * gridheight + i) + 23] = (&nodes[(j * gridheight + i)] == endnode) + 0.5f * nodes[(j * gridheight + i)].ispath;
                    }


                //set indices for reach rectangle
                index = new unsigned int[gridwidth * gridheight * 6];
                for (int i = 0; i < gridwidth; i++)
                    for (int j = 0; j < gridheight; j++)
                    {
                        index[6 * (j * gridheight + i) + 0] = indices[0] + 4 * (j * gridheight + i);
                        index[6 * (j * gridheight + i) + 1] = indices[1] + 4 * (j * gridheight + i);
                        index[6 * (j * gridheight + i) + 2] = indices[2] + 4 * (j * gridheight + i);
                        index[6 * (j * gridheight + i) + 3] = indices[3] + 4 * (j * gridheight + i);
                        index[6 * (j * gridheight + i) + 4] = indices[4] + 4 * (j * gridheight + i);
                        index[6 * (j * gridheight + i) + 5] = indices[5] + 4 * (j * gridheight + i);

                    }



                //generate VAO, VBO and EBO
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glGenBuffers(1, &EBO);
                //bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
                glBindVertexArray(VAO);

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9600, vertex, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 2400, index, GL_STATIC_DRAW);

                // position attribute
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                // color attribute
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);

                //process input
                processInput(window);

                //clear window
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                //render the triangles
                ourShader.use();
                glBindVertexArray(VAO);

                glDrawElements(GL_TRIANGLES, 2400, GL_UNSIGNED_INT, 0);



                //glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
                glfwSwapBuffers(window);
                glfwPollEvents();
                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);
                glDeleteBuffers(1, &EBO);
                delete[] vertex;
                delete[] index;


            }

            //draw path
            node* pathnode = endnode->parent;
            if (endnode->parent != nullptr)
            {   
                //parse path starting with end node and stopping with start node
                while (pathnode->parent != nullptr)
                {
                    pathnode->ispath = true;
                    pathnode = pathnode->parent;

                    glfwWaitEventsTimeout(0.05);

                    //change color of path nodes
                    vertex = new float[gridwidth * gridheight * 24];
                    for (int i = 0; i < gridwidth; i++)
                        for (int j = 0; j < gridheight; j++)
                        {
                            vertex[24 * (j * gridheight + i) + 0] = vertices[0] + i * 0.1f;
                            vertex[24 * (j * gridheight + i) + 1] = vertices[1] - j * 0.1f;
                            vertex[24 * (j * gridheight + i) + 2] = vertices[2];
                            vertex[24 * (j * gridheight + i) + 3] = !nodes[(j * gridheight + i)].isobstacle;
                            vertex[24 * (j * gridheight + i) + 4] = (&nodes[(j * gridheight + i)] == startnode) + 0.5f * nodes[(j * gridheight + i)].isvisited + (&nodes[(j * gridheight + i)] == endnode);
                            vertex[24 * (j * gridheight + i) + 5] = (&nodes[(j * gridheight + i)] == endnode) + 0.5f * nodes[(j * gridheight + i)].ispath;
                            vertex[24 * (j * gridheight + i) + 6] = vertices[6] + i * 0.1f;
                            vertex[24 * (j * gridheight + i) + 7] = vertices[7] - j * 0.1f;
                            vertex[24 * (j * gridheight + i) + 8] = vertices[8];
                            vertex[24 * (j * gridheight + i) + 9] = !nodes[(j * gridheight + i)].isobstacle;
                            vertex[24 * (j * gridheight + i) + 10] = (&nodes[(j * gridheight + i)] == startnode) + 0.5f * nodes[(j * gridheight + i)].isvisited + (&nodes[(j * gridheight + i)] == endnode);
                            vertex[24 * (j * gridheight + i) + 11] = (&nodes[(j * gridheight + i)] == endnode) + 0.5f * nodes[(j * gridheight + i)].ispath;
                            vertex[24 * (j * gridheight + i) + 12] = vertices[12] + i * 0.1f;
                            vertex[24 * (j * gridheight + i) + 13] = vertices[13] - j * 0.1f;
                            vertex[24 * (j * gridheight + i) + 14] = vertices[14];
                            vertex[24 * (j * gridheight + i) + 15] = !nodes[(j * gridheight + i)].isobstacle;
                            vertex[24 * (j * gridheight + i) + 16] = (&nodes[(j * gridheight + i)] == startnode) + 0.5f * nodes[(j * gridheight + i)].isvisited + (&nodes[(j * gridheight + i)] == endnode);
                            vertex[24 * (j * gridheight + i) + 17] = (&nodes[(j * gridheight + i)] == endnode) + 0.5f * nodes[(j * gridheight + i)].ispath;
                            vertex[24 * (j * gridheight + i) + 18] = vertices[18] + i * 0.1f;
                            vertex[24 * (j * gridheight + i) + 19] = vertices[19] - j * 0.1f;
                            vertex[24 * (j * gridheight + i) + 20] = vertices[20];
                            vertex[24 * (j * gridheight + i) + 21] = !nodes[(j * gridheight + i)].isobstacle;
                            vertex[24 * (j * gridheight + i) + 22] = (&nodes[(j * gridheight + i)] == startnode) + 0.5f * nodes[(j * gridheight + i)].isvisited + (&nodes[(j * gridheight + i)] == endnode);
                            vertex[24 * (j * gridheight + i) + 23] = (&nodes[(j * gridheight + i)] == endnode) + 0.5f * nodes[(j * gridheight + i)].ispath;
                        }




                    index = new unsigned int[gridwidth * gridheight * 6];
                    for (int i = 0; i < gridwidth; i++)
                        for (int j = 0; j < gridheight; j++)
                        {
                            index[6 * (j * gridheight + i) + 0] = indices[0] + 4 * (j * gridheight + i);
                            index[6 * (j * gridheight + i) + 1] = indices[1] + 4 * (j * gridheight + i);
                            index[6 * (j * gridheight + i) + 2] = indices[2] + 4 * (j * gridheight + i);
                            index[6 * (j * gridheight + i) + 3] = indices[3] + 4 * (j * gridheight + i);
                            index[6 * (j * gridheight + i) + 4] = indices[4] + 4 * (j * gridheight + i);
                            index[6 * (j * gridheight + i) + 5] = indices[5] + 4 * (j * gridheight + i);

                        }




                    glGenVertexArrays(1, &VAO);
                    glGenBuffers(1, &VBO);
                    glGenBuffers(1, &EBO);
                    //bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
                    glBindVertexArray(VAO);

                    glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9600, vertex, GL_STATIC_DRAW);

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 2400, index, GL_STATIC_DRAW);

                    //position attribute
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
                    glEnableVertexAttribArray(0);
                    // color attribute
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
                    glEnableVertexAttribArray(1);

                    //input
                    processInput(window);

                    //render
                    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT);

                    //render the triangle
                    ourShader.use();
                    glBindVertexArray(VAO);
                    //glDrawArrays(GL_TRIANGLES, 0, 3);
                    glDrawElements(GL_TRIANGLES, 2400, GL_UNSIGNED_INT, 0);

                    //glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
                    glfwSwapBuffers(window);
                    glfwPollEvents();
                    glDeleteVertexArrays(1, &VAO);
                    glDeleteBuffers(1, &VBO);
                    glDeleteBuffers(1, &EBO);
                    delete[] vertex;
                    delete[] index;
                }
            }
            

            while (!glfwGetKey(window, GLFW_KEY_SPACE))
            {

                glfwWaitEventsTimeout(0.1);

            }

        }

        glfwWaitEventsTimeout(0.2);

        //reset window to default grid after finishing
        vertex = new float[gridwidth * gridheight * 24];
        for (int i = 0; i < gridwidth; i++)
            for (int j = 0; j < gridheight; j++)
            {
                vertex[24 * (j * gridheight + i) + 0] = vertices[0] + i * 0.1f;
                vertex[24 * (j * gridheight + i) + 1] = vertices[1] - j * 0.1f;
                vertex[24 * (j * gridheight + i) + 2] = vertices[2];
                vertex[24 * (j * gridheight + i) + 3] = !nodes[(j * gridheight + i)].isobstacle;
                vertex[24 * (j * gridheight + i) + 4] = (&nodes[(j * gridheight + i)] == startnode) + 0.5f * nodes[(j * gridheight + i)].isvisited + (&nodes[(j * gridheight + i)] == endnode);
                vertex[24 * (j * gridheight + i) + 5] = (&nodes[(j * gridheight + i)] == endnode) + 0.5f * nodes[(j * gridheight + i)].ispath;
                vertex[24 * (j * gridheight + i) + 6] = vertices[6] + i * 0.1f;
                vertex[24 * (j * gridheight + i) + 7] = vertices[7] - j * 0.1f;
                vertex[24 * (j * gridheight + i) + 8] = vertices[8];
                vertex[24 * (j * gridheight + i) + 9] = !nodes[(j * gridheight + i)].isobstacle;
                vertex[24 * (j * gridheight + i) + 10] = (&nodes[(j * gridheight + i)] == startnode) + 0.5f * nodes[(j * gridheight + i)].isvisited + (&nodes[(j * gridheight + i)] == endnode);
                vertex[24 * (j * gridheight + i) + 11] = (&nodes[(j * gridheight + i)] == endnode) + 0.5f * nodes[(j * gridheight + i)].ispath;
                vertex[24 * (j * gridheight + i) + 12] = vertices[12] + i * 0.1f;
                vertex[24 * (j * gridheight + i) + 13] = vertices[13] - j * 0.1f;
                vertex[24 * (j * gridheight + i) + 14] = vertices[14];
                vertex[24 * (j * gridheight + i) + 15] = !nodes[(j * gridheight + i)].isobstacle;
                vertex[24 * (j * gridheight + i) + 16] = (&nodes[(j * gridheight + i)] == startnode) + 0.5f * nodes[(j * gridheight + i)].isvisited + (&nodes[(j * gridheight + i)] == endnode);
                vertex[24 * (j * gridheight + i) + 17] = (&nodes[(j * gridheight + i)] == endnode) + 0.5f * nodes[(j * gridheight + i)].ispath;
                vertex[24 * (j * gridheight + i) + 18] = vertices[18] + i * 0.1f;
                vertex[24 * (j * gridheight + i) + 19] = vertices[19] - j * 0.1f;
                vertex[24 * (j * gridheight + i) + 20] = vertices[20];
                vertex[24 * (j * gridheight + i) + 21] = !nodes[(j * gridheight + i)].isobstacle;
                vertex[24 * (j * gridheight + i) + 22] = (&nodes[(j * gridheight + i)] == startnode) + 0.5f * nodes[(j * gridheight + i)].isvisited + (&nodes[(j * gridheight + i)] == endnode);
                vertex[24 * (j * gridheight + i) + 23] = (&nodes[(j * gridheight + i)] == endnode) + 0.5f * nodes[(j * gridheight + i)].ispath;
            }



        
        index = new unsigned int[gridwidth * gridheight * 6];
        for (int i = 0; i < gridwidth; i++)
            for (int j = 0; j < gridheight; j++)
            {
                index[6 * (j * gridheight + i) + 0] = indices[0] + 4 * (j * gridheight + i);
                index[6 * (j * gridheight + i) + 1] = indices[1] + 4 * (j * gridheight + i);
                index[6 * (j * gridheight + i) + 2] = indices[2] + 4 * (j * gridheight + i);
                index[6 * (j * gridheight + i) + 3] = indices[3] + 4 * (j * gridheight + i);
                index[6 * (j * gridheight + i) + 4] = indices[4] + 4 * (j * gridheight + i);
                index[6 * (j * gridheight + i) + 5] = indices[5] + 4 * (j * gridheight + i);

            }



        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        //bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9600, vertex, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 2400, index, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        //input
        processInput(window);

        //render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //render the triangle
        ourShader.use();
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 2400, GL_UNSIGNED_INT, 0);

        

        //glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        delete[] vertex;
        delete[] index;

        // Measure speed
        currentTime = glfwGetTime();
        frameCount++;
        // If a second has passed.
        if (currentTime - previousTime >= 1.0)
        {
            frameCount = 0;
            previousTime = currentTime;
        }

    }
   

    //glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
}

float distance(float x1, float y1, float x2, float y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
