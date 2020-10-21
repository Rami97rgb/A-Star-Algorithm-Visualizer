# A-Star-Algorithm-Visualizer
Using C++ and modern OpenGL, I implemented the A-Star pathfinding algorithm and visualized it in the form of a grid with start and end nodes and obstacle nodes.

## Requirements
* To use this visualizer, you should first make sure you have OpenGL version 3.3+ installed.
* You shoud also download GLFW and Glad.

## Usage
To start the visualizer, run the "App.c++" file in the "\A-Star-Algorithm-Visualizer\A_Star_OpenGL\src" folder. A 20x20 grid of nodes will be displayed:

![alt text](https://github.com/Rami97rgb/A-Star-Algorithm-Visualizer/blob/master/images/astar1.png)

To change the start node position hold "s" and select new position with mouse.

To change the end node position hold "e" and select new position with mouse.

To turn a node into an obstacle click on it with mouse.

![alt text](https://github.com/Rami97rgb/A-Star-Algorithm-Visualizer/blob/master/images/astar2.png)

To start the A-Star algorithm press the Space key.

![alt text](https://github.com/Rami97rgb/A-Star-Algorithm-Visualizer/blob/master/images/astar3.png)

The orange nodes represent visited nodes.

![alt text](https://github.com/Rami97rgb/A-Star-Algorithm-Visualizer/blob/master/images/astar4.png)

After the algorithm finishes executing, the shortest path will be displayed. Press the Space key to resest.

## Resources
A-start algorithm explanation: https://www.youtube.com/watch?v=icZj67PTFhc

Learning OpenGL: https://learnopengl.com/

