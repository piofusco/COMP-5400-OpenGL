OpenGL graphics programming in C++.

COMP 5400 Programming Assignment 6: Collision Avoidance & Animation
Duration: Two Weeks (100 points)

Requirements:
1. The window size should be at least 800x800.

2. The window's title should be "COMP-5/6400Assignment 6."

3. This assignment should be an extension of your
assignment 5. However, you can always improve what you have in assignment 5.

4. The main additional requirements are:
a) to allow the user to move the “camera” around the environment
b) to add collision avoidance feature. This means people or objects cannot penetrate each other and a student cannot pass through a wall.
c) to define an automatic tour path that will show off interesting features in your design. This can be achieved by hitting a specific key and the automatic tour takes place
d) to add a spotlight to the camera that aims at the point where the camera is pointing to (you may have this in Assignment 5 already)
e) to allow the user to maneuver multiple students and/or other objects, e.g., buses, in the environment. (Optional) Or magically add/remove booths

5. Control of objects (students, etc.) and the viewpoint (camera) must be separate, but can be done simultaneously.

6. Obviously the user can only choose one mode between 4.a and 4.c. However, the object control for 4.e is independent from 4.a and 4.c.

7. The viewing must be done using perspective projection.

8. The minimum requirements for the camera control include: move forward/backward, turn right and left, and look upward and downward. Again common sense applies, e.g., cannot pass through any objects.

9. A switch is needed to switch to/from the spotlight mode.

10. The lighting control and effects from assignment 5 must
be kept.

11. Try to show off the highlights of your environment.


Additional comments:
- T begins tour. 1-4 will toggle lights
- WASD for basic movement. Q and E raise and lower. 
- IJKL controls student. 
- ESC brings you back to the origin. 
- Decent FPS style strafing is commented out in the preProcessing() method in main. 
- Basic wall collision for the camera and student model, but it doesn't work on every wall.
