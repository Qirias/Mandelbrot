# Calculating the Mandelbrot set using a compute shader.
This project is my introduction to compute shaders with OpenGL.
# Prerequisites
You'll need the g++ compiler and the glfw libary.<br><br>
For **DEBIAN**
```
sudo apt-get install g++
sudo apt-get install libglfw
sudo apt-get install libglfw-dev
```

For **MANJARO**
```
sudo pacman -S g++
sudo pacman -S glfw-x11
```

**Clone the project** <br>
```git clone https://github.com/Qirias/Mandelbrot.git```

**And finally to compile and run the program**<br>
```g++ main.cpp -lGL -lglfw && ./a.out```
# Controls
* **UP/DOWN/RIGHT/LEFT** arrows are for the movement.
* **I/U** increase and decrease the number of iterations by 10, respectively.
* **Z/CTRL+Z** zooming and unzooming.
* **L** shows the two triangles the texture is applied on. <br><br>
**If you wanna know how this works check Iñigo's Quilez paper** [Rendering Worlds with Two Triangles](https://www.iquilezles.org/www/material/nvscene2008/rwwtt.pdf)

# Showcase
![Screenshot from 2022-08-10 15-56-24](https://user-images.githubusercontent.com/49881189/183907311-b5b01d25-97e0-4d12-883c-b113be2c7b15.png)
![Screenshot from 2022-08-10 15-54-56](https://user-images.githubusercontent.com/49881189/183907395-33720f9b-d69c-4a80-88a7-7742dc35f078.png)
![gif](mandelbrot.gif)
